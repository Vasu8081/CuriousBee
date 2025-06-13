import Foundation

// MARK: - Protocol your models conform to
protocol BeeCodableModel {
    static var serverTypeMap: [String: String] { get }
}

// MARK: - Optional detection helper
protocol AnyOptional {
    var isNil: Bool { get }
}

extension Optional: AnyOptional {
    var isNil: Bool { self == nil }
}

// MARK: - Shared date formatters
enum ServerDateFormatter {
    static func formatter(with format: String) -> DateFormatter {
        let formatter = DateFormatter()
        formatter.calendar = Calendar(identifier: .iso8601)
        formatter.locale = Locale(identifier: "en_US_POSIX")
        formatter.timeZone = TimeZone(secondsFromGMT: 0)
        formatter.dateFormat = format
        return formatter
    }

    static let datetimeFormatter = formatter(with: "yyyy-MM-dd'T'HH:mm:ss.SSSSSS")
    static let dateFormatter = formatter(with: "yyyy-MM-dd")
    static let timeFormatter = formatter(with: "HH:mm:ss")
}

// MARK: - Type-erased Encodable wrapper
struct AnyEncodable: Encodable {
    private let _encode: (Encoder) throws -> Void

    init<T: Encodable>(_ wrapped: T) {
        self._encode = { encoder in
            try wrapped.encode(to: encoder)
        }
    }

    func encode(to encoder: Encoder) throws {
        try _encode(encoder)
    }
}

// MARK: - Recursive encoder
func encodeNestedValue(_ value: Any) throws -> Any {
    if let optional = value as? AnyOptional, optional.isNil {
        return NSNull()
    }

    switch value {
    case let rawEnum as any RawRepresentable:
        return rawEnum.rawValue
    case is String, is Int, is Double, is Bool, is UUID:
        return value
    case let date as Date:
        return ServerDateFormatter.datetimeFormatter.string(from: date)
    case let array as [Any]:
        return try array.map { try encodeNestedValue($0) }
    case let nested as (Encodable & BeeCodableModel):
        let data = try encodeModel(nested)
        return try JSONSerialization.jsonObject(with: data, options: [])
    case let encodable as Encodable:
        let data = try JSONEncoder().encode(AnyEncodable(encodable))
        return try JSONSerialization.jsonObject(with: data, options: .fragmentsAllowed)
    default:
        print("⚠️ Unhandled value type: \(type(of: value))")
        return value
    }
}

// MARK: - Encode a model
func encodeModel<T: Encodable & BeeCodableModel>(_ model: T) throws -> Data {
    var dict = [String: Any]()
    let mirror = Mirror(reflecting: model)
    let typeMap = T.serverTypeMap

    for child in mirror.children {
        guard let key = child.label else { continue }
        let value = child.value

        guard let type = typeMap[key]?.uppercased() else {
            dict[key] = try? encodeNestedValue(value)
            continue
        }

        do {
            switch type {
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

    guard JSONSerialization.isValidJSONObject(dict) else {
        throw EncodingError.invalidValue(
            dict,
            .init(codingPath: [], debugDescription: "Top-level dictionary is not valid JSON")
        )
    }

    return try JSONSerialization.data(withJSONObject: dict, options: [])
}

// MARK: - Model extension
extension BeeCodableModel where Self: Encodable {
    func encodedData() throws -> Data {
        return try encodeModel(self)
    }
}

// MARK: - Decoder Factory
func makeFlexibleJSONDecoder() -> JSONDecoder {
    let decoder = JSONDecoder()
    decoder.dateDecodingStrategy = .custom { decoder in
        let container = try decoder.singleValueContainer()
        let value = try container.decode(String.self)

        let formats = [
            "yyyy-MM-dd'T'HH:mm:ss.SSSSSS",
            "yyyy-MM-dd'T'HH:mm:ss",       // ✅ Add this format (no microseconds)
            "yyyy-MM-dd",                  // date only
            "HH:mm:ss"                     // time only
        ]

        for format in formats {
            let formatter = DateFormatter()
            formatter.dateFormat = format
            formatter.locale = Locale(identifier: "en_US_POSIX")
            formatter.timeZone = TimeZone(secondsFromGMT: 0)

            if let date = formatter.date(from: value) {
                return date
            }
        }

        throw DecodingError.dataCorruptedError(
            in: container,
            debugDescription: "Unknown date format: \(value)"
        )
    }
    return decoder
}

// MARK: - Decoding for BeeCodableModel
func decodeModel<T: Decodable>(_ data: Data, as type: T.Type) throws -> T {
    let decoder = makeFlexibleJSONDecoder()
    return try decoder.decode(T.self, from: data)
}

func decodeModel<T: Decodable & BeeCodableModel>(_ data: Data, as type: T.Type) throws -> T {
    let decoder = makeFlexibleJSONDecoder()
    return try decoder.decode(T.self, from: data)
}
