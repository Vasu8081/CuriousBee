import SwiftUI

struct CreditCardAccountFormView: View {
    @EnvironmentObject var userViewModel: UserViewModel
    @State private var creditCardAccount: CreditCardAccount
    @State private var formBuilderStorage: [String: Any] = [:]
    private let isEdit: Bool

    init(model: CreditCardAccount? = nil) {
        _creditCardAccount = State(initialValue: model ?? CreditCardAccount())
        isEdit = model != nil
    }

    var body: some View {
        VStack {
            Text(isEdit ? "Edit Credit Card" : "Create Credit Card")
                .font(.headline)

            GenericFormBuilder(model: $creditCardAccount, storage: $formBuilderStorage)

            Button(isEdit ? "Save Changes" : "Create") {
                if let data = try? JSONSerialization.data(withJSONObject: formBuilderStorage),
                   var updated = try? JSONDecoder().decode(CreditCardAccount.self, from: data) {
                    updated.id = creditCardAccount.id
                    creditCardAccount = updated

                    if isEdit {
                        userViewModel.accounts[creditCardAccount.id] = creditCardAccount
                        userViewModel.saveAccount(id: creditCardAccount.id)
                    } else {
                        userViewModel.createAccount(account: creditCardAccount, type: .CreditCardAccount)
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
