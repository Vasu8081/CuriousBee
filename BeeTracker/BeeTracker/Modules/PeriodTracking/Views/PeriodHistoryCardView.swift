import SwiftUI

struct PeriodHistoryCardView: View {
    let entry: PeriodEntry
    var onTap: () -> Void
    var onDelete: () -> Void

    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            HStack {
                Text("🥀 \(Helper.shared.formattedDate(entry.startDate)) → \(Helper.shared.formattedDate(entry.endDate))")
                    .font(.subheadline)
                    .fontWeight(.medium)
                Spacer()
                Button(action: onDelete) {
                    Image(systemName: "trash")
                        .foregroundColor(.red)
                }
                .buttonStyle(.plain)
            }

            if !entry.symptoms.isEmpty {
                WrapView(tags: entry.symptoms.map { $0.displayName }) { tag in
                    Text(tag)
                        .font(.caption)
                        .padding(.horizontal, 8)
                        .padding(.vertical, 4)
                        .background(Color.pink.opacity(0.15))
                        .cornerRadius(8)
                }
            } else {
                Spacer().frame(height: 12) // Maintain space if no symptoms
            }

            if !entry.notes.isEmpty {
                Text("Note: \(entry.notes)")
                    .font(.footnote)
                    .italic()
                    .foregroundColor(.gray)
            } else {
                Spacer().frame(height: 12) // Maintain space if no notes
            }
        }
        .padding()
        .frame(maxWidth: .infinity, minHeight: 120) // 🔧 Set minimum height
        .background(
            RoundedRectangle(cornerRadius: 12)
                .fill(Color.white)
                .shadow(color: Color.black.opacity(0.07), radius: 6, x: 0, y: 3)
                .overlay(
                    RoundedRectangle(cornerRadius: 12)
                        .stroke(Color.pink.opacity(0.1), lineWidth: 1)
                )
        )
        .onTapGesture { onTap() }
    }
}
