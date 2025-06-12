import SwiftUI

struct CalendarEntryForm: View {
    @Environment(\.dismiss) var dismiss
    @EnvironmentObject private var calendarViewModel: CalendarViewModel
    @EnvironmentObject var userViewModel: UserViewModel

    var selectedDate: Date
    @State private var title = ""
    @State private var notes = ""
    @State private var startTime = Date()
    @State private var endTime = Date()
    @State private var assignedUserId: UUID? = nil

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

                    Picker("Assign To", selection: $assignedUserId) {
                        Text("Unassigned").tag(UUID?.none)
                        ForEach(userViewModel.getUserViewModels(), id: \.id) { user in
                            if let id = user.id {
                                Text(user.name ?? "Unnamed").tag(Optional(id))
                            }
                        }
                    }
                    .pickerStyle(MenuPickerStyle())
                }
            }
            .navigationTitle("New Entry")
            .toolbar {
                ToolbarItem(placement: .confirmationAction) {
                    Button("Save") {
                        let calendar = Calendar.current

                        let fullStart = calendar.date(
                            bySettingHour: calendar.component(.hour, from: startTime),
                            minute: calendar.component(.minute, from: startTime),
                            second: 0,
                            of: selectedDate
                        ) ?? selectedDate

                        let fullEnd = calendar.date(
                            bySettingHour: calendar.component(.hour, from: endTime),
                            minute: calendar.component(.minute, from: endTime),
                            second: 0,
                            of: selectedDate
                        )

                        let newEntry = CalendarEntries(
                            user_id: assignedUserId,
                            title: title,
                            notes: notes.isEmpty ? nil : notes,
                            date: selectedDate,
                            start_time: fullStart,
                            end_time: fullEnd
                        )

                        calendarViewModel.addEntry(newEntry)
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
