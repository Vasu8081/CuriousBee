import Foundation

class UserViewModel: ObservableObject {
    @Published var accountViewModels: [UUID: AccountsViewModel] = [:]
    var accounts: [UUID: any Account] = [:]

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

                        if let account = AccountFactory.create(from: decrypted, type: accountType) {
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
    }
    

    func createAccount(account: any Account, type: AccountTypes) {
        // 1. Encode and encrypt the account
        guard let encoded = try? JSONEncoder().encode(account),
              let encrypted = encoded.encrypt() else {
            print("❌ Failed to encode/encrypt account")
            return
        }

        // 2. Create base model
        let newID = UUID()
        let model = Accounts(
            id: newID,
            user_id: UUID(uuidString: AuthenticateViewModel.shared.getGroupId()),
            type: type,
            account_data: encrypted
        )

        // 3. Create and save ViewModel
        let viewModel = AccountsViewModel(model: model)
        self.accountViewModels[newID] = viewModel
        self.accounts[newID] = account

        viewModel.save()
    }
    
    func saveAccount(id: UUID) {
        guard let viewModel = accountViewModels[id],
              let type = viewModel.type,
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
