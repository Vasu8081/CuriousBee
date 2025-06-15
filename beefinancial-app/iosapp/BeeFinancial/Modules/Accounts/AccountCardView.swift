import SwiftUI

struct AccountCardView: View {
    var account: any Account
    var type: AccountTypes?
    var onEdit: (() -> Void)? = nil
    var onDelete: (() -> Void)? = nil

    var body: some View {
        HStack {
            VStack(alignment: .leading, spacing: 6) {
                Text(account.name)
                    .font(.headline)
                Text("Portfolio Value: \(String(format: "%.2f", account.portfolio_value))")
                    .font(.subheadline)
                    .foregroundColor(account.portfolio_value >= 0 ? .green : .red)
            }

            Spacer()

            Menu {
                Button("Edit", systemImage: "pencil", action: { onEdit?() })
                Button("Delete", systemImage: "trash", role: .destructive, action: { onDelete?() })
            } label: {
                Image(systemName: "ellipsis")
                    .rotationEffect(.degrees(90))  // makes it vertical
                    .padding(.horizontal, 8)
                    .contentShape(Rectangle())
            }
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
