import SwiftUI

struct BankAccountFormView: View {
    @EnvironmentObject var viewModel: UserViewModel

    @State private var account = BankAccount()

    var body: some View {
        Form {
            Section(header: Text("Bank Account Details")) {
                TextField("Account Number", text: Binding(
                    get: { account.account_number ?? "" },
                    set: { account.account_number = $0 }
                ))
                .keyboardType(.numberPad)

                TextField("IFSC Code", text: Binding(
                    get: { account.ifsc_code ?? "" },
                    set: { account.ifsc_code = $0 }
                ))
                .autocapitalization(.allCharacters)
            }

            Button("Save Account") {
                viewModel.createAccount(account: account, type: .BankAccount)
            }
        }
        .navigationTitle("Add Bank Account")
    }
}
