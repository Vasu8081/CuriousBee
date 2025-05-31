import Foundation
import SwiftUI

class CalendarViewModel: ObservableObject {
    @Published var calendarEntriesViewModels: [CalendarEntriesViewModel] = []

    init() {
    }

    func reload() {
        ServerEndPoints.shared.getAllCalendarEntries { result in
            switch result {
            case .success(let entries):
                DispatchQueue.main.async {
                    for entry in entries {
                        self.calendarEntriesViewModels.append(CalendarEntriesViewModel(model: entry))
                    }
                }
            case .failure(let error):
                print("Failed to fetch calendar entries: \(error)")
            }
        }
    }
    
    func refreshGroupings(triggerFor ids: [UUID?]) {
        self.calendarEntriesViewModels = self.calendarEntriesViewModels.map { $0 }
    }

    func deleteEntry(_ entry: CalendarEntriesViewModel) {
        guard let id = entry._id else {
            print("Cannot delete entry: id is nil")
            return
        }
        ServerEndPoints.shared.deleteCalendarEntries(id: id) { result in
            switch result {
            case .success:
                self.calendarEntriesViewModels.removeAll { $0._id == entry._id }
            case .failure(let error):
                print("Failed to delete calendar entry: \(error)")
            }
        }
    }
    
    func addEntry(_ entry: CalendarEntries) {
        let model = CalendarEntriesViewModel(model: entry)
        model.save()
        self.calendarEntriesViewModels.append(model)
    }
    
    func addCalendarEntry(_ entry: CalendarEntriesViewModel?) {
        self.calendarEntriesViewModels.append(entry ?? CalendarEntriesViewModel(model: CalendarEntries()))
    }

    func getEntries(for userId: UUID?, on date: Date) -> [CalendarEntriesViewModel] {
        calendarEntriesViewModels.filter {
            let isPersonal = $0._user_id == userId
            let isShared = $0._group_id != nil
            let isSameDate = Calendar.current.isDate($0._date ?? Date(), inSameDayAs: date)
            return isSameDate && (isPersonal || isShared)
        }
        .sorted { ($0._start_time ?? .distantPast) < ($1._start_time ?? .distantPast) }
    }
    
    func getSharedEntries(on date: Date) -> [CalendarEntriesViewModel] {
        calendarEntriesViewModels.filter {
            $0._group_id != nil && Calendar.current.isDate($0._date ?? Date(), inSameDayAs: date)
        }
    }

    func minutesSinceMidnight(_ date: Date?) -> Int {
        let components = Calendar.current.dateComponents([.hour, .minute], from: date ?? Date())
        return (components.hour ?? 0) * 60 + (components.minute ?? 0)
    }
}
