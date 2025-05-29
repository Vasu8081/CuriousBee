import SwiftUI

@main
struct BeeTrackerApp: App {
    @UIApplicationDelegateAdaptor(AppDelegate.self) var appDelegate
    @StateObject private var toast = ToastManager.shared
    
    var body: some Scene {
        WindowGroup {
            NavigationView {
                HomeView()
            }
            .overlay(alignment: .bottom) {
                if toast.isShowing {
                    Text("\(toast.type.prefix) \(toast.message ?? "")")
                        .font(.caption)
                        .padding(.horizontal, 16)
                        .padding(.vertical, 10)
                        .background(toast.type.backgroundColor)
                        .foregroundColor(.white)
                        .cornerRadius(8)
                        .padding(.bottom, 40)
                        .transition(.move(edge: .bottom).combined(with: .opacity))
                        .animation(.easeInOut(duration: 0.3), value: toast.isShowing)
                }
            }
        }
    }
}
