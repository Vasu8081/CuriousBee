import SwiftUI

struct PeriodTabView: View {
    @StateObject private var notificationVM = NotificationTrackerViewModel()
    @State private var showNotifications = false

    var body: some View {
        NavigationView {
            ScrollView {
                PeriodSummaryView()
                    .padding()
            }
        }
    }
}
