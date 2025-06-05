import SwiftUI

struct PeriodSummaryView: View {
    @EnvironmentObject private var periodViewModel: PeriodViewModel
    @State private var showingAddForm = false
    @State private var selectedEntry: PeriodEntriesViewModel? = nil

    var body: some View {
        VStack(alignment: .leading, spacing: 12) {
            HStack {
                Text("🌸 Period Tracker")
                    .font(.title2)
                    .fontWeight(.semibold)
                Spacer()
                Button {
                    showingAddForm = true
                } label: {
                    Image(systemName: "plus.circle.fill")
                        .font(.title2)
                        .foregroundColor(.pink)
                }
            }
            .padding(.horizontal)
            .padding(.top)

            if let ongoing = periodViewModel.getOngoingPeriodViewModel() {
                OngoingPeriodCardView(
                    viewModel: ongoing,
                    currentDay: ongoing.currentDayIndex(),
                    remainingDays: ongoing.daysRemaining()
                )
            }
            else if let nextDate = periodViewModel.getNextExpectedPeriodDate() {
                NextExpectedCardView(nextDate: nextDate) {
                    let today = Calendar.current.startOfDay(for: Date())
                    periodViewModel.startNextExpectedPeriod(today)
                }
            }

            Text("History")
                .font(.headline)
                .padding(.horizontal)

            ScrollView {
                LazyVStack(spacing: 12) {
                    ForEach(periodViewModel.historyEntriesSorted(), id: \.id) { entry in
                        PeriodHistoryCardView(
                            viewModel: entry,
                            onTap: {
                                selectedEntry = entry
                            },
                            onDelete: {
                                periodViewModel.deletePeriodEntry(entry: entry)
                            }
                        )
                    }
                }
                .padding(.horizontal)
            }
        }
        .sheet(isPresented: $showingAddForm) {
            AddPeriodForm()
        }
        .sheet(item: $selectedEntry) { entry in
            EditPeriodForm(entryViewModel: entry)
        }
    }
}
