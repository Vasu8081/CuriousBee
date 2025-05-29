import SwiftUI

struct NotificationListView: View {
    @ObservedObject var viewModel: NotificationTrackerViewModel
    @Environment(\.dismiss) var dismiss

    var body: some View {
        NavigationView {
            List {
                ForEach(viewModel.receivedMessages.reversed(), id: \.self) { message in
                    Text(message)
                }
            }
            .navigationTitle("Notifications")
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button("Clear") {
                        viewModel.clearAll()
                        dismiss()
                    }
                }
            }
        }
    }
}
