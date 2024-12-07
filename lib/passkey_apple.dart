import 'passkey_apple_platform_interface.dart';

class PasskeyApple {
  Future<String?> getPlatformVersion() {
    return PasskeyApplePlatform.instance.getPlatformVersion();
  }

  Future<dynamic?> signIn() {
    return PasskeyApplePlatform.instance.signIn();
  }

  Future<dynamic?> isSignedIn(
      String userIdentifier,
  ) {
    return PasskeyApplePlatform.instance.isSignedIn(userIdentifier);
  }

  // TODO: add signOut method
}
