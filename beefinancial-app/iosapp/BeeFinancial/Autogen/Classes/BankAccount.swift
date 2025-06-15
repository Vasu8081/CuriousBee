import Foundation

struct BankAccount: Account, Codable, Hashable, DisplayableModel {
    var id: UUID = UUID()
    var name: String = ""
    var account_number: String = ""
    var ifsc_code: String = ""
    var portfolio_value: Double = 0.0

    enum CodingKeys: String, CodingKey {
        case id
        case name
        case account_number
        case ifsc_code
        case portfolio_value
    }

    init(from decoder: Decoder) throws {
        let container = try decoder.container(keyedBy: CodingKeys.self)
        self.id = try container.decodeIfPresent(UUID.self, forKey: .id) ?? UUID()
        self.name = try container.decodeIfPresent(String.self, forKey: .name) ?? ""
        self.account_number = try container.decodeIfPresent(String.self, forKey: .account_number) ?? ""
        self.ifsc_code = try container.decodeIfPresent(String.self, forKey: .ifsc_code) ?? ""
        self.portfolio_value = try container.decodeIfPresent(Double.self, forKey: .portfolio_value) ?? 0.0
    }

    init() {
    }

    func getDisplayNames() -> [String: DisplayFieldInfo] {
        return [
            "name": DisplayFieldInfo(label: "Account Name", type: "String"),
            "account_number": DisplayFieldInfo(label: "Account Number", type: "String"),
            "ifsc_code": DisplayFieldInfo(label: "IFSC Code", type: "String"),
        ]
    }
}
