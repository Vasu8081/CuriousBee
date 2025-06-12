import Foundation

class UserViewModel: ObservableObject {
    @Published private var usersViewModel: [UsersViewModel] = []
    
    init() {
    }

    func reload() {
        ServerEndPoints.shared.getAllUsers { result in
            switch result {
            case .success(let users):
                DispatchQueue.main.async {
                    for user in users{
                        self.usersViewModel.append(UsersViewModel(model: user))
                    }
                }
            case .failure(let error):
                print("Failed to fetch users: \(error)")
            }
        }
    }
    
    func getUserName(for id: UUID?) -> String {
        self.usersViewModel.first(where: { $0.id == id })?.name ?? "Unknown"
    }

    func getUserIds() -> [UUID] {
        self.usersViewModel.prefix(2).compactMap { $0.id }
    }
    
    func getUserViewModel(for id: UUID?) -> UsersViewModel? {
        self.usersViewModel.first(where: { $0.id == id })
    }
    
    func getUserViewModels() -> [UsersViewModel] {
        return self.usersViewModel
    }
}
