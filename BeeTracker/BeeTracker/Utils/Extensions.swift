import Foundation

extension Calendar {
    func isDate(_ date: Date, inBetween start: Date, and end: Date) -> Bool {
        return date >= start && date <= end
    }
}

extension Encodable {
    func toDictionary() throws -> [String: Any] {
        let data = try JSONEncoder().encode(self)
        let json = try JSONSerialization.jsonObject(with: data, options: [])
        guard let dictionary = json as? [String: Any] else {
            throw NSError(domain: "toDictionary", code: 1, userInfo: [NSLocalizedDescriptionKey: "Invalid structure"])
        }
        return dictionary
    }
}
