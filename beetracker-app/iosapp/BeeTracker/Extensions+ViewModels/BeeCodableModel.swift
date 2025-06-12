import Foundation

// MARK: - Protocol your models conform to
protocol BeeCodableModel {
    static var serverTypeMap: [String: String] { get }
}

// MARK: - Optional detection helper
protocol AnyOptional {
    var isNil: Bool { get }
}

enum ServerDateFormatter {
    static let datetimeFormatter: DateFormatter = {
        let formatter = DateFormatter()
        formatter.calendar = Calendar(identifier: .iso8601)
        formatter.locale = Locale(identifier: "en_US_POSIX")
        formatter.timeZone = TimeZone(secondsFromGMT: 0)
        formatter.dateFormat = "yyyy-MM-dd'T'HH:mm:ss.SSSSSS"
        return formatter
    }()
    
    static let dateFormatter: DateFormatter = {
        let f = DateFormatter()
        f.dateFormat = "yyyy-MM-dd"
        f.locale = Locale(identifier: "en_US_POSIX")
        return f
    }()

    static let timeFormatter: DateFormatter = {
        let f = DateFormatter()
        f.dateFormat = "HH:mm:ss"
        f.locale = Locale(identifier: "en_US_POSIX")
        return f
    }()
}

extension Optional: AnyOptional {
    var isNil: Bool { self == nil }
}

// MARK: - Type-erased Encodable wrapper
struct AnyEncodable: Encodable {
    private let _encode: (Encoder) throws -> Void

    init<T: Encodable>(_ wrapped: T) {
        self.encode = wrapped.encode(to:)
    }

    func encode(to encoder: Encoder) throws {
        try _encode(encoder)
    }
}

func encodeNestedValue(_ value: Any) throws -> Any {
    if let optional = value as? AnyOptional, optional.isNil {
        return NSNull()
    }

    // Handle primitives directly
    if value is String || value is Int || value is Double || value is Bool || value is UUID {
        return value
    }

    if let date = value as? Date {
        return ServerDateFormatter.datetimeFormatter.string(from: date)
    }

    // Handle array of any kind
    if let array = value as? [Any] {
        return try array.map { try encodeNestedValue($0) }
    }

    // Handle nested BeeCodableModel structs
    if let nested = value as? (Encodable & BeeCodableModel) {
        let data = try encodeModel(nested)
        let json = try JSONSerialization.jsonObject(with: data, options: [])
        return json
    }

    // Fallback: try generic encoding
    if let encodable = value as? Encodable {
        let data = try JSONEncoder().encode(AnyEncodable(encodable))
        let json = try JSONSerialization.jsonObject(with: data, options: [])
        return json
    }

    print("⚠️ Unhandled value type: \(type(of: value))")
    return value
}

func encodeModel<T: Encodable & BeeCodableModel>(_ model: T) throws -> Data {
    var dict = [String: Any]()
    let mirror = Mirror(reflecting: model)
    let typeMap = T.serverTypeMap

    for child in mirror.children {
        guard let key = child.label else { continue }
        let value = child.value

        guard let type = typeMap[key] else {
            do {
                dict[key] = try encodeNestedValue(value)
            } catch {
                print("⚠️ Failed to encode field '\(key)': \(error)")
                dict[key] = NSNull()
            }
            continue
        }

        do {
            switch type.uppercased() {
            case "DATE":
                if let date = value as? Date {
                    dict[key] = ServerDateFormatter.dateFormatter.string(from: date)
                }
            case "TIME":
                if let date = value as? Date {
                    dict[key] = ServerDateFormatter.timeFormatter.string(from: date)
                }
            case "DATETIME":
                if let date = value as? Date {
                    dict[key] = ServerDateFormatter.datetimeFormatter.string(from: date)
                }
            case "UUID":
                if let uuid = value as? UUID {
                    dict[key] = uuid.uuidString
                }
            default:
                dict[key] = try encodeNestedValue(value)
            }
        } catch {
            print("⚠️ Error encoding key '\(key)' with type '\(type)': \(error)")
            dict[key] = NSNull()
        }
    }
    if !JSONSerialization.isValidJSONObject(dict) {
        print("❌ Dictionary is not a valid JSON object!")
        throw EncodingError.invalidValue(
            dict,
            .init(codingPath: [], debugDescription: "Top-level dictionary is not valid JSON")
        )
    }

    return try JSONSerialization.data(withJSONObject: dict, options: [])
}

// MARK: - Optional extension for convenience
extension BeeCodableModel where Self: Encodable {
    func encodedData() throws -> Data {
        return try encodeModel(self)
    }
}
