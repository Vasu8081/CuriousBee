import SwiftUI
struct AccountIDWrapper: Identifiable, Equatable {
    var id: UUID
}

struct AccountListView: View {
    @EnvironmentObject var viewModel: UserViewModel
    @State private var searchText = ""
    @State private var selectedFilter: AccountTypes? = nil

    @State private var editingAccountID: AccountIDWrapper?

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
                // Search and filter
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
                .padding(.horizontal)

                // Account cards
                ScrollView {
                    LazyVStack(alignment: .leading, spacing: 16) {
                        ForEach(filteredAccounts, id: \.0) { id, account in
                            AccountCardView(
                                account: account,
                                type: viewModel.accountViewModels[id]?.type,
                                onEdit: {
                                    editingAccountID = AccountIDWrapper(id: id)
                                },
                                onDelete: {
                                    
                                }
                            )
                            .padding(.horizontal)
                        }
                    }
                    .padding(.vertical)
                }
            }
            .navigationTitle("Accounts")
            .sheet(item: $editingAccountID) { wrapper in
                let id = wrapper.id
                if let type = viewModel.accountViewModels[id]?.type,
                   let account = viewModel.accounts[id] {
                    AccountFormHelperView(type: type, model: account)
                        .environmentObject(viewModel)
                        .id(id)
                }
            }
        }
    }
}
