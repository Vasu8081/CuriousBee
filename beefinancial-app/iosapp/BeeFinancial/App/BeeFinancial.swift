import SwiftUI

@main
struct BeeFinancial: App {
    @UIApplicationDelegateAdaptor(AppDelegate.self) var appDelegate
    @StateObject private var toast = ToastManager.shared
    @StateObject private var userViewModel = UserViewModel()
    
    var body: some Scene {
        WindowGroup {
            NavigationView {
                StartView(appName: "Financial")
            }
            .environmentObject(userViewModel)
        }
    }
}
