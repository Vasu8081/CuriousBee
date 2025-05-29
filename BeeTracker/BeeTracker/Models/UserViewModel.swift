import Foundation

class UserViewModel: ObservableObject {
    static let shared = UserViewModel()
    
    @Published var needSync: Bool = false
    @Published var userData: User {
        didSet {
            save()
        }
    }
    @Published var calendarEntries: [CalendarEntry] = []

    func loadCalendarEntries() {
        self.calendarEntries = userData.calendarEntries
    }

    func addCalendarEntry(_ entry: CalendarEntry) {
        userData.calendarEntries.append(entry)
        calendarEntries = userData.calendarEntries
        save()
    }

    func updateCalendarEntry(_ updated: CalendarEntry) {
        if let index = userData.calendarEntries.firstIndex(where: { $0.id == updated.id }) {
            userData.calendarEntries[index] = updated
            calendarEntries = userData.calendarEntries
            save()
        }
    }

    func deleteCalendarEntry(id: UUID) {
        userData.calendarEntries.removeAll { $0.id == id }
        calendarEntries = userData.calendarEntries
        save()
    }
    
    private init() {
        self.userData = FileManagerService.shared.load() ?? User()
        syncFromServer()
    }

    func save() {
        needSync = true
        FileManagerService.shared.save(userData)
    }

    func syncFromServer() {
        ServerEndPoints.shared.getData { result in
            switch result {
            case .success(let json):
                do {
                    let data = try JSONSerialization.data(withJSONObject: json)
                    let decoded = try JSONDecoder().decode(User.self, from: data)
                    DispatchQueue.main.async {
                        self.userData = decoded
                        ProductViewModel.shared.scheduleLowStockNotifications()
                        self.loadCalendarEntries()
                        print("Synced from server Successfully")
                    }
                } catch {
                    print("❌ Decoding error:", error.localizedDescription)
                }

            case .failure(let error):
                print("❌ Failed to fetch from server:", error.localizedDescription)
            }
        }
    }

    func pushToServer(completion: (() -> Void)? = nil) {
        do {
            if needSync {
                let dict = try userData.toDictionary()
                ServerEndPoints.shared.postData(jsonData: dict) { result in
                    DispatchQueue.main.async {
                        switch result {
                        case .success(let msg):
                            self.needSync = false
                            print("✅ Push success:", msg)
                        case .failure(let error):
                            print("❌ Push failed:", error.localizedDescription)
                        }
                        completion?()
                    }
                }
            } else {
                completion?()
            }
        } catch {
            print("❌ Failed to convert userData to dictionary:", error.localizedDescription)
            completion?()
        }
    }
}
