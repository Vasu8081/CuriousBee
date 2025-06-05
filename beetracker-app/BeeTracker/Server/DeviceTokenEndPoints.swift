import Foundation

extension ServerEndPoints {
    func sendDeviceToken(_ user: Users, completion: @escaping (Result<String, Error>) -> Void) {
        guard let url = URL(string: "\(baseURL)users/device_token") else {
            completion(.failure(ServerError.invalidURL))
            return
        }
        
        guard let auth_token = AuthenticateViewModel.shared.getToken() else {
            completion(.failure(ServerError.missingToken))
            return
        }

        var request = URLRequest(url: url)
        request.httpMethod = "POST"
//        request.httpBody = try? JSONEncoder().encode(user)
        request.httpBody = try? encodeModel(user)
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        request.setValue("Bearer \(auth_token)", forHTTPHeaderField: "Authorization")

        send(request, expecting: String.self, completion: completion)
    }
}
