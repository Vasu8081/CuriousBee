//import Foundation
//
//class CalendarsViewModel: ObservableObject {
//    static let shared = CalendarsViewModel()
//
//    @Published private(set) var calendarEvents: [UUID: CalendarEntriesViewModel] = [:]
//
//    private init() {
//        fetchAll()
//    }
//
//    func fetchAll() {
//        ServerEndPoints.shared.getAllCalendarEntries { result in
//            switch result {
//            case .success(let entries):
//                DispatchQueue.main.async {
//                    var events: [UUID: CalendarEntriesViewModel] = [:]
//                    for entry in entries {
//                        if let id = entry.id {
//                            events[id] = CalendarEntriesViewModel(model: entry)
//                        }
//                    }
//                    self.calendarEvents = events
//                }
//            case .failure(let error):
//                print("Error fetching calendar entries: \(error)")
//            }
//        }
//    }
//
//    func addOrUpdate(_ model: CalendarEntries) {
//        ServerEndPoints.shared.addOrUpdateCalendarEntries(model) { result in
//            switch result {
//            case .success(let idString):
//                guard let uuid = UUID(uuidString: idString) else {
//                    print("Invalid ID returned from server")
//                    return
//                }
//                var updatedModel = model
//                updatedModel.id = uuid
//                DispatchQueue.main.async {
//                    self.calendarEvents[uuid] = CalendarEntriesViewModel(model: updatedModel)
//                }
//            case .failure(let error):
//                print("Error adding/updating calendar entry: \(error)")
//            }
//        }
//    }
//
//    func delete(id: UUID) {
//        ServerEndPoints.shared.deleteCalendarEntries(id: id) { result in
//            switch result {
//            case .success:
//                DispatchQueue.main.async {
//                    self.calendarEvents.removeValue(forKey: id)
//                }
//            case .failure(let error):
//                print("Error deleting calendar entry: \(error)")
//            }
//        }
//    }
//
//    func get(by id: UUID) {
//        ServerEndPoints.shared.getCalendarEntries(id: id) { result in
//            switch result {
//            case .success(let entry):
//                if let entryId = entry.id {
//                    DispatchQueue.main.async {
//                        self.calendarEvents[entryId] = CalendarEntriesViewModel(model: entry)
//                    }
//                }
//            case .failure(let error):
//                print("Error getting calendar entry: \(error)")
//            }
//        }
//    }
//
//    var allSorted: [CalendarEntriesViewModel] {
//        calendarEvents.values.sorted { ($0.date ?? .distantPast) < ($1.date ?? .distantPast) }
//    }
//}
