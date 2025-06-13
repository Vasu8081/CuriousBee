import SwiftUI

struct AccountListView: View {
    @EnvironmentObject var viewModel: UserViewModel
    @State private var showCreateForm = false

    var body: some View {
        NavigationView{
            VStack {
                // Placeholder list or message
                Text("No accounts to show yet.")
                    .foregroundColor(.secondary)
                    .padding()
                
                Spacer()
                
                Button(action: {
                    showCreateForm = true
                }) {
                    Label("Add Account", systemImage: "plus")
                        .padding()
                        .background(Color.blue)
                        .foregroundColor(.white)
                        .cornerRadius(8)
                }
                .sheet(isPresented: $showCreateForm) {
                    BankAccountFormView()
                }
            }
            .navigationTitle("Accounts")
        }
    }
}
