import Foundation
import UserNotifications

class NotificationTrackerViewModel: ObservableObject {
    @Published var receivedMessages: [String] = []
    @Published var unreadMessages: [String] = []

    init() {
        UNUserNotificationCenter.current().requestAuthorization(options: [.alert, .sound, .badge]) { _, _ in }
    }

    func receive(_ message: String) {
        guard !receivedMessages.contains(message) else { return }
        receivedMessages.append(message)
        unreadMessages.append(message)
        showLocalNotification(for: message)
    }
    
    func clearAll() {
        receivedMessages.removeAll()
        unreadMessages.removeAll()
    }

    private func showLocalNotification(for message: String) {
        let content = UNMutableNotificationContent()
        content.title = "New Message"
        content.body = message
        content.sound = .default

        let request = UNNotificationRequest(
            identifier: UUID().uuidString,
            content: content,
            trigger: nil
        )

        UNUserNotificationCenter.current().add(request)
    }
}
