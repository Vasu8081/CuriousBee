import Foundation

extension PeriodEntriesViewModel {
    var totalDays: Int {
        guard let startDate = _start_date, let endDate = _end_date else { return 0 }
        return Calendar.current.dateComponents([.day], from: startDate, to: endDate).day.map { $0 + 1 } ?? 0
    }

    func currentDayIndex(asOf date: Date = Date()) -> Int {
        guard let startDate = _start_date else { return 0 }
        let days = Calendar.current.dateComponents([.day], from: startDate, to: date).day ?? 0
        return min(days + 1, totalDays)
    }

    func daysRemaining(asOf date: Date = Date()) -> Int {
        guard let startDate = _start_date, let endDate = _end_date else { return 0 }
        
        let totalDuration = Calendar.current.dateComponents([.day], from: startDate, to: endDate+1).day ?? 0
        let daysPassed = Calendar.current.dateComponents([.day], from: startDate, to: date).day ?? 0

        return max(0, totalDuration - daysPassed)
    }
}
