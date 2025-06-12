import SwiftUI

struct CalendarTabView: View {
    @EnvironmentObject private var calendarViewModel: CalendarViewModel
    @EnvironmentObject private var userViewModel: UserViewModel
    @State private var selectedDate = Date()
    @State private var showAddEntry = false
    @State private var selectedEntry: CalendarEntriesViewModel? = nil
    @State private var showDatePicker = false

    let hourHeight: CGFloat = 60
    let columnWidth: CGFloat = 160

    var body: some View {
        VStack(spacing: 0) {
            HStack(spacing: 0) {
                Spacer().frame(width: 58)
                ForEach(userViewModel.getUserIds(), id: \.self) { id in
                    Text(userViewModel.getUserName(for: id))
                        .font(.subheadline.bold())
                        .frame(width: columnWidth, alignment: .center)
                }
            }
            .padding(.vertical, 6)
            .background(Color(.systemBackground))
            .zIndex(1)

            ScrollViewReader { proxy in
                ScrollView(showsIndicators: true) {
                    HStack(alignment: .top, spacing: 0) {
                        timeGutter
                        ZStack(alignment: .topLeading) {
                            // Force layout even when no entries
                            Rectangle()
                                .fill(Color.clear)
                                .frame(height: hourHeight * 24)

                            timelineGrid

                            ForEach(userViewModel.getUserIds().indices, id: \.self) { index in
                                let id = userViewModel.getUserIds()[index]
                                entryColumn(for: id, xOffset: CGFloat(index) * (columnWidth + 12))
                            }

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
            CalendarEntryForm(selectedDate: selectedDate)
        }
        .sheet(item: $selectedEntry) { entry in
            CalendarEditForm(entryToEdit: entry)
        }
        .sheet(isPresented: $showDatePicker) {
            VStack(spacing: 16) {
                DatePicker("Select a Date", selection: $selectedDate, displayedComponents: .date)
                    .datePickerStyle(.graphical)
                    .padding()
                Button("Done") { showDatePicker = false }
                    .padding()
            }
        }
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
        ZStack(alignment: .topLeading) {
            ForEach(0..<24) { hour in
                Rectangle()
                    .fill(Color.gray.opacity(0.2))
                    .frame(height: 1)
                    .offset(y: CGFloat(hour) * hourHeight)
            }
        }
    }

    private func entryColumn(for userId: UUID, xOffset: CGFloat) -> some View {
        ZStack(alignment: .topLeading) {
            // Show user-specific entries
            ForEach(calendarViewModel.getEntries(for: userId, on: selectedDate)) { entry in
                if entry.group_id == nil {
                    CalendarEntryCardView(
                        viewModel: entry,
                        columnWidth: columnWidth,
                        xOffset: xOffset
                    )
                    .onTapGesture {
                        selectedEntry = entry
                    }
                }
            }

            // Show shared entries only once (on the first user column)
            if userViewModel.getUserIds().first == userId {
                let sharedXOffset = CGFloat(0) * (columnWidth + 12)
                ForEach(calendarViewModel.getSharedEntries(on: selectedDate)) { entry in
                    GroupCalendarEntryCardView(
                        viewModel: entry,
                        totalWidth: 2 * (columnWidth), // Adjust as needed
                        xOffset: sharedXOffset
                    )
                    .onTapGesture {
                        selectedEntry = entry
                    }
                }
            }
        }
    }

    private var currentTimeLine: some View {
        let now = Date()
        guard Calendar.current.isDate(now, inSameDayAs: selectedDate) else {
            return AnyView(EmptyView())
        }

        let minutes = calendarViewModel.minutesSinceMidnight(now)
        let y = CGFloat(minutes) / 60 * hourHeight

        return AnyView(
            Rectangle()
                .fill(Color.red)
                .frame(height: 1.5)
                .offset(x: 0, y: y)
        )
    }

    private func scrollToHour(_ hour: Int, proxy: ScrollViewProxy) {
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            withAnimation {
                proxy.scrollTo("timeline", anchor: .top)
            }
        }
    }
}
