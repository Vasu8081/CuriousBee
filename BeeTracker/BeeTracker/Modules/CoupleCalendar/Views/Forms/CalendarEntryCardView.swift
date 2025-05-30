struct CalendarEntryCardView: View {
    @ObservedObject var viewModel: CalendarEntriesViewModel

    var body: some View {
        let start = CalendarViewModel.shared.minutesSinceMidnight(viewModel._start_time)
        let end = CalendarViewModel.shared.minutesSinceMidnight(viewModel._end_time ?? viewModel._start_time)
        let height = CGFloat(end - start)
        let yOffset = CGFloat(start)

        VStack(alignment: .leading, spacing: 2) {
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
        .frame(width: 160, height: height)
        .background(Color.blue.opacity(0.2))
        .cornerRadius(8)
        .overlay(
            RoundedRectangle(cornerRadius: 8)
                .stroke(Color.primary.opacity(0.05), lineWidth: 1)
        )
        .position(x: 160 / 2, y: yOffset + height / 2)
    }
}