import Foundation

struct BankAccount: Account, Codable, Hashable, DisplayableModel {
    var id: UUID = UUID()
    var account_number: String = ""

    var ifsc_code: String = ""

    func getDisplayNames() -> [String: DisplayFieldInfo] {
        return [
            "account_number" : DisplayFieldInfo(label: "Account Number", type: "String"),
            "ifsc_code" : DisplayFieldInfo(label: "IFSC Code", type: "String"),
        ]
    }

}
