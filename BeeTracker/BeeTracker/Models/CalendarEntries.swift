import Foundation

struct CalendarEntries: Codable {
    var id: UUID?
    var user_id: UUID?
    var title: String?
    var notes: String?
    var date: Date?
    var start_time: Date?
    var end_time: Date?
    var task_id: UUID?
}
