import Foundation

extension ServerEndPoints {
    func getData(completion: @escaping (Result<[String: Any], Error>) -> Void) {
        guard let token = AuthenticateViewModel.shared.getToken() else {
            completion(.failure(ServerError.missingToken))
            return
        }

        guard let url = URL(string: "\(baseURL)/json/shared.json") else {
            completion(.failure(ServerError.invalidURL))
            return
        }

        var request = URLRequest(url: url)
        request.httpMethod = "GET"
        request.setValue("Bearer \(token)", forHTTPHeaderField: "Authorization")

        send(request, expecting: [String: Any].self, completion: completion)
    }

    func postData(jsonData: [String: Any], completion: @escaping (Result<String, Error>) -> Void) {

        guard let token = AuthenticateViewModel.shared.getToken() else {
            completion(.failure(ServerError.missingToken))
            return
        }

        guard let url = URL(string: "\(baseURL)/json/shared.json") else {
            completion(.failure(ServerError.invalidURL))
            return
        }

        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        request.httpBody = try? JSONSerialization.data(withJSONObject: jsonData)
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        request.setValue("Bearer \(token)", forHTTPHeaderField: "Authorization")

        send(request, expecting: String.self, completion: completion)
    }
}
