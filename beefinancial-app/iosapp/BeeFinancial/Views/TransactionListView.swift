import SwiftUI

struct TransactionListView: View {
    @ObservedObject var userViewModel: UserViewModel

    var body: some View {
        NavigationView {
            List {
                ForEach(userViewModel.userData.allTransactions) { txn in
                    VStack(alignment: .leading) {
                        HStack {
                            Text(txn.flow.rawValue.capitalized)
                                .foregroundColor(
                                    txn.flow == .income ? .green :
                                    txn.flow == .expense ? .red :
                                    .blue
                                )
                            Spacer()
                            Text(txn.date.formatted(date: .abbreviated, time: .omitted))
                                .font(.subheadline)
                                .foregroundColor(.gray)
                        }

                        Text(String(format: "₹%.2f", txn.amount))
                            .font(.headline)

                        Text("\(txn.category.emoji ?? "") \(txn.category.name)")
                            .font(.subheadline)
                            .foregroundColor(.gray)

                        if let note = txn.note {
                            Text(note)
                                .font(.footnote)
                                .foregroundColor(.gray)
                        }
                    }
                    .padding(.vertical, 4)
                }
            }
            .navigationTitle("All Transactions")
        }
    }
}
