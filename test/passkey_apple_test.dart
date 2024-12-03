import 'package:flutter_test/flutter_test.dart';
import 'package:passkey_apple/passkey_apple.dart';
import 'package:passkey_apple/passkey_apple_platform_interface.dart';
import 'package:passkey_apple/passkey_apple_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockPasskeyApplePlatform
    with MockPlatformInterfaceMixin
    implements PasskeyApplePlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final PasskeyApplePlatform initialPlatform = PasskeyApplePlatform.instance;

  test('$MethodChannelPasskeyApple is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelPasskeyApple>());
  });

  test('getPlatformVersion', () async {
    PasskeyApple passkeyApplePlugin = PasskeyApple();
    MockPasskeyApplePlatform fakePlatform = MockPasskeyApplePlatform();
    PasskeyApplePlatform.instance = fakePlatform;

    expect(await passkeyApplePlugin.getPlatformVersion(), '42');
  });
}
