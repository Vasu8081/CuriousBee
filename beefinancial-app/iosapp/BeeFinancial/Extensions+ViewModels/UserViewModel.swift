import Foundation

class UserViewModel: ObservableObject {
    @Published var accountViewModels: [UUID: AccountsViewModel] = [:]
    @Published var transactionsViewModels: [UUID: TransactionsViewModel] = [:]
    var accounts: [UUID: any Account] = [:]
    var transactions: [UUID: TransactionDetails] = [:]

    init() {}

    func reload() {
        ServerEndPoints.shared.getAllAccounts { result in
            switch result {
            case .success(let entries):
                DispatchQueue.main.async {
                    for entry in entries {
                        guard let id = entry.id else { continue }

                        let viewModel = AccountsViewModel(model: entry)
                        self.accountViewModels[id] = viewModel
                        guard let accountType = entry.type,
                              let decrypted = viewModel.account_data else {
                            continue
                        }
                        
                        if let account = AccountFactory.create(from: decrypted, type: accountType, id: id) {
                            self.accounts[id] = account
                        } else {
                            print("❌ Failed to decode account for ID \(id)")
                        }
                    }
                }
            case .failure(let error):
                print("❌ Failed to fetch accounts entries: \(error)")
            }
        }
        
        ServerEndPoints.shared.getAllTransactions { result in
            switch result {
            case .success(let entries):
                DispatchQueue.main.async {
                    for entry in entries {
                        guard let id = entry.id else { continue }

                        let viewModel = TransactionsViewModel(model: entry)
                        self.transactionsViewModels[id] = viewModel
                        guard let decrypted = viewModel.transaction_details else {
                            continue
                        }

                        if let transaction_details = try? JSONDecoder().decode(TransactionDetails.self, from: decrypted) {
                            self.transactions[id] = transaction_details
                        } else {
                            print("❌ Failed to decode transactions for ID \(id)")
                        }
                    }
                }
            case .failure(let error):
                print("❌ Failed to fetch transaction entries: \(error)")
            }
        }
    }
    

    func createAccount(account: any Account, type: AccountTypes) {
        guard let encoded = try? JSONEncoder().encode(account),
              let encrypted = encoded.encrypt() else {
            print("❌ Failed to encode/encrypt account")
            return
        }
        let newID = UUID()
        let model = Accounts(
            id: newID,
            user_id: UUID(uuidString: AuthenticateViewModel.shared.getGroupId()),
            type: type,
            account_data: encrypted
        )
        let viewModel = AccountsViewModel(model: model)
        self.accountViewModels[newID] = viewModel
        self.accounts[newID] = account
        viewModel.save()
    }
    
    func createTransaction(from_account: any Account, to_account: any Account, transaction_details: TransactionDetails, category: CategoryTypes) {
        guard let encoded = try? JSONEncoder().encode(transaction_details),
              let encrypted = encoded.encrypt() else {
            print("❌ Failed to encode/encrypt account")
            return
        }
        let newID = UUID()
        let model = Transactions(
            id: newID,
            from_account_id: from_account.id,
            to_account_id: to_account.id,
            type: category, transaction_details: encrypted
        )
        let viewModel = TransactionsViewModel(model: model)
        self.transactionsViewModels[newID] = viewModel
        self.transactions[newID] = transaction_details
        viewModel.save()
    }
    
    func saveAccount(id: UUID) {
        guard let viewModel = accountViewModels[id],
              let account = accounts[id],
              let encoded = try? JSONEncoder().encode(account),
              let encrypted = encoded.encrypt() else {
            print("❌ Missing or invalid data for id: \(id)")
            return
        }
        viewModel.account_data = encrypted
        viewModel.save()
    }
}
