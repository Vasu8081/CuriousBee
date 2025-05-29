import Foundation

enum TaskInteractionStyle: String, Codable, CaseIterable {
    case soloNoDisturbance     // 👤 e.g. office work
    case soloWithCall          // ☎️ e.g. laundry
    case canDoAloneOrTogether  // 👥 e.g. cooking
    case bothConscious         // ❤️ e.g. deep conversation
    case oneWithSupport        // ✨ e.g. I lead, partner supports
    case letPartnerDecide      // ❓ open-ended

    var emoji: String {
        switch self {
        case .soloNoDisturbance: return "👤"
        case .soloWithCall: return "☎️"
        case .canDoAloneOrTogether: return "👥"
        case .bothConscious: return "❤️"
        case .oneWithSupport: return "✨"
        case .letPartnerDecide: return "❓"
        }
    }

    var description: String {
        switch self {
        case .soloNoDisturbance: return "Alone without disturbance"
        case .soloWithCall: return "Alone but on call"
        case .canDoAloneOrTogether: return "Can do alone or both"
        case .bothConscious: return "Both with conscious presence"
        case .oneWithSupport: return "One leads, other supports"
        case .letPartnerDecide: return "Let partner decide"
        }
    }
}

enum TaskPresencePreference: String, Codable, CaseIterable {
    case me
    case partner
    case both
}

enum TaskDoer: String, Codable, CaseIterable {
    case me
    case partner
}

struct Task: Identifiable, Codable {
    let id: UUID
    var title: String
    var notes: String?
    var deadline: Date?
    var interactionStyle: TaskInteractionStyle
    var presencePreference: TaskPresencePreference
    var assignedTo: TaskPresencePreference
    var primaryDoer: TaskDoer?
    var isCompleted: Bool
    var createdDate: Date
    var scheduledCalendarEntryId: UUID?

    init(
        id: UUID = UUID(),
        title: String,
        notes: String? = nil,
        deadline: Date? = nil,
        interactionStyle: TaskInteractionStyle,
        presencePreference: TaskPresencePreference,
        assignedTo: TaskPresencePreference,
        primaryDoer: TaskDoer? = nil,
        isCompleted: Bool = false,
        createdDate: Date = Date(),
        scheduledCalendarEntryId: UUID? = nil
    ) {
        self.id = id
        self.title = title
        self.notes = notes
        self.deadline = deadline
        self.interactionStyle = interactionStyle
        self.presencePreference = presencePreference
        self.assignedTo = assignedTo
        self.primaryDoer = primaryDoer
        self.isCompleted = isCompleted
        self.createdDate = createdDate
        self.scheduledCalendarEntryId = scheduledCalendarEntryId
    }
}
