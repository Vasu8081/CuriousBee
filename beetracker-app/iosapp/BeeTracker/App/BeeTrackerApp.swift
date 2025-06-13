import SwiftUI

@main
struct BeeTrackerApp: App {
    @UIApplicationDelegateAdaptor(AppDelegate.self) var appDelegate
    @StateObject private var toast = ToastManager.shared
    @StateObject private var userViewModel = UserViewModel()
    @StateObject private var productViewModel = ProductViewModel()
    @StateObject private var calendarViewModel = CalendarViewModel()
    @StateObject private var periodViewModel = PeriodViewModel()
    @StateObject private var taskViewModel = TaskViewModel()
    
    var body: some Scene {
        WindowGroup {
            NavigationView {
                StartView(appName: "Tracker")
            }
            .environmentObject(userViewModel)
            .environmentObject(calendarViewModel)
            .environmentObject(periodViewModel)
            .environmentObject(taskViewModel)
            .environmentObject(productViewModel)
            .overlay(alignment: .bottom) {
                if toast.isShowing || toast.isOfflineWarningActive {
                    Text("\(toast.type.prefix) \(toast.message ?? "")")
                        .font(.caption)
                        .padding(.horizontal, 16)
                        .padding(.vertical, 10)
                        .background(toast.type.backgroundColor)
                        .foregroundColor(.white)
                        .cornerRadius(8)
                        .padding(.bottom, 40)
                        .transition(.move(edge: .bottom).combined(with: .opacity))
                        .animation(.easeInOut(duration: 0.3), value: toast.isShowing || toast.isOfflineWarningActive)
                }
            }
        }
    }
}
