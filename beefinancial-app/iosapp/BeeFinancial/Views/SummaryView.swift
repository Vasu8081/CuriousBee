import SwiftUI

struct SummaryView: View {
    @ObservedObject var userViewModel: UserViewModel

    var body: some View {
        NavigationView {
            List {
                ForEach(AccountCategory.allCases, id: \.self) { category in
                    let accounts = userViewModel.userData.accounts.filter {
                        AccountCategory.map[$0.type] == category
                    }
                    let total = accounts.map { $0.balance }.reduce(0, +)

                    if !accounts.isEmpty {
                        Section(header: Text(category.displayName)) {
                            HStack {
                                Text("Total")
                                Spacer()
                                Text(String(format: "₹%.2f", total))
                                    .bold()
                            }

                            ForEach(accounts) { account in
                                HStack {
                                    Text(account.name)
                                    Spacer()
                                    Text(String(format: "₹%.2f", account.balance))
                                }
                            }
                        }
                    }
                }
            }
            .navigationTitle("Summary")
        }
    }
}
