import Foundation

struct PropertyAccount: Account {
    let id: UUID
    var name: String
    var balance: Double
    var remarks: String?

    var type: AccountType { .property }

    var propertyType: String
    var location: String
    var purchaseDate: Date

    var transactions: [Transaction]

    func summary() -> String {
        let formattedBalance = String(format: "₹%.2f", balance)
        return "\(propertyType.capitalized) at \(location) • \(formattedBalance) • Purchased on \(purchaseDate.formatted(date: .abbreviated, time: .omitted))"
    }
}
