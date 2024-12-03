#ifndef FLUTTER_PLUGIN_PASSKEY_APPLE_PLUGIN_H_
#define FLUTTER_PLUGIN_PASSKEY_APPLE_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace passkey_apple {

class PasskeyApplePlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  PasskeyApplePlugin();

  virtual ~PasskeyApplePlugin();

  // Disallow copy and assign.
  PasskeyApplePlugin(const PasskeyApplePlugin&) = delete;
  PasskeyApplePlugin& operator=(const PasskeyApplePlugin&) = delete;

  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace passkey_apple

#endif  // FLUTTER_PLUGIN_PASSKEY_APPLE_PLUGIN_H_
