import Foundation

struct UserData: Codable {
    var accounts: [AnyAccount]
    var categories: [Category]

    static var empty: UserData {
        UserData(
            accounts: [],
            categories: Category.defaultCategories()
        )
    }

    var allTransactions: [Transaction] {
        accounts.flatMap { $0.transactions }
            .sorted { $0.date > $1.date }
    }
}
