import Foundation

struct FDAccount: Account {
    let id: UUID
    var name: String
    var balance: Double
    var remarks: String?

    var type: AccountType { .fd }

    var bankName: String
    var interestRate: Double
    var startDate: Date
    var maturityDate: Date

    var transactions: [Transaction]

    func summary() -> String {
        let formattedBalance = String(format: "₹%.2f", balance)
        let formattedRate = String(format: "%.2f", interestRate)
        return "FD in \(bankName) • \(formattedBalance) @\(formattedRate)% • Matures \(maturityDate.formatted(date: .abbreviated, time: .omitted))"
    }
}
