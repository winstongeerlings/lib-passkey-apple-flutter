import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:passkey_apple/passkey_apple_config.dart';

import 'passkey_apple_platform_interface.dart';

/// An implementation of [PasskeyApplePlatform] that uses method channels.
class MethodChannelPasskeyApple extends PasskeyApplePlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('passkey_apple');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }

  @override
  Future<dynamic?> signIn(PasskeyAppleConfig? config) async {
    if (config == null) {
      final signIn = await methodChannel.invokeMethod<String>('signIn');
      return signIn;
    }

    final signIn = await methodChannel.invokeMethod<String>('signIn', <String, String>{
      'clientId': config.clientId,
      'redirectUri': config.redirectUri,
      'scheme': config.scheme,
    });
    return signIn;
  }

  @override
  Future<dynamic?> isSignedIn(
    String userIdentifier,
  ) async {
    final isSignedIn = await methodChannel.invokeMethod<String>('isSignedIn', <String, String>{'userIdentifier': userIdentifier});
    return isSignedIn;
  }
}
