import SwiftUI

struct BankAccountFormView: View {
    @EnvironmentObject var userViewModel: UserViewModel
    @State private var bankAccount: BankAccount
    @State private var formBuilderStorage: [String: Any] = [:]
    private let isEdit: Bool

    init(model: BankAccount? = nil) {
        _bankAccount = State(initialValue: model ?? BankAccount())
        isEdit = model != nil
    }

    var body: some View {
        VStack {
            Text(isEdit ? "Edit Bank Account" : "Create Bank Account")
                .font(.headline)

            GenericFormBuilder(model: $bankAccount, storage: $formBuilderStorage)

            Button(isEdit ? "Save Changes" : "Create") {
                if let data = try? JSONSerialization.data(withJSONObject: formBuilderStorage),
                   var updated = try? JSONDecoder().decode(BankAccount.self, from: data) {
                    updated.id = bankAccount.id // retain original ID
                    bankAccount = updated

                    if isEdit {
                        userViewModel.accounts[bankAccount.id] = bankAccount
                        userViewModel.saveAccount(id: bankAccount.id)
                    } else {
                        userViewModel.createAccount(account: bankAccount, type: .BankAccount)
                    }
                } else {
                    print("❌ Could not update model before saving")
                }
            }
            .padding()
        }
        .padding()
    }
}
