import SwiftUI

struct OngoingPeriodCardView: View {
    let entry: PeriodEntry
    let currentDay: Int
    let remainingDays: Int
    let onEndPeriod: () -> Void
    let onDelete: () -> Void
    let viewModel: PeriodTrackerViewModel

    @State private var showMoodOptions = false
    
    private let moodOptions = ["😊", "😢", "😡", "😴", "🥰", "😕", "🤒"]

    var body: some View {
        ZStack {
            VStack(alignment: .leading, spacing: 12) {
                HStack {
                    Text("🌸 Ongoing Period")
                        .font(.headline)
                    Spacer()
                    Text(Helper.shared.formatDateRange(entry.startDate, entry.endDate))
                        .font(.caption)
                        .foregroundColor(.secondary)
                }

                HStack {
                    Text("Day \(currentDay) of \(entry.totalDays)")
                    Spacer()
                    Text("\(remainingDays) days left")
                }
                .font(.subheadline)

                Divider()

                HStack(spacing: 12) {
                    Button {
                        withAnimation {
                            showMoodOptions.toggle()
                        }
                    } label: {
                        Label("Mood", systemImage: "heart")
                            .frame(maxWidth: .infinity)
                    }

                    Button {
                        onEndPeriod()
                    } label: {
                        Label("End", systemImage: "xmark.circle")
                            .frame(maxWidth: .infinity)
                    }

                    Button(role: .destructive) {
                        onDelete()
                    } label: {
                        Label("Delete", systemImage: "trash")
                            .frame(maxWidth: .infinity)
                    }
                }
                .font(.caption)
                .buttonStyle(.bordered)
            }
            .padding()
            .frame(maxWidth: .infinity)
            .background(Color.pink.opacity(0.1))
            .cornerRadius(12)
            .padding(.horizontal)

            if showMoodOptions {
                VStack {
                    Spacer().frame(height: 10)

                    HStack {
                        Spacer()
                        ScrollView(.horizontal, showsIndicators: false) {
                            HStack(spacing: 10) {
                                ForEach(moodOptions, id: \.self) { emoji in
                                    Text(emoji)
                                        .font(.title)
                                        .padding(8)
                                        .background(Color.white)
                                        .clipShape(Circle())
                                        .shadow(radius: 2)
                                        .onTapGesture {
                                            showMoodOptions = false
                                            viewModel.sendMood(emoji)
                                        }
                                }
                            }
                            .padding(.horizontal, 12)
                        }
                        .padding(8)
                        .background(.ultraThinMaterial)
                        .cornerRadius(16)
                        .shadow(radius: 4)
                        Spacer()
                    }
                }
                .transition(.move(edge: .top))
                .zIndex(1)
            }
        }
    }
}
