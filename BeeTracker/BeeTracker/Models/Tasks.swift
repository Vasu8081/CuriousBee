import Foundation

struct Tasks: Codable {
    var id: UUID?
    var title: String?
    var notes: String?
    var deadline: Date?
    var interaction_style: String?
    var primary_doer_user_id: UUID?
    var other_users_presence_necessary: Bool?
    var is_completed: Bool?
    var group_id: UUID?
    var calendar_entrie: CalendarEntries?
}
