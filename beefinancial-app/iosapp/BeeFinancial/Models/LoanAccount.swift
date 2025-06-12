import Foundation

struct LoanAccount: Account {
    let id: UUID
    var name: String
    var balance: Double
    var remarks: String?

    var type: AccountType { .loan }

    var lenderName: String
    var interestRate: Double
    var emiAmount: Double
    var dueDate: Date

    var transactions: [Transaction]

    func summary() -> String {
        let formattedBalance = String(format: "₹%.2f", balance)
        let formattedRate = String(format: "%.2f", interestRate)
        let formattedEMI = String(format: "₹%.2f", emiAmount)
        let formattedDate = dueDate.formatted(date: .abbreviated, time: .omitted)
        return "Loan from \(lenderName) • \(formattedBalance) @\(formattedRate)% • EMI \(formattedEMI) due \(formattedDate)"
    }
}
