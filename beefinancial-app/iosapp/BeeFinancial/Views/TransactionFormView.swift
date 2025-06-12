import SwiftUI

struct TransactionFormView: View {
    @Environment(\.dismiss) var dismiss

    var type: TransactionFlow
    var allAccounts: [AnyAccount]
    var categories: [Category]
    var currentAccountID: UUID
    var onSubmit: (Transaction, UUID?) -> Void  // linked account (from or to)

    @State private var amount: String = ""
    @State private var note: String = ""
    @State private var date: Date = Date()
    @State private var selectedAccountID: UUID? = nil
    @State private var selectedCategory: Category? = nil

    var otherAccounts: [AnyAccount] {
        allAccounts.filter { $0.id != currentAccountID }
    }

    var filteredCategories: [Category] {
        categories.filter { $0.type.rawValue == type.rawValue }
    }

    var formTitle: String {
        switch type {
        case .income: return "Add Income"
        case .expense: return "Add Expense"
        case .transfer: return "Add Transfer"
        }
    }

    var fromOrToLabel: String {
        switch type {
        case .income: return "From Account"
        case .expense: return "To Account"
        case .transfer: return "To Account"
        }
    }

    var body: some View {
        NavigationView {
            Form {
                Section(header: Text("Details")) {
                    TextField("Amount", text: $amount)
                        .keyboardType(.decimalPad)

                    TextField("Note", text: $note)

                    Picker("Category", selection: $selectedCategory) {
                        Text("Select Category").tag(Category?.none)
                        ForEach(filteredCategories, id: \.id) { cat in
                            Text("\(cat.emoji ?? "") \(cat.name)").tag(Optional(cat))
                        }
                    }

                    DatePicker("Date", selection: $date, displayedComponents: .date)
                }

                if type != .expense && type != .income {
                    Section(header: Text(fromOrToLabel)) {
                        Picker(fromOrToLabel, selection: $selectedAccountID) {
                            Text("None").tag(UUID?.none)
                            ForEach(otherAccounts, id: \.id) { acc in
                                Text(acc.name).tag(Optional(acc.id))
                            }
                        }
                    }
                }
            }
            .navigationTitle(formTitle)
            .toolbar {
                ToolbarItem(placement: .cancellationAction) {
                    Button("Cancel") {
                        dismiss()
                    }
                }

                ToolbarItem(placement: .confirmationAction) {
                    Button("Add") {
                        guard let amountValue = Double(amount),
                              amountValue > 0,
                              let category = selectedCategory else { return }

                        let txn = Transaction(
                            id: UUID(),
                            date: date,
                            amount: amountValue,
                            note: note.isEmpty ? nil : note,
                            category: category,
                            fromAccountID: type == .expense ? currentAccountID :
                                            type == .transfer ? currentAccountID : nil,
                            toAccountID: type == .income ? currentAccountID :
                                          type == .transfer ? (selectedAccountID ?? currentAccountID) : nil
                        )

                        onSubmit(txn, selectedAccountID)
                        dismiss()
                    }
                    .disabled(Double(amount) == nil || selectedCategory == nil)
                }
            }
        }
        .onAppear {
            if selectedCategory == nil {
                selectedCategory = filteredCategories.first
            }
        }
    }
}
