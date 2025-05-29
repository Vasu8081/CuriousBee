import Foundation

extension ServerEndPoints {
    func sendMessage(text: String, completion: @escaping (Result<String, Error>) -> Void) {
        guard let token = AuthenticateViewModel.shared.getToken() else {
            completion(.failure(ServerError.missingToken))
            return
        }
        
        guard let url = URL(string: "\(baseURL)/send/message") else {
            completion(.failure(ServerError.invalidURL))
            return
        }
        
        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        request.httpBody = try? JSONSerialization.data(withJSONObject: ["message": text], options: [])
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        request.setValue("Bearer \(token)", forHTTPHeaderField: "Authorization")

        send(request, expecting: String.self, completion: completion)
    }
}
