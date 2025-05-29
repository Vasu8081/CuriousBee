import Foundation

struct ProductStatus: Codable {
    var id: UUID?
    var name: String?
    var products: [Products] = []
}
