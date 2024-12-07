import AuthenticationServices

#if os(OSX)
import FlutterMacOS
#elseif os(iOS)
import Flutter
#endif

// TODO: make file pretty, it is one big mess now
public class PasskeyApplePlugin: NSObject, FlutterPlugin {
  var _lastSignInWithAppleAuthorizationController: SignInWithAppleAuthorizationController?


  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "passkey_apple", binaryMessenger: registrar.messenger)
    let instance = PasskeyApplePlugin()
    registrar.addMethodCallDelegate(instance, channel: channel)
  }

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
    switch call.method {
    case "getPlatformVersion":
      result("macOS " + ProcessInfo.processInfo.operatingSystemVersionString)
    case "signIn":
     let signInController = SignInWithAppleAuthorizationController(result)

     // store to keep alive
     _lastSignInWithAppleAuthorizationController = signInController

     signInController.signInWithApplePasskey()

     case "isSignedIn":
     // TODO: make error handling better:
     // SignInWithAppleGenericError.missingArguments(call).toFlutterError() - https://github.com/aboutyou/dart_packages/blob/3076e42a1a15751e11a7664ec95ea7e6645c79e9/packages/sign_in_with_apple/sign_in_with_apple/darwin/Classes/SignInWithAppleAvailablePlugin.swift
         guard let args = call.arguments as? [String: Any] else {
             result("no args")
             return
         }

         guard let userIdentifier = args["userIdentifier"] as? String else {
             result("no userIdentifier")
             return
         }

      let appleIDProvider = ASAuthorizationAppleIDProvider()

      appleIDProvider.getCredentialState(forUserID: userIdentifier) {
          credentialState, error in
          if let error = error {
                print(error.localizedDescription)
              result("false")
              return
          }

          switch credentialState {
          case .authorized:
              result("true")
          case .revoked:
              result("true")
          case .notFound:
              result("false")

          default:
              result("false")
          }
      }

    default:
      result(FlutterMethodNotImplemented)
    }
  }
}

@available(iOS 13.0, macOS 12.15, *)
class SignInWithAppleAuthorizationController: NSObject, ASAuthorizationControllerDelegate {
  var callback: FlutterResult

  init(_ callback: @escaping FlutterResult) {
      self.callback = callback
  }

  public func signInWithApplePasskey() {
      let appleIDProvider = ASAuthorizationAppleIDProvider()
      let request = appleIDProvider.createRequest()
      request.requestedScopes = [.fullName, .email]

        let authorizationController = ASAuthorizationController(authorizationRequests: [request])
        authorizationController.delegate = self
        authorizationController.performRequests()
    }

    public func authorizationController(
        controller _: ASAuthorizationController,
        didCompleteWithAuthorization authorization: ASAuthorization
    ) {
       if let credential = authorization.credential as? ASAuthorizationAppleIDCredential {
            callback(parseData(data: credential.identityToken))
       }
    }

    public func authorizationController(
        controller _: ASAuthorizationController,
        didCompleteWithError error: Error
    ){
      print("authorizationController: didCompleteWithError called with error: \(error.localizedDescription)")
      callback(FlutterError(code: "SIGN_IN_FAILED", message: error.localizedDescription, details: nil))
    }

    private func parseData(data: Data?) -> String? {
          return data != nil ? String(data: data!, encoding: .utf8) : nil
    }
}
