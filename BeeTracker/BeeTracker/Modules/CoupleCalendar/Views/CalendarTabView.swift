import SwiftUI

struct CalendarTabView: View {
    @ObservedObject private var userViewModel = UserViewModel.shared
    @State private var selectedDate = Date()
    @State private var showAddEntry = false
    @State private var selectedEntry: CalendarEntry? = nil
    @State private var showDatePicker = false

    let hourHeight: CGFloat = 60
    let columnWidth: CGFloat = 160

    var body: some View {
        VStack(spacing: 0) {
            HStack(spacing: 0) {
                Spacer().frame(width: 58)
                Text("Mine")
                    .font(.subheadline.bold())
                    .frame(width: columnWidth, alignment: .center)
                Text("Partner")
                    .font(.subheadline.bold())
                    .frame(width: columnWidth, alignment: .center)
            }
            .padding(.vertical, 6)
            .background(Color(.systemBackground))
            .zIndex(1)

            ScrollViewReader { proxy in
                ScrollView(showsIndicators: true) {
                    HStack(alignment: .top, spacing: 0) {
                        timeGutter
                        ZStack(alignment: .topLeading) {
                            timelineGrid
                            entryColumn(for: .me, xOffset: 0)
                            entryColumn(for: .partner, xOffset: columnWidth + 12)
                            currentTimeLine
                        }
                        .frame(height: hourHeight * 24)
                    }
                    .padding(.top, 8)
                    .padding(.leading, 8)
                    .id("timeline")
                }
                .onAppear {
                    scrollToHour(8, proxy: proxy)
                }
            }
        }
        .navigationTitle("Calendar")
        .overlay(alignment: .bottomTrailing) {
            HStack(spacing: 12) {
                Button(action: { showDatePicker = true }) {
                    Image(systemName: "calendar")
                        .font(.title2)
                        .foregroundColor(.white)
                        .padding()
                        .background(Color.secondary)
                        .clipShape(Circle())
                        .shadow(radius: 4)
                }

                Button(action: { showAddEntry = true }) {
                    Image(systemName: "plus")
                        .font(.title2)
                        .foregroundColor(.white)
                        .padding()
                        .background(Color.accentColor)
                        .clipShape(Circle())
                        .shadow(radius: 4)
                }
            }
            .padding()
        }
        .sheet(isPresented: $showAddEntry) {
            CalendarEntryForm(
                selectedDate: selectedDate
            ) { newEntry in
                UserViewModel.shared.addCalendarEntry(newEntry)
            }
        }
        .sheet(item: $selectedEntry) { entry in
            CalendarEditForm(
                entryToEdit: entry,
                onSave: { updated in
                    UserViewModel.shared.updateCalendarEntry(updated)
                },
                onDelete: {
                    UserViewModel.shared.deleteCalendarEntry(id: entry.id)
                }
            )
        }
        .sheet(isPresented: $showDatePicker) {
            VStack(spacing: 16) {
                DatePicker("Select a Date", selection: $selectedDate, displayedComponents: .date)
                    .datePickerStyle(.graphical)
                    .padding()
                Button("Done") {
                    showDatePicker = false
                }
                .padding()
            }
        }
    }

    private var entries: [CalendarEntry] {
        userViewModel.calendarEntries
            .filter { Calendar.current.isDate($0.date, inSameDayAs: selectedDate) }
            .sorted(by: { $0.startTime < $1.startTime })
    }

    private var timeGutter: some View {
        VStack(spacing: 0) {
            ForEach(0..<24) { hour in
                Text(hourLabel(for: hour))
                    .font(.caption2)
                    .foregroundColor(.gray)
                    .frame(height: hourHeight, alignment: .topTrailing)
                    .frame(width: 50, alignment: .trailing)
            }
        }
    }

    private func hourLabel(for hour: Int) -> String {
        let formatter = DateFormatter()
        formatter.dateFormat = "h a"
        let date = Calendar.current.date(bySettingHour: hour, minute: 0, second: 0, of: Date()) ?? Date()
        return formatter.string(from: date)
    }

    private var timelineGrid: some View {
        ForEach(0..<24) { hour in
            Rectangle()
                .fill(Color.gray.opacity(0.2))
                .frame(height: 1)
                .offset(y: CGFloat(hour) * hourHeight)
                .padding(.leading, 4)
        }
    }

    private func entryColumn(for assignment: TaskAssignment, xOffset: CGFloat) -> some View {
        ZStack(alignment: .topLeading) {
            ForEach(entries.filter { $0.assignedTo == assignment || $0.assignedTo == .both }) { entry in
                let start = minutesSinceMidnight(entry.startTime)
                let end = minutesSinceMidnight(entry.endTime ?? entry.startTime)
                let height = CGFloat(end - start)
                let yOffset = CGFloat(start)

                VStack(alignment: .leading, spacing: 2) {
                    Text("\(entry.startTime.formatted(date: .omitted, time: .shortened)) – \(entry.endTime?.formatted(date: .omitted, time: .shortened) ?? "")")
                        .font(.caption2)
                        .foregroundColor(.secondary)
                    Text(entry.title)
                        .font(.caption)
                        .bold()
                        .lineLimit(1)
                    if let notes = entry.notes {
                        Text(notes)
                            .font(.caption2)
                            .lineLimit(1)
                            .foregroundColor(.gray)
                    }
                }
                .padding(6)
                .frame(width: columnWidth, height: height)
                .background(cardColor(for: entry.entryType))
                .cornerRadius(8)
                .overlay(
                    RoundedRectangle(cornerRadius: 8)
                        .stroke(Color.primary.opacity(0.05), lineWidth: 1)
                )
                .position(x: xOffset + columnWidth / 2, y: yOffset + height / 2)
                .onTapGesture {
                    selectedEntry = entry
                }
            }
        }
    }

    private var currentTimeLine: some View {
        let now = Date()
        guard Calendar.current.isDate(now, inSameDayAs: selectedDate) else { return AnyView(EmptyView()) }

        let y = CGFloat(minutesSinceMidnight(now))
        return AnyView(
            Rectangle()
                .fill(Color.red)
                .frame(height: 1.5)
                .offset(x: 0, y: y)
        )
    }

    private func minutesSinceMidnight(_ date: Date) -> Int {
        let components = Calendar.current.dateComponents([.hour, .minute], from: date)
        return (components.hour ?? 0) * 60 + (components.minute ?? 0)
    }

    private func scrollToHour(_ hour: Int, proxy: ScrollViewProxy) {
        let yOffset = CGFloat(hour) * hourHeight - 20
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            withAnimation {
                proxy.scrollTo("timeline", anchor: .top)
            }
        }
    }

    private func cardColor(for type: CalendarEntryType) -> Color {
        switch type {
        case .task: return Color.blue.opacity(0.2)
        case .event: return Color.green.opacity(0.2)
        case .period: return Color.pink.opacity(0.2)
        case .mood: return Color.orange.opacity(0.2)
        }
    }
}
