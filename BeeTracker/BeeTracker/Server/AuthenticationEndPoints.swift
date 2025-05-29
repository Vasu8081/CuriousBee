import Foundation

extension ServerEndPoints {
    
    func signIn(email: String, password: String, completion: @escaping (Result<AuthUser, Error>) -> Void) {
        guard let url = URL(string: "\(baseURL)/login") else {
            completion(.failure(ServerError.invalidURL))
            return
        }

        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")

        let body: [String: String] = [
            "username": email,
            "password": password
        ]

        request.httpBody = try? JSONSerialization.data(withJSONObject: body)

        URLSession.shared.dataTask(with: request) { data, response, error in
            if let error = error {
                DispatchQueue.main.async {
                    completion(.failure(error))
                }
                return
            }

            guard let data = data,
                  let json = try? JSONSerialization.jsonObject(with: data) as? [String: String],
                  let accessToken = json["access_token"],
                  let refreshToken = json["refresh_token"] else {
                DispatchQueue.main.async {
                    completion(.failure(ServerError.decodingFailed))
                }
                return
            }

            let accessExp = Date().addingTimeInterval(15 * 60)
            let refreshExp = Date().addingTimeInterval(30 * 24 * 60 * 60)

            let user = AuthUser(
                userEmail: email,
                refreshToken: refreshToken,
                accessToken: accessToken,
                refreshTokenExpirationDate: refreshExp,
                accessTokenExpirationDate: accessExp
            )

            DispatchQueue.main.async {
                completion(.success(user))
            }
        }.resume()
    }
    
    func refreshToken(currentUser: AuthUser, completion: @escaping (Result<AuthUser, Error>) -> Void) {
        guard currentUser.refreshTokenExpirationDate > Date() else {
            completion(.failure(ServerError.expiredToken))
            return
        }

        guard let url = URL(string: "\(baseURL)/refresh") else {
            completion(.failure(ServerError.invalidURL))
            return
        }

        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        request.httpBody = try? JSONSerialization.data(withJSONObject: [
            "refresh_token": currentUser.refreshToken
        ])

        URLSession.shared.dataTask(with: request) { data, response, error in
            if let error = error {
                DispatchQueue.main.async {
                    completion(.failure(error))
                }
                return
            }

            guard let data = data,
                  let json = try? JSONSerialization.jsonObject(with: data) as? [String: String],
                  let newAccessToken = json["access_token"],
                  let newRefreshToken = json["refresh_token"] else {
                DispatchQueue.main.async {
                    completion(.failure(ServerError.decodingFailed))
                }
                return
            }

            let accessExp = Date().addingTimeInterval(15 * 60)
            let refreshExp = Date().addingTimeInterval(30 * 24 * 60 * 60)

            let updatedUser = AuthUser(
                userEmail: currentUser.userEmail,
                refreshToken: newRefreshToken,
                accessToken: newAccessToken,
                refreshTokenExpirationDate: refreshExp,
                accessTokenExpirationDate: accessExp
            )

            DispatchQueue.main.async {
                completion(.success(updatedUser))
            }
        }.resume()
    }
}
