import SwiftUI

@main
struct BeeFinancial: App {
    @UIApplicationDelegateAdaptor(AppDelegate.self) var appDelegate
    @StateObject private var toast = ToastManager.shared
    @StateObject private var userViewModel = UserViewModel()
    
    init() {
        ServerEndPoints.shared.appName = "beefinancial"
    }
    
    var body: some Scene {
        WindowGroup {
            NavigationStack {
                StartView(appName: "Financial")
            }
            .environmentObject(userViewModel)
        }
    }
}
