import SwiftUI
import Foundation

struct PeriodHistoryCardView: View {
    @EnvironmentObject private var periodViewModel: PeriodViewModel
    @ObservedObject var viewModel: PeriodEntriesViewModel
    @State private var tags: [String] = []

    var onTap: () -> Void
    var onDelete: () -> Void

    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            HStack {
                Text("🥀 \(Helper.shared.formattedDate(viewModel.start_date ?? Date())) → \(Helper.shared.formattedDate(viewModel.end_date ?? Date()))")
                    .font(.subheadline)
                    .fontWeight(.medium)
                Spacer()
                Button(action: onDelete) {
                    Image(systemName: "trash")
                        .foregroundColor(.red)
                }
                .buttonStyle(.plain)
            }

            if !tags.isEmpty {
                WrapView(tags: tags) { tag in
                    Text(tag)
                        .font(.caption)
                        .padding(.horizontal, 8)
                        .padding(.vertical, 4)
                        .background(Color.pink.opacity(0.15))
                        .cornerRadius(8)
                }
            } else {
                Spacer().frame(height: 12)
            }

            if let notes = viewModel.notes, !notes.isEmpty {
                Text("Note: \(notes)")
                    .font(.footnote)
                    .italic()
                    .foregroundColor(.gray)
            } else {
                Spacer().frame(height: 12)
            }
        }
        .padding()
        .frame(maxWidth: .infinity, minHeight: 120)
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
        .onAppear {
            tags = updateTags()
        }
        .onChange(of: viewModel.period_symptoms) {
            tags = updateTags()
        }
    }

    private func updateTags() -> [String] {
        return viewModel.period_symptoms
            .compactMap { periodViewModel.getSymptomName(for: $0.symptom_id) }
    }
}
