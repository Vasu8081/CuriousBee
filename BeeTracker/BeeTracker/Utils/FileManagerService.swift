import Foundation

class FileManagerService {
    static let shared = FileManagerService()
    private let fileName = "UserData.json"

    private var fileURL: URL? {
        FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first?.appendingPathComponent(fileName)
    }

    func save(_ userData: User) {
        guard let url = fileURL else { return }
        do {
            let data = try JSONEncoder().encode(userData)
            try data.write(to: url)
        } catch {
            print("Failed to save UserData: \(error)")
        }
    }

    func load() -> User? {
        guard let url = fileURL else { return nil }
        do {
            let data = try Data(contentsOf: url)
            return try JSONDecoder().decode(User.self, from: data)
        } catch {
            print("Failed to load UserData: \(error)")
            return nil
        }
    }
}
