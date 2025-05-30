import Foundation
import SwiftUI

class CalendarViewModel: ObservableObject {
    @Published var calendarEntriesViewModels: [CalendarEntriesViewModel] = []
    @Published var users: [Users] = []

    init() {
    }

    func reload() {
        ServerEndPoints.shared.getAllUsers { result in
            switch result {
            case .success(let users):
                DispatchQueue.main.async { self.users = users }
            case .failure(let error):
                print("Failed to fetch users: \(error)")
            }
        }

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

    func getUserName(for id: UUID?) -> String {
        users.first(where: { $0._id == id })?._name ?? "Unknown"
    }

    func getUserIds() -> [UUID] {
        users.prefix(2).compactMap { $0._id }
    }
    
    func deleteEntry(_ entry: CalendarEntriesViewModel) {
        self.calendarEntriesViewModels.removeAll { $0._id == entry._id }
    }
    
    func addEntry(_ entry: CalendarEntries) {
        self.calendarEntriesViewModels.append(CalendarEntriesViewModel(model: entry))
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
    
//    func getEntries(for userId: UUID?, on date: Date) -> [CalendarEntriesViewModel] {
//        calendarEntriesViewModels
//            .filter { $0._user_id == userId && Calendar.current.isDate($0._date ?? Date(), inSameDayAs: date) }
//            .sorted { ($0._start_time ?? .distantPast) < ($1._start_time ?? .distantPast) }
//    }

    func minutesSinceMidnight(_ date: Date?) -> Int {
        let components = Calendar.current.dateComponents([.hour, .minute], from: date ?? Date())
        return (components.hour ?? 0) * 60 + (components.minute ?? 0)
    }
}
