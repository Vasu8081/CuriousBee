import SwiftUI

struct StartView: View {
    @StateObject private var auth = AuthenticateViewModel.shared
    @StateObject private var server = ServerEndPoints.shared
    @StateObject private var notifications = NotificationTrackerViewModel()

    @State private var showNotifications = false
    @State private var showLoginForm = false
    @State private var enteredURL = ""
    @State private var isSyncing = false
    @State private var rotation = 0.0
    
    var appName: String

    var body: some View {
        NavigationStack {
            HomeView()
        }
        .onAppear {
            NotificationCenter.default.addObserver(
                forName: .notificationTapped,
                object: nil,
                queue: .main
            ) { notification in
                if let message = notification.object as? String {
                    print(message)
                }
            }
        }
        .navigationTitle(server.isDev ? "🐝 "+appName+"Dev" : "🐝 "+appName)
        .navigationBarTitleDisplayMode(.inline)
        .toolbar {
            ToolbarItemGroup(placement: .navigationBarTrailing) {
                notificationButton
                toggleModeButton
                authOrSyncButton
            }
        }
        .sheet(isPresented: $showNotifications) {
            NotificationListView(viewModel: notifications)
        }
        .sheet(isPresented: $showLoginForm) {
            LoginViewForm()
        }
        .onChange(of: auth.isSignedIn, {
            if auth.isSignedIn {
                ToastManager.shared.hidePersistentWarning()
                reload()
            }
            else {
                ToastManager.shared.show(
                    "Offline mode — changes won't be saved to the server.",
                    type: .warning,
                    persistent: true
                )
            }
        })
    }
    
    private var toggleModeButton: some View {
        Button {
            ServerEndPoints.shared.toggleMode()
        } label: {
            Image(systemName: "arrow.2.squarepath")
                .foregroundColor(.blue)
        }
    }

    private var notificationButton: some View {
        ZStack(alignment: .topTrailing) {
            Button {
                showNotifications = true
            } label: {
                Image(systemName: "bell")
            }

            if !notifications.unreadMessages.isEmpty {
                Circle()
                    .fill(Color.red)
                    .frame(width: 10, height: 10)
                    .offset(x: 6, y: -6)
            }
        }
    }

    private var authOrSyncButton: some View {
        Group {
            if auth.email == nil {
                Button {
                    showLoginForm = true
                } label: {
                    Image(systemName: "lock.fill")
                        .foregroundColor(.gray)
                }
            } else{
                Button {
                    auth.signOut()
                } label: {
                    Image(systemName: "rectangle.portrait.and.arrow.right")
                        .foregroundColor(.red)
                }
            }
        }
    }
    
    private func reload() {
        let token = UserDefaults.standard.string(forKey: "deviceToken") ?? "1234"
        var user = Users()
        user.apple_device_token = token
        ServerEndPoints.shared.sendDeviceToken(user, completion:{ response in
            print(response)
        })
    }
}
