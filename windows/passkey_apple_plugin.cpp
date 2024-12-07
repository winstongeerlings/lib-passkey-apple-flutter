#include "passkey_apple_plugin.h"

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <windows.h>

namespace passkey_apple {

HANDLE g_hMutex = NULL;
HANDLE g_hEvent = NULL;
std::wstring g_token;

bool IsAlreadyRunning() {
  g_hMutex = CreateMutex(NULL, TRUE, L"PasskeyApplePluginMutex");
  if (GetLastError() == ERROR_ALREADY_EXISTS) {
    return true;
  }
  return false;
}

std::string WideStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}

void RegisterCustomURLScheme(const std::wstring& scheme, const std::wstring& applicationPath) {
  HKEY hKey;
  std::wstring keyPath = L"SOFTWARE\\Classes\\" + scheme;

  // Create the registry key for the custom URL scheme
  if (RegCreateKeyEx(HKEY_CURRENT_USER, keyPath.c_str(), 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
    std::wstring urlProtocol = L"URL:" + scheme + L" Protocol";
    RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)urlProtocol.c_str(), static_cast<DWORD>((urlProtocol.size() + 1) * sizeof(wchar_t)));
    RegSetValueEx(hKey, L"URL Protocol", 0, REG_SZ, (BYTE*)L"", sizeof(wchar_t));

    HKEY hSubKey;
    if (RegCreateKeyEx(hKey, L"shell\\open\\command", 0, NULL, 0, KEY_WRITE, NULL, &hSubKey, NULL) == ERROR_SUCCESS) {
      std::wstring command = L"\"" + applicationPath + L"\" \"%1\"";
      RegSetValueEx(hSubKey, NULL, 0, REG_SZ, (BYTE*)command.c_str(), static_cast<DWORD>((command.size() + 1) * sizeof(wchar_t)));
      RegCloseKey(hSubKey);
    }
    RegCloseKey(hKey);
  }
}

HWND PasskeyApplePlugin::mainWindowHandle = nullptr;

void PasskeyApplePlugin::RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar) {
  auto channel = std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
      registrar->messenger(), "passkey_apple",
      &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<PasskeyApplePlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

PasskeyApplePlugin::PasskeyApplePlugin() {
  if (IsAlreadyRunning()) {
    // Send the URL to the running instance
    HWND hwnd = FindWindow(NULL, L"PasskeyApplePluginWindow");
    if (hwnd) {
      COPYDATASTRUCT cds;
      cds.dwData = 1;
      cds.cbData = (DWORD)(wcslen(GetCommandLine()) + 1) * sizeof(wchar_t);
      cds.lpData = (PVOID)GetCommandLine();
      SendMessage(hwnd, WM_COPYDATA, (WPARAM)hwnd, (LPARAM)(LPVOID)&cds);
    }
    exit(0);
  }

  wchar_t path[MAX_PATH];
  GetModuleFileName(NULL, path, MAX_PATH);
  RegisterCustomURLScheme(L"<scheme>", path);

  // Register the window class with WindowProc
  WNDCLASS wc = {0};
  wc.lpfnWndProc = PasskeyApplePlugin::WindowProc;
  wc.hInstance = GetModuleHandle(NULL);
  wc.lpszClassName = L"PasskeyApplePluginWindowClass";

  RegisterClass(&wc);

  mainWindowHandle = CreateWindow(
      wc.lpszClassName, L"PasskeyApplePluginWindow", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
      NULL, NULL, wc.hInstance, NULL);

  // Ensure the main window handle is correctly set
  SetWindowText(mainWindowHandle, L"PasskeyApplePluginWindow");

  // Create the event
  g_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

PasskeyApplePlugin::~PasskeyApplePlugin() {
    if (g_hMutex) {
        ReleaseMutex(g_hMutex);
        CloseHandle(g_hMutex);
    }
    if (g_hEvent) {
        CloseHandle(g_hEvent);
    }
}

void PasskeyApplePlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("signIn") == 0) {
    SignInWithApple(std::move(result));
  } else {
    result->NotImplemented();
  }
}

void PasskeyApplePlugin::SignInWithApple(
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  // URL of the Apple login page
  // TODO: inputs need to come from the Dart side
    const wchar_t* url = L"https://appleid.apple.com/auth/authorize"
                         L"?client_id=<id>>"
                         L"&response_type=code id_token"
                         L"&scope=name%20email"
                         L"&response_mode=form_post"
                         L"&redirect_uri=<redirect>>";

  // Open the default browser with the Apple login page
  HINSTANCE hInstance = ShellExecute(
      nullptr, L"open", url, nullptr, nullptr, SW_SHOWNORMAL);

  if (reinterpret_cast<intptr_t>(hInstance) <= 32) {
    result->Error("BrowserError", "Failed to open the default browser");
    return;
  }

  // Create a new thread to wait for the event
  std::thread([result = std::move(result)]() mutable {
    // Wait for the event to be signaled
    WaitForSingleObject(g_hEvent, INFINITE);

    // Usage
    std::string token_str = WideStringToString(g_token);
    result->Success(flutter::EncodableValue(token_str));
//    result->Success(flutter::EncodableValue("token-that-was-received"));
  }).detach();
}

LRESULT CALLBACK PasskeyApplePlugin::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (uMsg == WM_COPYDATA) {
    PCOPYDATASTRUCT pCopyData = (PCOPYDATASTRUCT)lParam;
    std::wstring url((wchar_t*)pCopyData->lpData, pCopyData->cbData / sizeof(wchar_t));

    // Print url
    std::wcout << url << std::endl;

   // Check if the URL contains the redirect URI
   if (url.find(L"<scheme>://callback") != std::wstring::npos) {
       // Extract the token from the URL
       size_t token_pos = url.find(L"token=");
       if (token_pos != std::wstring::npos) {
           g_token = url.substr(token_pos + 6);

           // Signal the event
           SetEvent(g_hEvent);

           // There is still this issue where on callback to the app, the app is not brought to the front
       }
   }
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

}  // namespace passkey_apple