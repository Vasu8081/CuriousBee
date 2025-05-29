import Foundation

enum CalendarEntryType: String, Codable, CaseIterable {
    case task
    case event
    case period
    case mood
}

enum TaskAssignment: String, Codable, CaseIterable {
    case me
    case partner
    case both
}

struct CalendarEntry: Identifiable, Codable, Equatable {
    let id: UUID
    var title: String
    var notes: String?
    var date: Date
    var startTime: Date
    var endTime: Date?
    var entryType: CalendarEntryType
    var assignedTo: TaskAssignment
    var linkedTaskId: UUID?

    init(
        id: UUID = UUID(),
        title: String,
        notes: String? = nil,
        date: Date,
        startTime: Date,
        endTime: Date? = nil,
        entryType: CalendarEntryType,
        assignedTo: TaskAssignment,
        linkedTaskId: UUID? = nil
    ) {
        self.id = id
        self.title = title
        self.notes = notes
        self.date = date
        self.startTime = startTime
        self.endTime = endTime
        self.entryType = entryType
        self.assignedTo = assignedTo
        self.linkedTaskId = linkedTaskId
    }
}
