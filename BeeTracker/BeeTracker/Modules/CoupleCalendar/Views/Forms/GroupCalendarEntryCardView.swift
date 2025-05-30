import SwiftUI

struct GroupCalendarEntryCardView: View {
    @ObservedObject var viewModel: CalendarEntriesViewModel
    @EnvironmentObject private var calendarViewModel: CalendarViewModel
    let totalWidth: CGFloat

    var body: some View {
        let start = calendarViewModel.minutesSinceMidnight(viewModel._start_time)
        let end = calendarViewModel.minutesSinceMidnight(viewModel._end_time ?? viewModel._start_time)
        let height = CGFloat(end - start)
        let yOffset = CGFloat(start)

        VStack(alignment: .leading, spacing: 4) {
            HStack {
                Text("Shared")
                    .font(.caption2)
                    .padding(4)
                    .background(Color.purple.opacity(0.2))
                    .cornerRadius(4)
                Spacer()
            }

            Text("\(viewModel._start_time?.formatted(date: .omitted, time: .shortened) ?? "") – \(viewModel._end_time?.formatted(date: .omitted, time: .shortened) ?? "")")
                .font(.caption2)
                .foregroundColor(.secondary)

            Text(viewModel._title ?? "")
                .font(.caption)
                .bold()
                .lineLimit(1)

            if let notes = viewModel._notes {
                Text(notes)
                    .font(.caption2)
                    .lineLimit(1)
                    .foregroundColor(.gray)
            }
        }
        .padding(6)
        .frame(width: totalWidth, height: height)
        .background(Color.purple.opacity(0.25))
        .cornerRadius(8)
        .overlay(
            RoundedRectangle(cornerRadius: 8)
                .stroke(Color.primary.opacity(0.05), lineWidth: 1)
        )
        .position(x: totalWidth / 2 + 58, y: yOffset + height / 2) // 58 is the gutter offset
    }
}