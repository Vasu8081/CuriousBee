import SwiftUI

struct TransactionCreateForm: View {
    @EnvironmentObject var userViewModel: UserViewModel

    @State private var selectedFromAccountID: UUID?
    @State private var selectedToAccountID: UUID?
    @State private var selectedCategory: CategoryTypes = .Groceries
    @State private var transactionDetails = TransactionDetails()

    @State private var showSuccess = false
    @State private var showError = false

    var sortedAccountIDs: [UUID] {
        userViewModel.accounts.keys.sorted()
    }

    var body: some View {
        Form {
            fromAccountSection
            toAccountSection
            categorySection
            transactionDetailsSection

            Button("Save Transaction") {
                saveTransaction()
            }
            .alert("Transaction Saved", isPresented: $showSuccess) {
                Button("OK", role: .cancel) {}
            }

            if showError {
                Text("❌ Please fill in all fields").foregroundColor(.red)
            }
        }
        .navigationTitle("New Transaction")
    }

    private var fromAccountSection: some View {
        Section(header: Text("From Account")) {
            Picker("From", selection: Binding(
                get: { selectedFromAccountID ?? UUID() },
                set: { selectedFromAccountID = $0 }
            )) {
                ForEach(sortedAccountIDs, id: \.self) { id in
                    if let name = userViewModel.accounts[id]?.name {
                        Text(name).tag(id)
                    }
                }
            }
        }
    }

    private var toAccountSection: some View {
        Section(header: Text("To Account")) {
            Picker("To", selection: Binding(
                get: { selectedToAccountID ?? UUID() },
                set: { selectedToAccountID = $0 }
            )) {
                ForEach(sortedAccountIDs, id: \.self) { id in
                    if let name = userViewModel.accounts[id]?.name {
                        Text(name).tag(id)
                    }
                }
            }
        }
    }

    private var categorySection: some View {
        Section(header: Text("Category")) {
            Picker("Category", selection: $selectedCategory) {
                ForEach(CategoryTypes.allCases, id: \.self) { category in
                    Text(category.rawValue.capitalized).tag(category)
                }
            }
        }
    }

    private var transactionDetailsSection: some View {
        Section(header: Text("Details")) {
            DatePicker("Transaction Date", selection: $transactionDetails.transaction_date, displayedComponents: .date)
            TextField("Amount", value: $transactionDetails.amount, format: .number)
                .keyboardType(.decimalPad)
        }
    }

    private func saveTransaction() {
        guard
            let fromID = selectedFromAccountID,
            let toID = selectedToAccountID,
            let fromAccount = userViewModel.accounts[fromID],
            let toAccount = userViewModel.accounts[toID]
        else {
            showError = true
            return
        }

        userViewModel.createTransaction(
            from_account: fromAccount,
            to_account: toAccount,
            transaction_details: transactionDetails,
            category: selectedCategory
        )

        showSuccess = true
        resetForm()
    }

    private func resetForm() {
        selectedFromAccountID = nil
        selectedToAccountID = nil
        selectedCategory = .Groceries
        transactionDetails = TransactionDetails()
        showError = false
    }
}
