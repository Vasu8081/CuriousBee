import Foundation

class ServerEndPoints : ObservableObject {
    static let shared = ServerEndPoints()
    
    var baseURL: String {
        isDev ? "https://beefinancial-dev.curiousbytes.in/" : "https://beefinancial.curiousbytes.in/"
    }
    
    @Published var isDev: Bool {
        didSet {
            UserDefaults.standard.set(isDev, forKey: "isDevMode")
        }
    }
    
    func toggleMode() {
        isDev.toggle()
        if isDev {
            ToastManager.shared.show(
                "In Developer Mode",
                type: .warning,
                persistent: true
            )
        } else {
            ToastManager.shared.hidePersistentWarning()
        }
        AuthenticateViewModel.shared.signOut()
    }
    
    private init() {
        self.isDev = UserDefaults.standard.bool(forKey: "isDevMode")
    }
    
    enum ServerError: Error {
        case invalidURL
        case noData
        case decodingFailed
        case missingToken
        case expiredToken
    }
    
    func customJSONDecoder() -> JSONDecoder {
        let decoder = JSONDecoder()

        // Flexible date decoding strategy
        decoder.dateDecodingStrategy = .custom { decoder in
            let container = try decoder.singleValueContainer()
            let dateString = try container.decode(String.self)

            let formats = [
                "yyyy-MM-dd'T'HH:mm:ss.SSSSSS",  // full ISO with microseconds
                "yyyy-MM-dd'T'HH:mm:ss",         // ISO without milliseconds
                "yyyy-MM-dd",                    // just date
                "HH:mm:ss"                       // time only
            ]

            for format in formats {
                let formatter = DateFormatter()
                formatter.dateFormat = format
                formatter.locale = Locale(identifier: "en_US_POSIX")
                if let date = formatter.date(from: dateString) {
                    return date
                }
            }

            throw DecodingError.dataCorruptedError(
                in: container,
                debugDescription: "Date string does not match any expected format: \(dateString)"
            )
        }

        return decoder
    }
    
    func decodeModel<T: Decodable>(_ data: Data, as type: T.Type, using typeMap: [String: String]) throws -> T {
        let decoder = JSONDecoder()

        decoder.dateDecodingStrategy = .custom { decoder in
            let container = try decoder.singleValueContainer()
            let value = try container.decode(String.self)

            if let d = ServerDateFormatter.datetimeFormatter.date(from: value) { return d }
            if let d = ServerDateFormatter.dateFormatter.date(from: value) { return d }
            if let d = ServerDateFormatter.timeFormatter.date(from: value) { return d }

            throw DecodingError.dataCorruptedError(in: container, debugDescription: "Unknown date format: \\(value)")
        }

        return try decoder.decode(T.self, from: data)
    }
    
    func send<T: Decodable>(_ request: URLRequest, expecting: T.Type, completion: @escaping (Result<T, Error>) -> Void) {
        print("Using decodable")
        URLSession.shared.dataTask(with: request) { data, response, error in
            if let error = error {
                DispatchQueue.main.async { completion(.failure(error)) }
                return
            }

            guard let data = data else {
                DispatchQueue.main.async { completion(.failure(ServerError.noData)) }
                return
            }

            if T.self == String.self, let result = String(data: data, encoding: .utf8) as? T {
                DispatchQueue.main.async { completion(.success(result)) }
                return
            }

            do {
                let decoded = try self.customJSONDecoder().decode(T.self, from: data)
                DispatchQueue.main.async { completion(.success(decoded)) }
            } catch {
                print("Decoding failed: \(error)")
                print("Response Data: \(String(data: data, encoding: .utf8) ?? "")")
                DispatchQueue.main.async { completion(.failure(ServerError.decodingFailed)) }
            }
        }.resume()
    }
    
    func send<T: Decodable & BeeCodableModel>(_ request: URLRequest, expecting: T.Type, completion: @escaping (Result<T, Error>) -> Void) {
        print("Using Beedecodable")
        URLSession.shared.dataTask(with: request) { data, response, error in
            if let error = error {
                DispatchQueue.main.async { completion(.failure(error)) }
                return
            }

            guard let data = data else {
                DispatchQueue.main.async { completion(.failure(ServerError.noData)) }
                return
            }

            if T.self == String.self, let result = String(data: data, encoding: .utf8) as? T {
                DispatchQueue.main.async { completion(.success(result)) }
                return
            }

            do {
                let decoded = try self.decodeModel(data, as: T.self, using: T.serverTypeMap)
                DispatchQueue.main.async { completion(.success(decoded)) }
            } catch {
                print("Decoding failed: \(error)")
                print("Response Data: \(String(data: data, encoding: .utf8) ?? ",")")
                DispatchQueue.main.async { completion(.failure(ServerError.decodingFailed)) }
            }
        }.resume()
    }
}
