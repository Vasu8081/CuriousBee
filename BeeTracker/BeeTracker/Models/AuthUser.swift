import Foundation

struct AuthUser: Codable {
    let userEmail: String
    let refreshToken: String
    let accessToken: String
    let refreshTokenExpirationDate: Date
    let accessTokenExpirationDate: Date
}
