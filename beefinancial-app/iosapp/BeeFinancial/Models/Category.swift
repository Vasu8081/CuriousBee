import Foundation

enum CategoryType: String, Codable, CaseIterable {
    case income
    case expense
    case investment
    case transfer
}

struct Category: Codable, Identifiable, Hashable {
    var id: UUID
    var name: String
    var type: CategoryType
    var emoji: String?  // Optional icon or emoji

    static func defaultCategories() -> [Category] {
        [
            Category(id: UUID(), name: "Salary", type: .income, emoji: "💼"),
            Category(id: UUID(), name: "Groceries", type: .expense, emoji: "🛒"),
            Category(id: UUID(), name: "Utilities", type: .expense, emoji: "💡"),
            Category(id: UUID(), name: "Shopping", type: .expense, emoji: "🛍️"),
            Category(id: UUID(), name: "Food", type: .expense, emoji: "🍔"),
            Category(id: UUID(), name: "Investment", type: .investment, emoji: "📈"),
            Category(id: UUID(), name: "Transfer", type: .transfer, emoji: "🔁")
        ]
    }
}
