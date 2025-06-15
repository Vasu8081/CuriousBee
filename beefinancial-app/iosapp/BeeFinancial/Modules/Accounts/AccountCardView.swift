import SwiftUI

struct AccountCardView: View {
    var account: any Account
    var type: AccountTypes?

    var body: some View {
        VStack(alignment: .leading, spacing: 6) {
            Text(account.name)
                .font(.headline)
            Text("Portfolio Value: \(String(format: "%.2f", account.portfolio_value))")
                .font(.subheadline)
                .foregroundColor(account.portfolio_value >= 0 ? .green : .red)
        }
        .padding()
        .background(cardColor)
        .cornerRadius(12)
        .shadow(radius: 3)
    }

    private var cardColor: Color {
        switch type {
        case .BankAccount: return Color.blue.opacity(0.15)
        case .CreditCardAccount: return Color.orange.opacity(0.15)
        default: return Color.secondary.opacity(0.1)
        }
    }
}
