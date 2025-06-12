import Foundation

class UserViewModel: ObservableObject {
    @Published var userData: UserData = .empty
    @Published var needSync: Bool = false
    
    func populateMockData() {
        let mockCategories: [Category] = [
            Category(id: UUID(), name: "Salary", type: .income, emoji: "💼"),
            Category(id: UUID(), name: "Groceries", type: .expense, emoji: "🛒"),
            Category(id: UUID(), name: "Utilities", type: .expense, emoji: "💡"),
            Category(id: UUID(), name: "Food", type: .expense, emoji: "🍔"),
            Category(id: UUID(), name: "Shopping", type: .expense, emoji: "🛍️"),
            Category(id: UUID(), name: "Loan", type: .expense, emoji: "🏦"),
            Category(id: UUID(), name: "Savings", type: .income, emoji: "💰"),
            Category(id: UUID(), name: "Investment", type: .investment, emoji: "📈"),
            Category(id: UUID(), name: "Lending", type: .expense, emoji: "🤝"),
            Category(id: UUID(), name: "Transfer", type: .transfer, emoji: "🔁")
        ]

        userData.categories = mockCategories

        func category(named name: String) -> Category {
            return mockCategories.first(where: { $0.name == name })!
        }

        let iciciID = UUID()
        let loanID = UUID()
        let rdID = UUID()
        let creditCardID = UUID()
        let chitID = UUID()
        let lendID = UUID()

        userData.accounts = [
            .bank(BankAccount(
                id: iciciID,
                name: "ICICI Savings",
                balance: 102340.75,
                remarks: "Salary and daily expenses",
                accountNumber: "1234567890",
                ifscCode: "ICIC0001234",
                bankName: "ICICI Bank",
                transactions: [
                    Transaction(
                        id: UUID(),
                        date: Date().addingTimeInterval(-86400 * 2),
                        amount: 50000,
                        note: "Salary",
                        category: category(named: "Salary"),
                        fromAccountID: nil,
                        toAccountID: iciciID
                    ),
                    Transaction(
                        id: UUID(),
                        date: Date().addingTimeInterval(-86400),
                        amount: 2300,
                        note: "Groceries",
                        category: category(named: "Groceries"),
                        fromAccountID: iciciID,
                        toAccountID: nil
                    ),
                    Transaction(
                        id: UUID(),
                        date: Date(),
                        amount: 1200,
                        note: "Phone Bill",
                        category: category(named: "Utilities"),
                        fromAccountID: iciciID,
                        toAccountID: nil
                    )
                ]
            )),
            .loan(LoanAccount(
                id: loanID,
                name: "Home Loan",
                balance: 1172500.00,
                remarks: "20-year housing loan",
                lenderName: "SBI",
                interestRate: 7.25,
                emiAmount: 18750,
                dueDate: Calendar.current.date(byAdding: .day, value: 20, to: Date())!,
                transactions: [
                    Transaction(
                        id: UUID(),
                        date: Date().addingTimeInterval(-86400 * 10),
                        amount: 18750,
                        note: "EMI - April",
                        category: category(named: "Loan"),
                        fromAccountID: loanID,
                        toAccountID: nil
                    ),
                    Transaction(
                        id: UUID(),
                        date: Date().addingTimeInterval(-86400 * 40),
                        amount: 18750,
                        note: "EMI - March",
                        category: category(named: "Loan"),
                        fromAccountID: loanID,
                        toAccountID: nil
                    )
                ]
            )),
            .bank(BankAccount(
                id: rdID,
                name: "HDFC RD",
                balance: 15000,
                remarks: "Recurring deposit for vacation",
                accountNumber: "8765432101",
                ifscCode: "HDFC0005678",
                bankName: "HDFC Bank",
                transactions: [
                    Transaction(
                        id: UUID(),
                        date: Date().addingTimeInterval(-86400 * 30),
                        amount: 5000,
                        note: "RD Installment",
                        category: category(named: "Savings"),
                        fromAccountID: nil,
                        toAccountID: rdID
                    )
                ]
            )),
            .loan(LoanAccount(
                id: creditCardID,
                name: "Credit Card - Axis",
                balance: 33000,
                remarks: "Monthly usage credit card",
                lenderName: "Axis Bank",
                interestRate: 0,
                emiAmount: 0,
                dueDate: Calendar.current.date(byAdding: .day, value: 10, to: Date())!,
                transactions: [
                    Transaction(
                        id: UUID(),
                        date: Date().addingTimeInterval(-86400 * 5),
                        amount: 1200,
                        note: "Zomato",
                        category: category(named: "Food"),
                        fromAccountID: creditCardID,
                        toAccountID: nil
                    ),
                    Transaction(
                        id: UUID(),
                        date: Date().addingTimeInterval(-86400 * 3),
                        amount: 1800,
                        note: "Amazon Shopping",
                        category: category(named: "Shopping"),
                        fromAccountID: creditCardID,
                        toAccountID: nil
                    )
                ]
            )),
            .bank(BankAccount(
                id: chitID,
                name: "Chit Fund",
                balance: 24000,
                remarks: "Monthly contribution for chit group",
                accountNumber: "000011112222",
                ifscCode: "CHIT000001",
                bankName: "Local Group",
                transactions: [
                    Transaction(
                        id: UUID(),
                        date: Date().addingTimeInterval(-86400 * 15),
                        amount: 2000,
                        note: "Chit Contribution",
                        category: category(named: "Investment"),
                        fromAccountID: chitID,
                        toAccountID: nil
                    )
                ]
            )),
            .loan(LoanAccount(
                id: lendID,
                name: "Lend to Ramesh",
                balance: 8000,
                remarks: "Borrowed cash to friend",
                lenderName: "Ramesh",
                interestRate: 0,
                emiAmount: 0,
                dueDate: Calendar.current.date(byAdding: .day, value: 30, to: Date())!,
                transactions: [
                    Transaction(
                        id: UUID(),
                        date: Date().addingTimeInterval(-86400 * 20),
                        amount: 8000,
                        note: "Lent to Ramesh",
                        category: category(named: "Lending"),
                        fromAccountID: lendID,
                        toAccountID: nil
                    )
                ]
            ))
        ]

        needSync = true
    }
}
