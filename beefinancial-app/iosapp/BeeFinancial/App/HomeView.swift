import SwiftUI

struct HomeView: View {
    @EnvironmentObject var userViewModel: UserViewModel
    @State private var selectedTab = 0

    var body: some View {
        NavigationStack {
            TabView(selection: $selectedTab) {
                TabView {
                    SummaryView(userViewModel: userViewModel)
                        .tabItem {
                            Label("Summary", systemImage: "chart.pie.fill")
                        }
                    
                    AccountListView(userViewModel: userViewModel)
                        .tabItem {
                            Label("Accounts", systemImage: "banknote.fill")
                        }
                    
                    CategoryTabView(userViewModel: userViewModel)
                        .tabItem {
                            Label("Categories", systemImage: "folder.fill")
                        }
                    
                    TransactionListView(userViewModel: userViewModel)
                        .tabItem {
                            Label("Transactions", systemImage: "list.bullet.rectangle.portrait")
                        }
                }
            }
        }
        .onAppear(){
            userViewModel.populateMockData()
        }
    }
}
