import Foundation

struct Groups: Codable {
    var id: UUID?
    var name: String?
    var users: [Users] = []
    var tasks: [Tasks] = []
    var period_info: PeriodInfos?
    var product_infos: [ProductInfos] = []
}
