import SwiftUI

struct HomeView: View {
    @StateObject private var auth = AuthenticateViewModel.shared
    @StateObject private var notifications = NotificationTrackerViewModel()
    @StateObject private var userViewModel = UserViewModel.shared

    @State private var selectedTab = 0
    @State private var showNotifications = false
    @State private var showLoginForm = false
//    @State private var showURLPrompt = true
    @State private var enteredURL = ""
    @State private var isSyncing = false
    @State private var rotation = 0.0

    var body: some View {
        NavigationStack {
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
            .onAppear {
                NotificationCenter.default.addObserver(
                    forName: .notificationTapped,
                    object: nil,
                    queue: .main
                ) { notification in
                    if let message = notification.object as? String {
                        print(message)
                        if message == "Perform Sync" {
                            UserViewModel.shared.syncFromServer()
                        }
                        else{
                            notifications.receive(message)
                            showNotifications = true
                        }
                    }
                }
            }
        }
        .navigationTitle("🐝 Tracker")
        .navigationBarTitleDisplayMode(.inline)
        .toolbar {
            ToolbarItemGroup(placement: .navigationBarTrailing) {
                notificationButton
                authOrSyncButton
            }
        }
        .sheet(isPresented: $showNotifications) {
            NotificationListView(viewModel: notifications)
        }
        .sheet(isPresented: $showLoginForm) {
            LoginViewForm()
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
                if userViewModel.needSync {
                    Button {
                        performSync()
                    } label: {
                        Image(systemName: "arrow.triangle.2.circlepath")
                            .rotationEffect(.degrees(rotation))
                            .foregroundColor(.blue)
                    }
                    .disabled(isSyncing)
                }
                else {
                    Button {
                        auth.signOut()
                    } label: {
                        Image(systemName: "rectangle.portrait.and.arrow.right")
                            .foregroundColor(.red)
                    }
                    
                }
            }
        }
    }

    private func performSync() {
        guard !isSyncing else { return }

        isSyncing = true
        withAnimation(Animation.linear(duration: 1).repeatForever(autoreverses: false)) {
            rotation += 360
        }

        userViewModel.pushToServer {
            DispatchQueue.main.async {
                isSyncing = false
                rotation = 0
            }
        }
    }
}
