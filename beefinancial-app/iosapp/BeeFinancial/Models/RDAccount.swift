import Foundation

struct RDAccount: Account {
    let id: UUID
    var name: String
    var balance: Double
    var remarks: String?

    var type: AccountType { .rd }

    var monthlyAmount: Double
    var startDate: Date
    var maturityDate: Date
    var bankName: String
    var interestRate: Double

    var transactions: [Transaction]

    func summary() -> String {
        let bal = String(format: "₹%.2f", balance)
        let rate = String(format: "%.2f", interestRate)
        return "RD in \(bankName) • \(bal) • \(String(format: "₹%.2f", monthlyAmount))/mo @\(rate)% • Matures \(maturityDate.formatted(date: .abbreviated, time: .omitted))"
    }
}
