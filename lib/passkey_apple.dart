import 'dart:io';

import 'passkey_apple_config.dart';
import 'passkey_apple_platform_interface.dart';

class PasskeyApple {

  late PasskeyAppleConfig _config;
  bool _hasSetConfig = false;

  void setConfig(PasskeyAppleConfig config) {
    _config = config;
    _hasSetConfig = true;
  }

  Future<String?> getPlatformVersion() {
    return PasskeyApplePlatform.instance.getPlatformVersion();
  }

  Future<dynamic?> signIn() {
    if (Platform.isWindows) {
      // On windows we follow Apple oAuth flow so we need to pass the config
      if (!_hasSetConfig) {
        throw Exception('PasskeyAppleConfig must be set before calling signIn');
      }

      return PasskeyApplePlatform.instance.signIn(_config);
    } else if (Platform.isMacOS) {
      return PasskeyApplePlatform.instance.signIn(null);
    }

    throw UnsupportedError('Unsupported platform');
  }

  Future<dynamic?> isSignedIn(
      String userIdentifier,
  ) {
    return PasskeyApplePlatform.instance.isSignedIn(userIdentifier);
  }

  // TODO: add signOut method
}
