import SwiftUI

struct NotificationBellView: View {
    @ObservedObject var viewModel: NotificationTrackerViewModel
    var onTap: () -> Void

    var body: some View {
        ZStack(alignment: .topTrailing) {
            Button(action: onTap) {
                Image(systemName: "bell")
            }

            if !viewModel.unreadMessages.isEmpty {
                Circle()
                    .fill(Color.red)
                    .frame(width: 10, height: 10)
                    .offset(x: 6, y: -6)
            }
        }
    }
}
