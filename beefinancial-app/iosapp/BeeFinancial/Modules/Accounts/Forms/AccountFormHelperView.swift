import SwiftUI

struct AccountFormHelperView: View {
    let type: AccountTypes
    let model: (any Account)?

    var body: some View {
        switch type {
        case .BankAccount:
            if let model = model as? BankAccount {
                BankAccountFormView(model: model)
            } else {
                BankAccountFormView()
            }

        case .CreditCardAccount:
            if let model = model as? CreditCardAccount {
                CreditCardAccountFormView(model: model)
            } else {
                CreditCardAccountFormView()
            }

        // Add more cases here if you support more types
        }
    }
}
