import SwiftUI

struct AddAccountFormView: View {
    @Environment(\.dismiss) var dismiss
    var onSave: (AnyAccount) -> Void

    @State private var selectedType: AccountType = .bank

    // Common fields
    @State private var name = ""
    @State private var balance = ""
    @State private var remarks = ""

    // Bank
    @State private var accountNumber = ""
    @State private var ifscCode = ""
    @State private var bankName = ""

    // Loan
    @State private var lenderName = ""
    @State private var interestRate = ""
    @State private var emiAmount = ""
    @State private var dueDate = Date()

    // Credit Card
    @State private var cardProvider = ""
    @State private var cardNumber = ""
    @State private var cardLimit = ""
    @State private var cardDueDate = Date()
    @State private var billingCycleDay = ""

    // Borrow / Give
    @State private var personName = ""
    @State private var contactNumber = ""
    @State private var address = ""

    // FD
    @State private var fdBankName = ""
    @State private var fdInterestRate = ""
    @State private var fdStartDate = Date()
    @State private var fdMaturityDate = Date()

    // RD
    @State private var rdMonthlyAmount = ""
    @State private var rdStartDate = Date()
    @State private var rdMaturityDate = Date()
    @State private var rdBankName = ""
    @State private var rdInterestRate = ""

    // Chit
    @State private var chitGroupName = ""
    @State private var chitMonthlyContribution = ""
    @State private var chitStartDate = Date()
    @State private var chitMaturityDate = Date()

    // Property
    @State private var propertyType = ""
    @State private var location = ""
    @State private var purchaseDate = Date()

    // Stock
    @State private var brokerage = ""
    @State private var dematNumber = ""
    @State private var panNumber = ""
    @State private var openingDate = Date()

