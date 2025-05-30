import Foundation

class CalendarViewForm : ObservableObject {
    @Published var calendarEntries: [CalendarEntries] = []
    @Published var users: [Users] = []
    
    init() {
    }
    
    func fetchGroupEntries() async {
        ServerEndPoints.shared.getAllCalendarEntries { result in
            switch(result) {
                case .success(let entries):
                self.calendarEntries = entries
            case .failure(let error):
                print("Error: \(error)")
            }
        }
    }
    
    func fetchUsers() async {
        ServerEndPoints.shared.getAllUsers { result in
            switch(result) {
                case .success(let entries):
                    self.users = entries
                case .failure(let error):
                    print("Error: \(error)")
            }
        }
    }
    
    func getCalendarEntriesByUser(_ userId: UUID) -> [CalendarEntries] {
        return calendarEntries.filter { $0._id == userId }
    }
    
    func getUsers() -> [String] {
        return users.compactMap { $0._name }
    }
    
}
