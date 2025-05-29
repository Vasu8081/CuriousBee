import SwiftUI

struct PeriodSummaryView: View {
    @ObservedObject var viewModel: PeriodTrackerViewModel
    @State private var showingAddForm = false
    @State private var selectedEntry: PeriodEntry? = nil
    @State private var selectedMoodDate: IdentifiableDate? = nil

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

            if let ongoing = viewModel.currentOngoingPeriod() {
                OngoingPeriodCardView(
                    entry: ongoing,
                    currentDay: viewModel.currentDayIndex(in: ongoing),
                    remainingDays: viewModel.daysRemaining(in: ongoing),
                    onEndPeriod: {
                        viewModel.endCurrentPeriod()
                    },
                    onDelete: {
                        viewModel.deleteCurrentPeriod()
                    },
                    viewModel: viewModel
                )
            } else if let next = viewModel.nextExpectedDate() {
                NextExpectedCardView(nextDate: next) {
                    let today = Calendar.current.startOfDay(for: Date())
                    viewModel.addPeriod(start: today, duration: viewModel.tracker.averagePeriodDuration)
                }
            }

            Text("History")
                .font(.headline)
                .padding(.horizontal)

            ScrollView {
                VStack(spacing: 12) {
                    ForEach(viewModel.tracker.history
                        .filter { $0.isEnded || Calendar.current.startOfDay(for: $0.endDate) < Calendar.current.startOfDay(for: Date()) }
                        .sorted(by: { $0.startDate > $1.startDate })) { entry in

                        PeriodHistoryCardView(
                            entry: entry,
                            onTap: {
                                selectedEntry = entry
                            },
                            onDelete: {
                                viewModel.deletePeriod(entry)
                            }
                        )
                    }
                }
                .padding()
            }

        }
        .sheet(isPresented: $showingAddForm) {
            AddPeriodForm(tracker: $viewModel.tracker)
        }
        .sheet(item: $selectedEntry) { entry in
            if let index = viewModel.tracker.history.firstIndex(where: { $0.id == entry.id }) {
                EditPeriodForm(entry: $viewModel.tracker.history[index])
            }
        }
    }
}
