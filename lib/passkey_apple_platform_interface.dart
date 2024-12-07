import 'package:passkey_apple/passkey_apple_config.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'passkey_apple_method_channel.dart';

abstract class PasskeyApplePlatform extends PlatformInterface {
  /// Constructs a PasskeyApplePlatform.
  PasskeyApplePlatform() : super(token: _token);

  static final Object _token = Object();

  static PasskeyApplePlatform _instance = MethodChannelPasskeyApple();

  /// The default instance of [PasskeyApplePlatform] to use.
  ///
  /// Defaults to [MethodChannelPasskeyApple].
  static PasskeyApplePlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [PasskeyApplePlatform] when
  /// they register themselves.
  static set instance(PasskeyApplePlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }

  Future<dynamic?> signIn(PasskeyAppleConfig? config) {
    throw UnimplementedError('signIn() has not been implemented.');
  }

  Future<dynamic?> isSignedIn(
    String userIdentifier,
  ) {
    throw UnimplementedError('isSignedIn() has not been implemented.');
  }
}
