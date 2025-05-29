import Foundation

struct ProductInfos: Codable {
    var id: UUID?
    var group_id: UUID?
    var name: String?
    var category_id: UUID?
    var reminder_enabled: Bool?
    var days_per_quantity: Double?
    var quantity_unit_size: String?
    var products: [Products] = []
}
