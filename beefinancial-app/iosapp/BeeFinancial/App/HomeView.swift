import SwiftUI

struct HomeView: View {
    @EnvironmentObject var userViewModel: UserViewModel
    @StateObject private var auth = AuthenticateViewModel.shared
    @State private var selectedTab = 0

    var body: some View {
        TabView(selection: $selectedTab) {
            AccountListView()
                .tabItem {
                    Label("Accounts", systemImage: "banknote.fill")
                }
                .tag(0)
        }
        .onChange(of: auth.isSignedIn) { oldValue, newValue in
            if newValue {
                reload()
            }
        }
    }
    
    private func reload() {
        userViewModel.reload()
    }
}
