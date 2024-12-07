#ifndef FLUTTER_PLUGIN_PASSKEY_APPLE_PLUGIN_H_
#define FLUTTER_PLUGIN_PASSKEY_APPLE_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>
#include <string>
#include <windows.h>

namespace passkey_apple {

class PasskeyApplePlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  PasskeyApplePlugin();

  virtual ~PasskeyApplePlugin();

  // Disallow copy and assign.
  PasskeyApplePlugin(const PasskeyApplePlugin&) = delete;
  PasskeyApplePlugin& operator=(const PasskeyApplePlugin&) = delete;

 private:
  void HandleMethodCall(const flutter::MethodCall<flutter::EncodableValue> &method_call,
                        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

  void SignInWithApple(std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  static HWND mainWindowHandle;
};

}  // namespace passkey_apple

#endif  // FLUTTER_PLUGIN_PASSKEY_APPLE_PLUGIN_H_