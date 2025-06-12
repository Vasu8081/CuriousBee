import Foundation

class Helper {
    static let shared = Helper()
    
    public func formattedDate(_ date: Date) -> String {
        let f = DateFormatter()
        f.dateStyle = .medium
        return f.string(from: date)
    }
    
    public func formatDateRange(_ start: Date, _ end: Date) -> String {
        let f = DateFormatter()
        f.dateFormat = "MMM d"
        return "\(f.string(from: start)) - \(f.string(from: end))"
    }
}
