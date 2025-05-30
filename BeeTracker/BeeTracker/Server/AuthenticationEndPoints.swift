import Foundation

extension ServerEndPoints {
    
    func signIn(email: String, password: String, completion: @escaping (Result<AuthUser, Error>) -> Void) {
        guard let url = URL(string: "\(baseURL)auth/login") else {
            completion(.failure(ServerError.invalidURL))
            return
        }

        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")

        struct LoginBody: Encodable {
            let email: String
            let password: String
        }

        struct LoginResponse: Decodable {
            let message: String
            let tokens: TokenPair
            let groupId: UUID
        }

        struct TokenPair: Decodable {
            let access_token: String
            let refresh_token: String
        }

        let loginBody = LoginBody(email: email, password: password)
        request.httpBody = try? JSONEncoder().encode(loginBody)

        URLSession.shared.dataTask(with: request) { data, response, error in
            if let error = error {
                DispatchQueue.main.async {
                    completion(.failure(error))
                }
                return
            }

            guard let data = data else {
                DispatchQueue.main.async {
                    completion(.failure(ServerError.noData))
                }
                return
            }

            do {
                let decoded = try JSONDecoder().decode(LoginResponse.self, from: data)

                let accessExp = Date().addingTimeInterval(15 * 60)
                let refreshExp = Date().addingTimeInterval(30 * 24 * 60 * 60)

                let user = AuthUser(
                    userEmail: email,
                    refreshToken: decoded.tokens.refresh_token,
                    accessToken: decoded.tokens.access_token,
                    refreshTokenExpirationDate: refreshExp,
                    accessTokenExpirationDate: accessExp,
                    groupId: decoded.groupId.uuidString
                )
                

                DispatchQueue.main.async {
                    completion(.success(user))
                }

            } catch {
                print("JSON Decode Error: \(error)")
                print(String(data: data, encoding: .utf8) ?? "Invalid UTF-8 data")
                DispatchQueue.main.async {
                    completion(.failure(ServerError.decodingFailed))
                }
            }
        }.resume()
    }
    
    func refreshToken(currentUser: AuthUser, completion: @escaping (Result<AuthUser, Error>) -> Void) {
        guard currentUser.refreshTokenExpirationDate > Date() else {
            completion(.failure(ServerError.expiredToken))
            return
        }

        guard let url = URL(string: "\(baseURL)auth/refresh") else {
            completion(.failure(ServerError.invalidURL))
            return
        }

        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")

        struct RefreshBody: Encodable {
            let refresh_token: String
        }

        struct LoginResponse: Decodable {
            let message: String
            let tokens: TokenPair
            let groupId: UUID
        }

        struct TokenPair: Decodable {
            let access_token: String
            let refresh_token: String
        }
        
        let loginBody = RefreshBody(refresh_token: currentUser.refreshToken)
        request.httpBody = try? JSONEncoder().encode(loginBody)
        
        URLSession.shared.dataTask(with: request) { data, response, error in
            if let error = error {
                DispatchQueue.main.async {
                    completion(.failure(error))
                }
                return
            }
            
            guard let data = data else {
                DispatchQueue.main.async {
                    completion(.failure(ServerError.noData))
                }
                return
            }
            
            do {
                let decoded = try JSONDecoder().decode(LoginResponse.self, from: data)
                
                let accessExp = Date().addingTimeInterval(15 * 60)
                let refreshExp = Date().addingTimeInterval(30 * 24 * 60 * 60)
                
                let user = AuthUser(
                    userEmail: currentUser.userEmail,
                    refreshToken: decoded.tokens.refresh_token,
                    accessToken: decoded.tokens.access_token,
                    refreshTokenExpirationDate: refreshExp,
                    accessTokenExpirationDate: accessExp,
                    groupId: decoded.groupId.uuidString
                )
                
                DispatchQueue.main.async {
                    completion(.success(user))
                }
                
            } catch {
                print("JSON Decode Error: \(error)")
                print(String(data: data, encoding: .utf8) ?? "Invalid UTF-8 data")
                DispatchQueue.main.async {
                    completion(.failure(ServerError.decodingFailed))
                }
            }
        }.resume()
    }
}
