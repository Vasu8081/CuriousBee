import Foundation

struct ProductCategories: Codable {
    var id: UUID?
    var name: String?
    var product_infos: [ProductInfos] = []
}
