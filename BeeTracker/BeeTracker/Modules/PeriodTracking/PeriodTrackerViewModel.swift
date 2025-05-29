import Foundation
import SwiftUI

class PeriodTrackerViewModel: ObservableObject {
    @Published var tracker: PeriodTracker

    init(tracker: PeriodTracker = PeriodTracker()) {
        self.tracker = tracker
    }

    func addPeriod(start: Date, duration: Int, notes: String = "") {
        let end = Calendar.current.date(byAdding: .day, value: duration - 1, to: start) ?? start
        let newEntry = PeriodEntry(
            startDate: start,
            endDate: end,
            notes: notes,
            symptoms: []
        )
        tracker.history.append(newEntry)
        tracker.activePeriodStartDate = start
        updatePredictions()
    }

    func updatePeriod(_ entry: PeriodEntry) {
        if let index = tracker.history.firstIndex(where: { $0.id == entry.id }) {
            tracker.history[index] = entry
        }
    }

    func endActivePeriod() {
        tracker.activePeriodStartDate = nil
    }

    func nextExpectedDate() -> Date? {
        guard let last = tracker.history.last else { return nil }
        return Calendar.current.date(byAdding: .day, value: tracker.averageCycleLength, to: last.startDate)
    }

    func isDayInFutureOrToday(_ day: Date) -> Bool {
        Calendar.current.isDateInToday(day) || day > Date()
    }

    func updatePredictions() {
        if let last = tracker.history.last {
            tracker.predictedNextPeriod = nextExpectedDate()
            tracker.predictedFertileWindowStart = Calendar.current.date(byAdding: .day, value: tracker.averageCycleLength - 14, to: last.startDate)
            tracker.predictedFertileWindowEnd = Calendar.current.date(byAdding: .day, value: tracker.averageCycleLength - 10, to: last.startDate)
        }
    }
    
    func currentOngoingPeriod() -> PeriodEntry? {
        let today = Calendar.current.startOfDay(for: Date())
        
        return tracker.history.first(where: { entry in
            let end = Calendar.current.startOfDay(for: entry.endDate)
            
            if end < today {
                return false  // Already ended
            } else if end == today {
                return !entry.isEnded
            } else {
                return true
            }
        })
    }

    
    func endCurrentPeriod() {
        guard let ongoing = currentOngoingPeriod() else { return }

        if let index = tracker.history.firstIndex(where: { $0.id == ongoing.id }) {
            let today = Calendar.current.startOfDay(for: Date())
            tracker.history[index].endDate = today
            tracker.history[index].isEnded = true
            tracker.activePeriodStartDate = nil
        }
    }


    func daysRemaining(in entry: PeriodEntry) -> Int {
        let today = Calendar.current.startOfDay(for: Date())
        let end = Calendar.current.startOfDay(for: entry.endDate)
        return Calendar.current.dateComponents([.day], from: today, to: end).day ?? 0
    }

    func currentDayIndex(in entry: PeriodEntry) -> Int {
        let today = Calendar.current.startOfDay(for: Date())
        let start = Calendar.current.startOfDay(for: entry.startDate)
        return (Calendar.current.dateComponents([.day], from: start, to: today).day ?? 0) + 1
    }
    
    func deleteCurrentPeriod() {
        guard let ongoing = currentOngoingPeriod() else { return }

        if let index = tracker.history.firstIndex(where: { $0.id == ongoing.id }) {
            tracker.history.remove(at: index)
            tracker.activePeriodStartDate = nil
        }
    }
    
    func deletePeriod(_ entry: PeriodEntry) {
        if let index = tracker.history.firstIndex(where: { $0.id == entry.id }) {
            tracker.history.remove(at: index)
        }
    }

    
    func sendMood(_ emoji: String) {
        ServerEndPoints.shared.sendMessage(text: emoji) { result in
            DispatchQueue.main.async {
                switch result {
                case .success:
                    ToastManager.shared.show("Sent Successfully", type: .success)
                case .failure:
                    ToastManager.shared.show("Unable to send", type: .error)
                }
            }
        }
    }
}
