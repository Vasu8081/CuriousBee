import SwiftUI

struct PeriodTabView: View {
    @ObservedObject private var viewModel = UserViewModel.shared
    @StateObject private var periodVM = PeriodTrackerViewModel()
    @StateObject private var notificationVM = NotificationTrackerViewModel()

    @State private var showNotifications = false

    var body: some View {
        NavigationView {
            ScrollView {
                PeriodSummaryView(viewModel: periodVM)
                    .padding()
            }
        }
        .onAppear {
            periodVM.tracker = viewModel.userData.periodTracker
        }
        .onChange(of: periodVM.tracker) {
            viewModel.userData.periodTracker = periodVM.tracker
        }
    }
}
