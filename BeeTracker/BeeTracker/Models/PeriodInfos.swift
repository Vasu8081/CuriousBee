import Foundation

struct PeriodInfos: Codable {
    var group_id: UUID?
    var average_cycle_length: Int?
    var average_period_duration: Int?
    var active_period_start_date: Date?
    var predicted_next_period_date: Date?
    var period_entries: [PeriodEntries] = []
}
