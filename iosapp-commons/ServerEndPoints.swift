import Foundation

class ServerEndPoints: ObservableObject {
    static let shared = ServerEndPoints()

    var appName: String = ""

    var baseURL: String {
        isDev ? "https://\(appName)-dev.curiousbytes.in/" : "https://\(appName).curiousbytes.in/"
    }

    @Published var isDev: Bool {
        didSet {
            UserDefaults.standard.set(isDev, forKey: "isDevMode")
        }
    }

    private init() {
        self.isDev = UserDefaults.standard.bool(forKey: "isDevMode")
    }

    enum ServerError: Error {
        case invalidURL, noData, decodingFailed, missingToken, expiredToken
    }

    func toggleMode() {
        isDev.toggle()
        if isDev {
            ToastManager.shared.show("In Developer Mode", type: .warning, persistent: true)
        } else {
            ToastManager.shared.hidePersistentWarning()
        }
        AuthenticateViewModel.shared.signOut()
    }

    // Generic Send for any Decodable
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
                let decoded = try decodeModel(data, as: T.self)
                DispatchQueue.main.async { completion(.success(decoded)) }
            } catch {
                print("Decoding failed: \(error)")
                print("Response Data: \(String(data: data, encoding: .utf8) ?? "")")
                DispatchQueue.main.async { completion(.failure(ServerError.decodingFailed)) }
            }
        }.resume()
    }

    // Send for BeeCodableModel with typeMap support
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
                let decoded = try decodeModel(data, as: T.self)
                DispatchQueue.main.async { completion(.success(decoded)) }
            } catch {
                print("Decoding failed: \(error)")
                print("Response Data: \(String(data: data, encoding: .utf8) ?? "")")
                DispatchQueue.main.async { completion(.failure(ServerError.decodingFailed)) }
            }
        }.resume()
    }
}
