import Foundation

struct CreditCardAccount: Account {
    let id: UUID
    var name: String
    var balance: Double
    var remarks: String?

    var type: AccountType { .creditCard }

    var provider: String
    var cardNumber: String
    var limit: Double
    var dueDate: Date
    var billingCycleDay: Int

    var transactions: [Transaction]

    func summary() -> String {
        let formattedBalance = String(format: "₹%.2f", balance)
        let formattedLimit = String(format: "₹%.2f", limit)
        return "\(provider) Card (\(cardNumber)) • Balance \(formattedBalance) / Limit \(formattedLimit) • Due on \(dueDate.formatted(date: .abbreviated, time: .omitted))"
    }
}
