import Foundation

enum AccountType: String, Codable, CaseIterable {
    case bank
    case loan
    case chit
    case stock
    case fd
    case rd
    case creditCard
    case property
    case give
    case borrow

    var displayName: String {
        switch self {
        case .bank: return "Bank"
        case .loan: return "Loan"
        case .chit: return "Chit Fund"
        case .stock: return "Stock"
        case .fd: return "Fixed Deposit"
        case .rd: return "Recurring Deposit"
        case .creditCard: return "Credit Card"
        case .property: return "Property"
        case .give: return "Lend"
        case .borrow: return "Borrow"
        }
    }
}

protocol Account: Identifiable, Codable {
    var id: UUID { get }
    var name: String { get set }
    var balance: Double { get set }
    var remarks: String? { get set }
    var type: AccountType { get }

    func summary() -> String
}