    var body: some View {
        NavigationView {
            Form {
                Section(header: Text("Account Type")) {
                    Picker("Type", selection: $selectedType) {
                        ForEach(AccountType.allCases, id: \..self) { type in
                            Text(type.rawValue.capitalized).tag(type)
                        }
                    }
                }

                Section(header: Text("Basic Info")) {
                    TextField("Name", text: $name)
                    TextField("Balance", text: $balance).keyboardType(.decimalPad)
                    TextField("Remarks", text: $remarks)
                }

                switch selectedType {
                case .bank:
                    Section(header: Text("Bank Details")) {
                        TextField("Account Number", text: $accountNumber)
                        TextField("IFSC Code", text: $ifscCode)
                        TextField("Bank Name", text: $bankName)
                    }

                case .loan:
                    Section(header: Text("Loan Details")) {
                        TextField("Lender Name", text: $lenderName)
                        TextField("Interest Rate (%)", text: $interestRate).keyboardType(.decimalPad)
                        TextField("EMI Amount", text: $emiAmount).keyboardType(.decimalPad)
                        DatePicker("Due Date", selection: $dueDate, displayedComponents: .date)
                    }

                case .creditCard:
                    Section(header: Text("Credit Card Details")) {
                        TextField("Provider", text: $cardProvider)
                        TextField("Card Number", text: $cardNumber)
                        TextField("Limit", text: $cardLimit).keyboardType(.decimalPad)
                        DatePicker("Due Date", selection: $cardDueDate, displayedComponents: .date)
                        TextField("Billing Cycle Day", text: $billingCycleDay).keyboardType(.numberPad)
                    }

                case .borrow, .give:
                    Section(header: Text("Person Details")) {
                        TextField("Person Name", text: $personName)
                        TextField("Contact Number", text: $contactNumber)
                        TextField("Address", text: $address)
                        TextField("Interest Rate (%)", text: $interestRate).keyboardType(.decimalPad)
                        DatePicker("Due Date", selection: $dueDate, displayedComponents: .date)
                    }

                case .fd:
                    Section(header: Text("FD Details")) {
                        TextField("Bank Name", text: $fdBankName)
                        TextField("Interest Rate (%)", text: $fdInterestRate).keyboardType(.decimalPad)
                        DatePicker("Start Date", selection: $fdStartDate, displayedComponents: .date)
                        DatePicker("Maturity Date", selection: $fdMaturityDate, displayedComponents: .date)
                    }

                case .rd:
                    Section(header: Text("RD Details")) {
                        TextField("Monthly Amount", text: $rdMonthlyAmount).keyboardType(.decimalPad)
                        DatePicker("Start Date", selection: $rdStartDate, displayedComponents: .date)
                        DatePicker("Maturity Date", selection: $rdMaturityDate, displayedComponents: .date)
                        TextField("Bank Name", text: $rdBankName)
                        TextField("Interest Rate (%)", text: $rdInterestRate).keyboardType(.decimalPad)
                    }

                case .chit:
                    Section(header: Text("Chit Details")) {
                        TextField("Group Name", text: $chitGroupName)
                        TextField("Monthly Contribution", text: $chitMonthlyContribution).keyboardType(.decimalPad)
                        DatePicker("Start Date", selection: $chitStartDate, displayedComponents: .date)
                        DatePicker("Maturity Date", selection: $chitMaturityDate, displayedComponents: .date)
                    }

                case .property:
                    Section(header: Text("Property Details")) {
                        TextField("Type", text: $propertyType)
                        TextField("Location", text: $location)
                        DatePicker("Purchase Date", selection: $purchaseDate, displayedComponents: .date)
                    }

                case .stock:
                    Section(header: Text("Stock Details")) {
                        TextField("Brokerage", text: $brokerage)
                        TextField("Demat Number", text: $dematNumber)
                        TextField("PAN Number", text: $panNumber)
                        DatePicker("Opening Date", selection: $openingDate, displayedComponents: .date)
                    }
                }
            }
            .navigationTitle("Add Account")
            .toolbar {
                ToolbarItem(placement: .cancellationAction) {
                    Button("Cancel") { dismiss() }
                }
                ToolbarItem(placement: .confirmationAction) {
                    Button("Save") {
                        guard let bal = Double(balance) else { return }
                        let id = UUID()
                        let baseRemarks = remarks.isEmpty ? nil : remarks

                        let account: AnyAccount
                        switch selectedType {
                        case .bank:
                            account = .bank(BankAccount(id: id, name: name, balance: bal, remarks: baseRemarks, accountNumber: accountNumber, ifscCode: ifscCode, bankName: bankName, transactions: []))
                        case .loan:
                            account = .loan(LoanAccount(id: id, name: name, balance: bal, remarks: baseRemarks, lenderName: lenderName, interestRate: Double(interestRate) ?? 0, emiAmount: Double(emiAmount) ?? 0, dueDate: dueDate, transactions: []))
                        case .creditCard:
                            account = .creditCard(CreditCardAccount(id: id, name: name, balance: bal, remarks: baseRemarks, provider: cardProvider, cardNumber: cardNumber, limit: Double(cardLimit) ?? 0, dueDate: cardDueDate, billingCycleDay: Int(billingCycleDay) ?? 1, transactions: []))
                        case .borrow:
                            account = .borrow(BorrowAccount(id: id, name: name, balance: bal, remarks: baseRemarks, personName: personName, contactNumber: contactNumber, address: address, interestRate: Double(interestRate) ?? 0, dueDate: dueDate, transactions: []))
                        case .give:
                            account = .give(GiveAccount(id: id, name: name, balance: bal, remarks: baseRemarks, personName: personName, contactNumber: contactNumber, address: address, interestRate: Double(interestRate) ?? 0, dueDate: dueDate, transactions: []))
                        case .fd:
                            account = .fd(FDAccount(id: id, name: name, balance: bal, remarks: baseRemarks, bankName: fdBankName, interestRate: Double(fdInterestRate) ?? 0, startDate: fdStartDate, maturityDate: fdMaturityDate, transactions: []))
                        case .rd:
                            account = .rd(RDAccount(id: id, name: name, balance: bal, remarks: baseRemarks, monthlyAmount: Double(rdMonthlyAmount) ?? 0, startDate: rdStartDate, maturityDate: rdMaturityDate, bankName: rdBankName, interestRate: Double(rdInterestRate) ?? 0, transactions: []))
                        case .chit:
                            account = .chit(ChitAccount(id: id, name: name, balance: bal, remarks: baseRemarks, groupName: chitGroupName, monthlyContribution: Double(chitMonthlyContribution) ?? 0, startDate: chitStartDate, maturityDate: chitMaturityDate, transactions: []))
                        case .property:
                            account = .property(PropertyAccount(id: id, name: name, balance: bal, remarks: baseRemarks, propertyType: propertyType, location: location, purchaseDate: purchaseDate, transactions: []))
                        case .stock:
                            account = .stock(StockAccount(id: id, name: name, balance: bal, remarks: baseRemarks, brokerage: brokerage, dematNumber: dematNumber, panNumber: panNumber, openingDate: openingDate, transactions: []))
                        }

                        onSave(account)
                        dismiss()
                    }
                    .disabled(name.isEmpty || balance.isEmpty)
                }
            }
        }
    }
}
