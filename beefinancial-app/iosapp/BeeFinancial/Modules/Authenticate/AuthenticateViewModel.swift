import Foundation
import SwiftUI

class AuthenticateViewModel: ObservableObject {
    static let shared = AuthenticateViewModel()

    @Published var isSignedIn: Bool = false
    @Published var email: String? = nil
    @Published var errorMessage: String? = nil

    private let storageKey = "auth_user"
    private var refreshTimer: Timer?

    private var currentUser: AuthUser? {
        get {
            guard let data = UserDefaults.standard.data(forKey: storageKey),
                  let user = try? JSONDecoder().decode(AuthUser.self, from: data) else {
                print("⚠️ No valid stored user found in UserDefaults.")
                return nil
            }
            print("🔐 Loaded user from storage: \(user.userEmail)")
            return user
        }
        set {
            if let newUser = newValue {
                if let data = try? JSONEncoder().encode(newUser) {
                    UserDefaults.standard.set(data, forKey: storageKey)
                    print("✅ Stored user credentials for: \(newUser.userEmail)")
                }
            } else {
                UserDefaults.standard.removeObject(forKey: storageKey)
                print("🧹 Cleared stored user.")
            }
        }
    }

    private init() {
        print("🟡 AuthenticateViewModel initialized")
        if let user = currentUser, user.refreshTokenExpirationDate > Date() {
            email = user.userEmail
            print("✅ Auto-login as \(user.userEmail)")
            startRefreshTimer()
        } else {
            print("❌ Auto-login failed or refresh token expired")
            isSignedIn = false
        }
    }

    func signIn(email: String, password: String) {
        print("🔐 Attempting sign-in for \(email)...")
        ServerEndPoints.shared.signIn(email: email, password: password) { result in
            DispatchQueue.main.async {
                switch result {
                case .success(let user):
                    self.currentUser = user
                    self.email = user.userEmail
                    self.isSignedIn = true
                    self.startRefreshTimer()
                    print("✅ Sign-in successful for \(user.userEmail)")
                case .failure(let error):
                    self.errorMessage = error.localizedDescription
                    self.isSignedIn = false
                    print("❌ Sign-in failed: \(error.localizedDescription)")
                }
            }
        }
    }

    func refreshTokenIfNeeded(completion: @escaping (Bool) -> Void) {
        guard let user = currentUser else {
            print("❌ No user found. Cannot refresh.")
            completion(false)
            return
        }

        ServerEndPoints.shared.refreshToken(currentUser: user) { result in
            switch result {
            case .success(let updatedUser):
                DispatchQueue.main.async {
                    self.currentUser = updatedUser
                    self.email = updatedUser.userEmail
                    self.isSignedIn = true
                    print("✅ Token refresh succeeded for \(updatedUser.userEmail)")
                    completion(true)
                }
            case .failure:
                DispatchQueue.main.async {
                    self.signOut()
                    print("❌ Token refresh failed. Signed out.")
                    completion(false)
                }
            }
        }
    }

    func getAccessToken(completion: @escaping (String?) -> Void) {
        guard let user = currentUser else {
            print("❌ getAccessToken: No user session")
            completion(nil)
            return
        }

        if user.accessTokenExpirationDate > Date() {
            print("🟢 getAccessToken: Using valid token for \(user.userEmail)")
            completion(user.accessToken)
        } else if user.refreshTokenExpirationDate > Date() {
            print("🔄 getAccessToken: Token expired. Trying refresh.")
            ServerEndPoints.shared.refreshToken(currentUser: user) { result in
                switch result {
                case .success(let updatedUser):
                    DispatchQueue.main.async {
                        self.currentUser = updatedUser
                        self.email = updatedUser.userEmail
                        self.isSignedIn = true
                        print("✅ getAccessToken: Refresh successful")
                        completion(updatedUser.accessToken)
                    }
                case .failure:
                    DispatchQueue.main.async {
                        self.signOut()
                        print("❌ getAccessToken: Refresh failed. Signed out.")
                        completion(nil)
                    }
                }
            }
        } else {
            print("❌ getAccessToken: Both tokens expired. Signing out.")
            signOut()
            completion(nil)
        }
    }

    func getToken() -> String? {
        guard let token = currentUser?.accessToken else {
            print("❌ getToken(): No access token available.")
            return nil
        }
        print("🔑 getToken(): Returning access token.")
        return token
    }
    
    func getGroupId() -> String {
        return currentUser?.groupId ?? ""
    }

    func signOut() {
        print("🚪 Signing out user: \(email ?? "unknown")")
        currentUser = nil
        email = nil
        isSignedIn = false
        stopRefreshTimer()
    }

    private func startRefreshTimer() {
        refreshTimer?.invalidate()
        self.refreshTokenIfNeeded { success in
            if success {
                print("✅ Initial token refresh succeeded before starting timer.")
            } else {
                print("⚠️ Initial token refresh failed before starting timer.")
            }
            // ⏱️ Then start the repeating refresh timer
            self.refreshTimer = Timer.scheduledTimer(withTimeInterval: 300, repeats: true) { _ in
                self.refreshTokenIfNeeded { success in
                    if success {
                        print("🔁 Periodic token refresh succeeded.")
                    } else {
                        print("⚠️ Periodic token refresh failed.")
                    }
                }
            }
            RunLoop.main.add(self.refreshTimer!, forMode: .common)
            print("⏱️ Started refresh timer (every 5 mins).")
        }
    }

    private func stopRefreshTimer() {
        refreshTimer?.invalidate()
        refreshTimer = nil
        print("🛑 Stopped refresh timer.")
    }
}
