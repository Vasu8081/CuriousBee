import Foundation

enum AccountCategory: String, CaseIterable {
    case credit
    case debit
    case savings
    case investments

    var displayName: String {
        switch self {
        case .credit: return "Credit Accounts"
        case .debit: return "Debit / Lending"
        case .savings: return "Savings / Deposits"
        case .investments: return "Investments / Assets"
        }
    }

    static let map: [AccountType: AccountCategory] = [
        .bank: .savings,
        .fd: .savings,
        .rd: .savings,
        .loan: .credit,
        .creditCard: .credit,
        .give: .debit,
        .borrow: .credit,
        .chit: .savings,
        .stock: .investments,
        .property: .investments
    ]
}
