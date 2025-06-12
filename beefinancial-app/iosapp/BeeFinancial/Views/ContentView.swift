import SwiftUI

struct ContentView: View {
    @StateObject private var userViewModel = UserViewModel()

    var body: some View {
        NavigationView {
            List {
                ForEach(userViewModel.userData.accounts) { account in
                    VStack(alignment: .leading) {
                        Text(account.name)
                            .font(.headline)
                        Text(account.summary())
                            .font(.subheadline)
                    }
                }
            }
            .navigationTitle("Accounts")
        }
    }
}
