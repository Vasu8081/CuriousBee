import SwiftUI

struct CategoryDetailView: View {
    var category: Category
    @ObservedObject var userViewModel: UserViewModel
    var selectedRange: CategoryFilterRange

    var transactions: [Transaction] {
        userViewModel.userData.allTransactions
            .filter {
                $0.category.id == category.id &&
                selectedRange.matches($0.date)
            }
            .sorted { $0.date > $1.date }
    }

    var body: some View {
        List(transactions) { txn in
            VStack(alignment: .leading, spacing: 4) {
                HStack {
                    Text(txn.date.formatted(date: .abbreviated, time: .omitted))
                    Spacer()
                    Text(String(format: "₹%.2f", txn.amount))
                        .foregroundColor(txn.flow == .income ? .green :
                                         txn.flow == .expense ? .red : .blue)
                }

                if let note = txn.note {
                    Text(note)
                        .font(.subheadline)
                        .foregroundColor(.gray)
                }

                HStack {
                    if let from = txn.fromAccountID,
                       let fromAccount = userViewModel.userData.accounts.first(where: { $0.id == from }) {
                        Text("From: \(fromAccount.name)").font(.footnote)
                    }

                    if let to = txn.toAccountID,
                       let toAccount = userViewModel.userData.accounts.first(where: { $0.id == to }) {
                        Text("To: \(toAccount.name)").font(.footnote)
                    }
                }
            }
            .padding(.vertical, 4)
        }
        .navigationTitle("\(category.emoji ?? "") \(category.name)")
    }
}
