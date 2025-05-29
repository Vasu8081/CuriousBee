import Foundation

struct Products: Codable {
    var id: UUID?
    var product_info_id: UUID?
    var quantity: Double?
    var started_using_date: Date?
    var product_completed_date: Date?
    var price: Double?
    var status_id: UUID?
    var order: Orders?
}
