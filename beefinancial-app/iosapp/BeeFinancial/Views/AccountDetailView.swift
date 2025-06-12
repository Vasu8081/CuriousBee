import SwiftUI

struct AccountDetailView: View {
    @State var account: AnyAccount
    @ObservedObject var userViewModel: UserViewModel

    @State private var showingIncomeForm = false
    @State private var showingExpenseForm = false
    @State private var showingTransferForm = false

    var body: some View {
        let txns = userViewModel.userData.allTransactions
            .filter { $0.fromAccountID == account.id || $0.toAccountID == account.id }
            .sorted { $0.date > $1.date }

        ZStack(alignment: .bottomTrailing) {
            VStack {
                VStack(alignment: .leading, spacing: 8) {
                    Text(account.name)
                        .font(.title2)
                        .bold()
                    Text("Balance: ₹\(String(format: "%.2f", account.balance))")

                    if let remarks = account.remarks {
                        Text("Remarks: \(remarks)")
                            .foregroundColor(.gray)
                            .font(.subheadline)
                    }
                }
                .padding()
                .background(Color(.systemGray6))
                .cornerRadius(10)
                .padding([.horizontal, .top])

                if txns.isEmpty {
                    Spacer()
                    Text("No transactions yet")
                        .foregroundColor(.gray)
                    Spacer()
                } else {
                    List(txns) { txn in
                        let isIncoming = txn.toAccountID == account.id
                        let direction = txn.fromAccountID != nil && txn.toAccountID != nil ? "Transfer" : isIncoming ? "Income" : "Expense"
                        let color: Color = isIncoming ? .green : txn.flow == .transfer ? .blue : .red

                        VStack(alignment: .leading) {
                            HStack {
                                Text(direction)
                                    .foregroundColor(color)
                                Spacer()
                                Text(txn.date.formatted(date: .abbreviated, time: .omitted))
                                    .font(.subheadline)
                                    .foregroundColor(.gray)
                            }

                            Text(String(format: "\u{20B9}%.2f", txn.amount))
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
            }

            VStack(spacing: 20) {
                Button(action: { showingIncomeForm = true }) {
                    Image(systemName: "plus.circle.fill")
                        .resizable()
                        .frame(width: 56, height: 56)
                        .foregroundColor(.green)
                }

                Button(action: { showingExpenseForm = true }) {
                    Image(systemName: "minus.circle.fill")
                        .resizable()
                        .frame(width: 56, height: 56)
                        .foregroundColor(.red)
                }

                Button(action: { showingTransferForm = true }) {
                    Image(systemName: "arrow.left.arrow.right.circle.fill")
                        .resizable()
                        .frame(width: 56, height: 56)
                        .foregroundColor(.blue)
                }
            }
            .padding(.bottom, 32)
            .padding(.trailing, 20)
        }
        .sheet(isPresented: $showingIncomeForm) {
            TransactionFormView(
                type: .income,
                allAccounts: userViewModel.userData.accounts,
                categories: userViewModel.userData.categories,
                currentAccountID: account.id
            ) { txn, linkedAccountID in
                handleAddTransaction(txn, linkedAccountID: linkedAccountID)
            }
        }
        .sheet(isPresented: $showingExpenseForm) {
            TransactionFormView(
                type: .expense,
                allAccounts: userViewModel.userData.accounts,
                categories: userViewModel.userData.categories,
                currentAccountID: account.id
            ) { txn, linkedAccountID in
                handleAddTransaction(txn, linkedAccountID: linkedAccountID)
            }
        }
        .sheet(isPresented: $showingTransferForm) {
            TransactionFormView(
                type: .transfer,
                allAccounts: userViewModel.userData.accounts,
                categories: userViewModel.userData.categories,
                currentAccountID: account.id
            ) { txn, linkedAccountID in
                handleAddTransaction(txn, linkedAccountID: linkedAccountID)
            }
        }
        .navigationTitle("Account Details")
        .navigationBarTitleDisplayMode(.inline)
    }

    private func handleAddTransaction(_ txn: Transaction, linkedAccountID: UUID?) {
        let fromID = txn.fromAccountID
        let toID = txn.toAccountID

        if let fromID = fromID,
           let fromIndex = userViewModel.userData.accounts.firstIndex(where: { $0.id == fromID }) {
            var fromAcc = userViewModel.userData.accounts[fromIndex]
            var txns = fromAcc.transactions
            txns.insert(txn, at: 0)
            let newBalance = fromAcc.balance - txn.amount
            fromAcc = updatedWith(account: fromAcc, newBalance: newBalance, transactions: txns)
            userViewModel.userData.accounts[fromIndex] = fromAcc

            if account.id == fromID {
                account = fromAcc
            }
        }

        if let toID = toID,
           let toIndex = userViewModel.userData.accounts.firstIndex(where: { $0.id == toID }) {
            var toAcc = userViewModel.userData.accounts[toIndex]
            let newBalance = toAcc.balance + txn.amount
            toAcc = updatedWith(account: toAcc, newBalance: newBalance, transactions: toAcc.transactions)
            userViewModel.userData.accounts[toIndex] = toAcc

            if account.id == toID {
                account = toAcc
            }
        }

        userViewModel.needSync = true
    }

    private func updatedWith(account: AnyAccount, newBalance: Double, transactions: [Transaction]) -> AnyAccount {
        switch account {
        case .bank(var b): b.balance = newBalance; b.transactions = transactions; return .bank(b)
        case .loan(var l): l.balance = newBalance; l.transactions = transactions; return .loan(l)
        case .creditCard(var c): c.balance = newBalance; c.transactions = transactions; return .creditCard(c)
        case .borrow(var b): b.balance = newBalance; b.transactions = transactions; return .borrow(b)
        case .give(var g): g.balance = newBalance; g.transactions = transactions; return .give(g)
        case .fd(var f): f.balance = newBalance; f.transactions = transactions; return .fd(f)
        case .rd(var r): r.balance = newBalance; r.transactions = transactions; return .rd(r)
        case .chit(var c): c.balance = newBalance; c.transactions = transactions; return .chit(c)
        case .property(var p): p.balance = newBalance; p.transactions = transactions; return .property(p)
        case .stock(var s): s.balance = newBalance; s.transactions = transactions; return .stock(s)
        }
    }
}
