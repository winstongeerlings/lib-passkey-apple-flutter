
import 'passkey_apple_platform_interface.dart';

class PasskeyApple {
  Future<String?> getPlatformVersion() {
    return PasskeyApplePlatform.instance.getPlatformVersion();
  }
}
