import Foundation

struct PeriodEntries: Codable {
    var id: UUID?
    var period_info_id: UUID?
    var start_date: Date?
    var end_date: Date?
    var notes: String?
    var is_ended: Bool?
    var period_symptoms: [PeriodSymptoms] = []
}
