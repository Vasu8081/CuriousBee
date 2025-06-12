import Foundation

struct ChitAccount: Account {
    let id: UUID
    var name: String
    var balance: Double
    var remarks: String?

    var type: AccountType { .chit }

    var groupName: String
    var monthlyContribution: Double
    var startDate: Date
    var maturityDate: Date

    var transactions: [Transaction]

    func summary() -> String {
        let formattedBalance = String(format: "₹%.2f", balance)
        let contribution = String(format: "₹%.2f", monthlyContribution)
        return "Chit \(groupName) • \(formattedBalance) • \(contribution)/mo • Ends \(maturityDate.formatted(date: .abbreviated, time: .omitted))"
    }
}
