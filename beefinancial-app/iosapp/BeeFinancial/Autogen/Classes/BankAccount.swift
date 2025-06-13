import Foundation

struct BankAccount: Account, Codable, Hashable {
    var id: UUID = UUID()
    var account_number: String?
    var ifsc_code: String?
}
