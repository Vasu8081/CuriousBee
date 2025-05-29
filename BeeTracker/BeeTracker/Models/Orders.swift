import Foundation

struct Orders: Codable {
    var id: UUID?
    var ordered_date: Date?
    var expected_delivery_date: Date?
    var received_date: Date?
    var product_id: UUID?
}
