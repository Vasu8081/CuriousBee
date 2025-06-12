import SwiftUI

struct GroupCalendarEntryCardView: View {
    @ObservedObject var viewModel: CalendarEntriesViewModel
    @EnvironmentObject private var calendarViewModel: CalendarViewModel
    let totalWidth: CGFloat
    let xOffset: CGFloat

    var body: some View {
        let start = calendarViewModel.minutesSinceMidnight(viewModel.start_time)
        let end = calendarViewModel.minutesSinceMidnight(viewModel.end_time ?? viewModel.start_time)
        let height = CGFloat(end - start)
        let yOffset = CGFloat(start)

        VStack(alignment: .leading, spacing: 4) {
            Text("\(viewModel.start_time?.formatted(date: .omitted, time: .shortened) ?? "") – \(viewModel.end_time?.formatted(date: .omitted, time: .shortened) ?? "")")
                .font(.caption2)
                .foregroundColor(.secondary)

            Text(viewModel.title ?? "")
                .font(.caption)
                .bold()
                .lineLimit(1)

            if let notes = viewModel.notes {
                Text(notes)
                    .font(.caption2)
                    .lineLimit(1)
                    .foregroundColor(.gray)
            }
        }
        .padding(6)
        .frame(width: totalWidth+12, height: height)
        .background(Color.purple.opacity(0.25))
        .cornerRadius(8)
        .overlay(
            RoundedRectangle(cornerRadius: 8)
                .stroke(Color.primary.opacity(0.05), lineWidth: 1)
        )
        .position(x: xOffset + totalWidth / 2, y: yOffset + height / 2)
    }
}
