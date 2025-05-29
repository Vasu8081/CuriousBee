import Foundation

struct Users: Codable {
    var id: UUID?
    var name: String?
    var email: String?
    var apple_device_token: String?
    var group_id: UUID?
    var is_admin: Bool?
    var tasks: [Tasks] = []
    var calendar_entries: [CalendarEntries] = []
}
