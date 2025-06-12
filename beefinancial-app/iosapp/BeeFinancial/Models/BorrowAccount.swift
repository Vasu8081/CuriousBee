import Foundation

struct BorrowAccount: Account {
    let id: UUID
    var name: String
    var balance: Double
    var remarks: String?

    var type: AccountType { .borrow }

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
        return "Borrowed from \(personName) • \(formattedBalance) @\(formattedRate)% • Due \(formattedDue)"
    }
}
