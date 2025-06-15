import SwiftUI

struct BankAccountFormView: View {
    @EnvironmentObject var userViewModel: UserViewModel
    @State private var bankAccount = BankAccount()
    @State private var formBuilderStorage: [String: Any] = [:]

    var body: some View {
        VStack {
            Text("Bank Account Form")
                .font(.headline)

            GenericFormBuilder(model: $bankAccount, storage: $formBuilderStorage)

            Button("Create") {
                if let data = try? JSONSerialization.data(withJSONObject: formBuilderStorage),
                   let updated = try? JSONDecoder().decode(BankAccount.self, from: data) {
                    bankAccount = updated
                    userViewModel.createAccount(account: bankAccount, type: .BankAccount)
                } else {
                    print("❌ Could not update model before saving")
                }
            }
            .padding()
        }
        .padding()
    }
}
