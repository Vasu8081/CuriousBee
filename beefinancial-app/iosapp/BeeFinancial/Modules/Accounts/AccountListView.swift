import SwiftUI

struct AccountListView: View {
    @EnvironmentObject var viewModel: UserViewModel
    @State private var searchText = ""
    @State private var selectedFilter: AccountTypes? = nil

    var filteredAccounts: [(UUID, any Account)] {
        viewModel.accounts
            .filter { id, account in
                (searchText.isEmpty || account.name.lowercased().contains(searchText.lowercased())) &&
                (selectedFilter == nil || viewModel.accountViewModels[id]?.type == selectedFilter)
            }
            .sorted { $0.value.name < $1.value.name }
    }

    var body: some View {
        NavigationView {
            VStack {
                // Search + Filter
                HStack {
                    TextField("Search by name...", text: $searchText)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                    Menu {
                        Button("All", action: { selectedFilter = nil })
                        ForEach(AccountTypes.allCases, id: \.self) { type in
                            Button(type.rawValue.capitalized, action: { selectedFilter = type })
                        }
                    } label: {
                        Label("Filter", systemImage: "line.3.horizontal.decrease.circle")
                    }
                }
                .padding()

                // Account Cards
                ScrollView {
                    LazyVStack(spacing: 12) {
                        ForEach(filteredAccounts, id: \.0) { id, account in
                            AccountCardView(account: account, type: viewModel.accountViewModels[id]?.type)
                        }
                    }
                    .padding()
                }
            }
            .navigationTitle("Accounts")
        }
    }
}
