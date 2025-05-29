import SwiftUI

struct CalendarEntryForm: View {
    @Environment(\.dismiss) var dismiss

    var selectedDate: Date
    var onSave: (CalendarEntry) -> Void

    @State private var title = ""
    @State private var notes = ""
    @State private var startTime = Date()
    @State private var endTime = Date()
    @State private var entryType: CalendarEntryType = .event
    @State private var assignedTo: TaskAssignment = .me

    var body: some View {
        NavigationStack {
            Form {
                Section(header: Text("Details")) {
                    TextField("Title", text: $title)
                    TextField("Notes", text: $notes)
                }

                Section(header: Text("Time")) {
                    DatePicker("Start", selection: $startTime, displayedComponents: .hourAndMinute)
                    DatePicker("End", selection: $endTime, displayedComponents: .hourAndMinute)
                }

                Section(header: Text("Type")) {
                    Picker("Entry Type", selection: $entryType) {
                        ForEach(CalendarEntryType.allCases, id: \.self) {
                            Text($0.rawValue.capitalized)
                        }
                    }
                    Picker("Assigned To", selection: $assignedTo) {
                        ForEach(TaskAssignment.allCases, id: \.self) {
                            Text($0.rawValue.capitalized)
                        }
                    }
                }
            }
            .navigationTitle("New Entry")
            .toolbar {
                ToolbarItem(placement: .confirmationAction) {
                    Button("Save") {
                        let calendar = Calendar.current
                        let fullStart = calendar.date(bySettingHour: calendar.component(.hour, from: startTime),
                                                      minute: calendar.component(.minute, from: startTime),
                                                      second: 0,
                                                      of: selectedDate) ?? selectedDate

                        let fullEnd = calendar.date(bySettingHour: calendar.component(.hour, from: endTime),
                                                    minute: calendar.component(.minute, from: endTime),
                                                    second: 0,
                                                    of: selectedDate)

                        let newEntry = CalendarEntry(
                            title: title,
                            notes: notes.isEmpty ? nil : notes,
                            date: selectedDate,
                            startTime: fullStart,
                            endTime: fullEnd,
                            entryType: entryType,
                            assignedTo: assignedTo
                        )
                        onSave(newEntry)
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
