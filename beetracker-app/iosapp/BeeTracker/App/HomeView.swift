import SwiftUI

struct HomeView: View {
    @StateObject private var auth = AuthenticateViewModel.shared
    @StateObject private var server = ServerEndPoints.shared
    @StateObject private var notifications = NotificationTrackerViewModel()
    
    @EnvironmentObject var userViewModel: UserViewModel
    @EnvironmentObject var calendarViewModel: CalendarViewModel
    @EnvironmentObject var periodViewModel: PeriodViewModel
    @EnvironmentObject var taskViewModel: TaskViewModel
    @EnvironmentObject var productViewModel: ProductViewModel

    @State private var selectedTab = 0

    var body: some View {
        TabView(selection: $selectedTab) {
            PeriodTabView()
                .tabItem {
                    Label("Periods", systemImage: "heart.fill")
                }
                .tag(0)

            ProductTabView()
                .tabItem {
                    Label("Products", systemImage: "leaf.fill")
                }
                .tag(1)

            CalendarTabView()
                .tabItem {
                    Label("Calendar", systemImage: "calendar")
                }
                .tag(2)

            TaskListView()
                .tabItem {
                    Label("Tasks", systemImage: "checkmark.circle")
                }
                .tag(3)
        }
        .onChange(of: auth.isSignedIn) { oldValue, newValue in
            if newValue {
                reload()
            }
        }
    }

    private func reload() {
        userViewModel.reload()
        periodViewModel.reload()
        productViewModel.reload()
        calendarViewModel.reload()
        taskViewModel.reload()
    }
}
