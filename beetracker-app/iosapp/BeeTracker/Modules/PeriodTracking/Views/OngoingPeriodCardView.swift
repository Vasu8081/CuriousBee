import SwiftUI

struct OngoingPeriodCardView: View {
    @EnvironmentObject var periodViewModel: PeriodViewModel
    @ObservedObject var viewModel: PeriodEntriesViewModel
    let currentDay: Int
    let remainingDays: Int

    @State private var editPeriodForm = false

    var body: some View {
        VStack(alignment: .leading, spacing: 12) {
            HStack {
                Text("🌸 Ongoing Period")
                    .font(.headline)
                Spacer()
                Text(Helper.shared.formatDateRange(viewModel.start_date ?? Date(), viewModel.end_date ?? Date()))
                    .font(.caption)
                    .foregroundColor(.secondary)
            }

            HStack {
                Text("Day \(currentDay) of \(viewModel.totalDays)")
            }
            .font(.subheadline)

            Divider()

            HStack(spacing: 12) {
                Button {
                    withAnimation { editPeriodForm.toggle() }
                } label: {
                    Label("Edit", systemImage: "pencil")
                        .frame(maxWidth: .infinity)
                }

                Button {
                    periodViewModel.endOngoingPeriod()
                } label: {
                    Label("End", systemImage: "xmark.circle")
                        .frame(maxWidth: .infinity)
                }

                Button(role: .destructive) {
                    periodViewModel.deleteOngoingPeriod()
                } label: {
                    Label("Delete", systemImage: "trash")
                        .frame(maxWidth: .infinity)
                }
            }
            .buttonStyle(.bordered)
            .font(.caption)
        }
        .padding()
        .background(Color.pink.opacity(0.1))
        .cornerRadius(12)
        .padding(.horizontal)
        .sheet(isPresented: $editPeriodForm) {
            EditPeriodForm(entryViewModel: viewModel)
        }
    }
}
