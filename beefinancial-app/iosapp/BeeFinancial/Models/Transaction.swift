import Foundation

enum TransactionFlow: String, Codable {
    case income
    case expense
    case transfer
}

struct Transaction: Codable, Identifiable {
    let id: UUID
    var date: Date
    var amount: Double
    var note: String?
    var category: Category

    /// One of these will be nil depending on flow
    var fromAccountID: UUID?
    var toAccountID: UUID?

    var flow: TransactionFlow {
        switch (fromAccountID, toAccountID) {
        case (nil, .some): return .income
        case (.some, nil): return .expense
        case (.some, .some): return .transfer
        default: return .expense  // fallback
        }
    }
}
