import UIKit
import UserNotifications

extension Notification.Name {
    static let notificationTapped = Notification.Name("notificationTapped")
}

class AppDelegate: UIResponder, UIApplicationDelegate, UNUserNotificationCenterDelegate {

    func application(_ application: UIApplication,
                     didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {

        // Set notification delegate
        UNUserNotificationCenter.current().delegate = self

        // Request permission
        UNUserNotificationCenter.current().requestAuthorization(options: [.alert, .sound, .badge]) { granted, error in
            if granted {
                DispatchQueue.main.async {
                    UIApplication.shared.registerForRemoteNotifications()
                }
            } else {
                print("Notification permission denied:", error?.localizedDescription ?? "Unknown error")
            }
        }

        return true
    }

    // Push registration successful
    func application(_ application: UIApplication,
                     didRegisterForRemoteNotificationsWithDeviceToken deviceToken: Data) {
        let token = deviceToken.map { String(format: "%02.2hhx", $0) }.joined()
        print("✅ Device Token:", token)
        UserDefaults.standard.set(token, forKey: "deviceToken")
    }

    // Push registration failed
    func application(_ application: UIApplication,
                     didFailToRegisterForRemoteNotificationsWithError error: Error) {
        print("❌ Failed to register for remote notifications:", error)
    }

    // 🔔 Handle notifications when app is in foreground
    func userNotificationCenter(_ center: UNUserNotificationCenter,
                                willPresent notification: UNNotification,
                                withCompletionHandler completionHandler: @escaping (UNNotificationPresentationOptions) -> Void) {
        
        let content = notification.request.content
        let message = content.body
        
        // Post message to in-app handler (e.g. bell icon)
        NotificationCenter.default.post(name: .notificationTapped, object: message)

        // Don't show system notification (banner/sound)
        completionHandler([]) // suppress banner, sound, etc.
    }

    // 🔕 Handle taps on notification (background or closed)
    func userNotificationCenter(_ center: UNUserNotificationCenter,
                                didReceive response: UNNotificationResponse,
                                withCompletionHandler completionHandler: @escaping () -> Void) {
        
        let content = response.notification.request.content
        let message = content.body

        NotificationCenter.default.post(name: .notificationTapped, object: message)
        completionHandler()
    }
}
