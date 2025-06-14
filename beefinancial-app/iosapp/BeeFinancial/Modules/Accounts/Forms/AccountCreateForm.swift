import SwiftUI

struct BankAccountFormView: View {
    @State private var bankAccount = BankAccount()

    var body: some View {
        VStack {
            Text("Bank Account Form")
                .font(.headline)

            GenericFormBuilder(model: $bankAccount)

            Button("Submit") {
                print("Submitted model:", bankAccount)
            }
            .padding()
        }
        .padding()
    }
}
