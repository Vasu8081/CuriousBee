import Foundation

class ServerEndPoints {
    static let shared: ServerEndPoints = {
        #if DEBUG
            return ServerEndPoints(baseURL: "http://localhost:8000/")
        #else
          return ServerEndPoints(baseURL: "https://app.curiousbytes.in")
        #endif
    }()
    
    var baseURL: String
    private init(baseURL: String) {
        self.baseURL = baseURL
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

    func send<T: Decodable>(_ request: URLRequest, expecting: T.Type, completion: @escaping (Result<T, Error>) -> Void) {
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
}
