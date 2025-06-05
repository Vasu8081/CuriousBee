import SwiftUI

struct NextExpectedCardView: View {
    let nextDate: Date
    let onStartTap: () -> Void

    var body: some View {
        HStack {
            Image(systemName: "calendar")
                .foregroundColor(.pink)
            Text("Next expected: \(Helper.shared.formattedDate(nextDate))")
                .font(.body)
                .fontWeight(.medium)
            Spacer()
            Button("Start") {
                onStartTap()
            }
            .padding(.horizontal, 12)
            .padding(.vertical, 6)
            .background(Color.pink.opacity(0.2))
            .cornerRadius(8)
        }
        .padding()
        .background(Color.pink.opacity(0.1))
        .cornerRadius(12)
        .padding(.horizontal)
    }
}
