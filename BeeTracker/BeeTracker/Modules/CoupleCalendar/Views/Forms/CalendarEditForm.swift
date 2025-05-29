import SwiftUI

struct CalendarEditForm: View {
    @Environment(\.dismiss) var dismiss

    var entryToEdit: CalendarEntry
    var onSave: (CalendarEntry) -> Void
    var onDelete: (() -> Void)? = nil

    @State private var title: String = ""
    @State private var notes: String = ""
    @State private var selectedDate: Date = Date()
    @State private var startTime: Date = Date()
    @State private var endTime: Date = Date()
    @State private var entryType: CalendarEntryType = .event
    @State private var assignedTo: TaskAssignment = .me

    var body: some View {
        NavigationStack {
            Form {
                Section(header: Text("Details")) {
                    TextField("Title", text: $title)
                    TextField("Notes", text: $notes)
                }

                Section(header: Text("Date & Time")) {
                    DatePicker("Date", selection: $selectedDate, displayedComponents: .date)
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

                if onDelete != nil {
                    Section {
                        Button(role: .destructive) {
                            onDelete?()
                            dismiss()
                        } label: {
                            Label("Delete Entry", systemImage: "trash")
                        }
                    }
                }
            }
            .navigationTitle("Edit Entry")
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

                        let updatedEntry = CalendarEntry(
                            id: entryToEdit.id,
                            title: title,
                            notes: notes.isEmpty ? nil : notes,
                            date: selectedDate,
                            startTime: fullStart,
                            endTime: fullEnd,
                            entryType: entryType,
                            assignedTo: assignedTo,
                            linkedTaskId: entryToEdit.linkedTaskId
                        )

                        onSave(updatedEntry)
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
        .onAppear {
            title = entryToEdit.title
            notes = entryToEdit.notes ?? ""
            selectedDate = entryToEdit.date
            startTime = entryToEdit.startTime
            endTime = entryToEdit.endTime ?? entryToEdit.startTime
            entryType = entryToEdit.entryType
            assignedTo = entryToEdit.assignedTo
        }
    }
}
