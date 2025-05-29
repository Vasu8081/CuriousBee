import SwiftUI

struct TaskScheduleForm: View {
    @Environment(\.dismiss) var dismiss
    var task: Task
    var onSchedule: (CalendarEntry) -> Void

    @State private var date: Date = Date()
    @State private var startTime: Date = Date()
    @State private var endTime: Date = Date().addingTimeInterval(3600)

    var body: some View {
        NavigationStack {
            Form {
                Section(header: Text("Schedule for \(task.title)")) {
                    DatePicker("Date", selection: $date, displayedComponents: .date)
                    DatePicker("Start Time", selection: $startTime, displayedComponents: .hourAndMinute)
                    DatePicker("End Time", selection: $endTime, displayedComponents: .hourAndMinute)
                }
            }
            .navigationTitle("Schedule Task")
            .toolbar {
                ToolbarItem(placement: .confirmationAction) {
                    Button("Add") {
                        let calendar = Calendar.current
                        let fullStart = calendar.date(bySettingHour: calendar.component(.hour, from: startTime),
                                                      minute: calendar.component(.minute, from: startTime),
                                                      second: 0,
                                                      of: date) ?? date

                        let fullEnd = calendar.date(bySettingHour: calendar.component(.hour, from: endTime),
                                                    minute: calendar.component(.minute, from: endTime),
                                                    second: 0,
                                                    of: date) ?? date

                        let assignedTo: TaskAssignment = {
                            switch task.interactionStyle {
                            case .soloNoDisturbance, .soloWithCall:
                                if let primary = task.primaryDoer {
                                    return primary == .me ? .me : .partner
                                }
                                return .me
                            case .canDoAloneOrTogether, .bothConscious, .oneWithSupport, .letPartnerDecide:
                                return .both
                            }
                        }()

                        let entry = CalendarEntry(
                            title: task.title,
                            notes: task.notes,
                            date: date,
                            startTime: fullStart,
                            endTime: fullEnd,
                            entryType: .task,
                            assignedTo: assignedTo,
                            linkedTaskId: task.id
                        )

                        onSchedule(entry)
                        dismiss()
                    }
                }

                ToolbarItem(placement: .cancellationAction) {
                    Button("Cancel") {
                        dismiss()
                    }
                }
            }
        }
    }
}
