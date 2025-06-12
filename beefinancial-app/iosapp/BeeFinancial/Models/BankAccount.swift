import Foundation

struct BankAccount: Account {
    let id: UUID
    var name: String
    var balance: Double
    var remarks: String?

    var type: AccountType { .bank }

    var accountNumber: String
    var ifscCode: String
    var bankName: String

    var transactions: [Transaction]

    func summary() -> String {
        "Bank Account '\(name)' (\(bankName)) - \(String(format: "₹%.2f", balance))"
    }
}
