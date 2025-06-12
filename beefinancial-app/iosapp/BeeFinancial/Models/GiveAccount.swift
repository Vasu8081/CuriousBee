import Foundation

struct GiveAccount: Account {
    let id: UUID
    var name: String
    var balance: Double
    var remarks: String?

    var type: AccountType { .give }

    var personName: String
    var contactNumber: String
    var address: String
    var interestRate: Double
    var dueDate: Date

    var transactions: [Transaction]

    func summary() -> String {
        let formattedBalance = String(format: "₹%.2f", balance)
        let formattedRate = String(format: "%.2f", interestRate)
        let formattedDue = dueDate.formatted(date: .abbreviated, time: .omitted)
        return "Lent to \(personName) • \(formattedBalance) @\(formattedRate)% • Due \(formattedDue)"
    }
}
