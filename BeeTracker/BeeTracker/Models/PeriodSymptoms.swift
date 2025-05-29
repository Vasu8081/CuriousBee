import Foundation

struct PeriodSymptoms: Codable {
    var id: UUID?
    var period_entry_id: UUID?
    var symptom_id: UUID?
    var _symptom_id: Symptoms?
}
