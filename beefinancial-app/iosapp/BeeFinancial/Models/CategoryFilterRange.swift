import Foundation

enum CategoryFilterRange: String, CaseIterable, Identifiable {
    case thisMonth = "This Month"
    case lastMonth = "Last Month"

    var id: String { rawValue }

    func matches(_ date: Date) -> Bool {
        let calendar = Calendar.current
        let now = Date()

        switch self {
        case .thisMonth:
            return calendar.isDate(date, equalTo: now, toGranularity: .month)
        case .lastMonth:
            guard let lastMonth = calendar.date(byAdding: .month, value: -1, to: now) else { return false }
            return calendar.component(.month, from: date) == calendar.component(.month, from: lastMonth) &&
                   calendar.component(.year, from: date) == calendar.component(.year, from: lastMonth)
        }
    }
}
