import SwiftUI

struct AccountListView: View {
    @ObservedObject var userViewModel: UserViewModel
    @State private var searchText = ""
    @State private var selectedFilter: AccountType? = nil
    @State private var showingAddAccountForm = false
    @State private var selectedAccount: AnyAccount? = nil

    var filteredAccounts: [AnyAccount] {
        userViewModel.userData.accounts.filter { account in
            (selectedFilter == nil || account.type == selectedFilter!) &&
            (searchText.isEmpty || account.name.lowercased().contains(searchText.lowercased()))
        }
    }

    var body: some View {
        NavigationStack {
            ZStack(alignment: .bottomTrailing) {
                VStack {
                    // Filter Bar
                    ScrollView(.horizontal, showsIndicators: false) {
                        HStack {
                            Button(action: { selectedFilter = nil }) {
                                Text("All")
                                    .padding(.horizontal, 12)
                                    .padding(.vertical, 6)
                                    .background(selectedFilter == nil ? Color.blue.opacity(0.2) : Color.clear)
                                    .cornerRadius(10)
                            }

                            ForEach(AccountType.allCases, id: \.self) { type in
                                Button(action: { selectedFilter = type }) {
                                    Text(type.rawValue.capitalized)
                                        .padding(.horizontal, 12)
                                        .padding(.vertical, 6)
                                        .background(selectedFilter == type ? Color.blue.opacity(0.2) : Color.clear)
                                        .cornerRadius(10)
                                }
                            }
                        }
                        .padding(.horizontal)
                    }

                    // Search Bar
                    TextField("Search accounts", text: $searchText)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                        .padding([.horizontal, .top])

                    // Account List
                    List(filteredAccounts, id: \.id) { account in
                        Button {
                            selectedAccount = account
                        } label: {
                            VStack(alignment: .leading) {
                                Text(account.name)
                                    .font(.headline)
                                Text(String(format: "₹%.2f", account.balance))
                                    .font(.subheadline)
                            }
                        }
                    }
                }

                // Floating Add Button (hidden when viewing detail)
                if selectedAccount == nil {
                    Button(action: {
                        showingAddAccountForm = true
                    }) {
                        Image(systemName: "plus.circle.fill")
                            .resizable()
                            .frame(width: 56, height: 56)
                            .foregroundColor(.blue)
                            .padding()
                    }
                }
            }
            .navigationDestination(item: $selectedAccount) { acc in
                AccountDetailView(account: acc, userViewModel: userViewModel)
            }
            .navigationTitle("Accounts")
            .sheet(isPresented: $showingAddAccountForm) {
                AddAccountFormView { newAccount in
                    userViewModel.userData.accounts.append(newAccount)
                    userViewModel.needSync = true
                    showingAddAccountForm = false
                }
            }
        }
    }
}
