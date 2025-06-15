import SwiftUI

struct CreditCardAccountFormView: View {
    @EnvironmentObject var userViewModel: UserViewModel
    @State private var creditCard = CreditCardAccount()
    @State private var formBuilderStorage: [String: Any] = [:]

    var body: some View {
        VStack {
            Text("Credit Card Form")
                .font(.headline)

            GenericFormBuilder(model: $creditCard, storage: $formBuilderStorage)

            Button("Create") {
                if let data = try? JSONSerialization.data(withJSONObject: formBuilderStorage),
                   let updated = try? JSONDecoder().decode(CreditCardAccount.self, from: data) {
                    creditCard = updated
                    userViewModel.createAccount(account: creditCard, type: .CreditCardAccount)
                } else {
                    print("❌ Could not update model before saving")
                }
            }
            .padding()
        }
        .padding()
    }
}
