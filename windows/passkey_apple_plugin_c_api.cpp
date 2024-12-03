#include "include/passkey_apple/passkey_apple_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "passkey_apple_plugin.h"

void PasskeyApplePluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  passkey_apple::PasskeyApplePlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
