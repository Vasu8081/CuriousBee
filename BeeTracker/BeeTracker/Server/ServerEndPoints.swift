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

    func send<T>(_ request: URLRequest, expecting: T.Type, completion: @escaping (Result<T, Error>) -> Void) {
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

            if T.self == [String: Any].self,
               let json = try? JSONSerialization.jsonObject(with: data) as? T {
                DispatchQueue.main.async { completion(.success(json)) }
                return
            }

            DispatchQueue.main.async { completion(.failure(ServerError.decodingFailed)) }
        }.resume()
    }
}
