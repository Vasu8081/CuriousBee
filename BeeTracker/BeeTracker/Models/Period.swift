import Foundation

struct IdentifiableDate: Identifiable, Equatable {
    let id = UUID()
    let value: Date
}

struct PeriodTracker: Codable, Equatable {
    var averageCycleLength: Int = 28
    var averagePeriodDuration: Int = 5
    var history: [PeriodEntry] = []
    var activePeriodStartDate: Date? = nil

    var predictedNextPeriod: Date? = nil
    var predictedFertileWindowStart: Date? = nil
    var predictedFertileWindowEnd: Date? = nil

    var autoLearningEnabled: Bool = true
}


struct PeriodEntry: Codable, Identifiable, Equatable {
    var id = UUID()
    var startDate: Date
    var endDate: Date
    var notes: String
    var symptoms: [SymptomTag]
    var isEnded: Bool = false
    
    var totalDays: Int {
        (Calendar.current.dateComponents([.day], from: startDate, to: endDate).day ?? -1) + 1
    }
}

enum SymptomTag: String, Codable, CaseIterable {
    case bloating
    case cramps
    case headache
    case nausea
    case fatigue
    case acne
    case cravings
    case tenderBreasts
    case moodSwings

    var displayName: String {
        switch self {
        case .tenderBreasts: return "Tender Breasts"
        case .moodSwings: return "Mood Swings"
        default: return rawValue.capitalized
        }
    }
}
