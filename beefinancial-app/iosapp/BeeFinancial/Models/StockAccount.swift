import Foundation

struct StockAccount: Account {
    let id: UUID
    var name: String
    var balance: Double
    var remarks: String?

    var type: AccountType { .stock }

    var brokerage: String
    var dematNumber: String
    var panNumber: String
    var openingDate: Date

    var transactions: [Transaction]

    func summary() -> String {
        let formattedBalance = String(format: "₹%.2f", balance)
        return "Stock - \(brokerage) • \(formattedBalance) • PAN \(panNumber)"
    }
}
