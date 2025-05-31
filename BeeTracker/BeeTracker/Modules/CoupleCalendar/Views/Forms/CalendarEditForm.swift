import SwiftUI

struct CalendarEditForm: View {
    @Environment(\.dismiss) var dismiss
    @EnvironmentObject var calendarViewModel: CalendarViewModel
    @EnvironmentObject var userViewModel: UserViewModel
    var entryToEdit: CalendarEntriesViewModel

    @State private var title: String = ""
    @State private var notes: String = ""
    @State private var selectedDate: Date = Date()
    @State private var startTime: Date = Date()
    @State private var endTime: Date = Date()
    @State private var assignedUserId: UUID? = nil

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

                Section(header: Text("Assigned To")) {
                    Picker("Assign To", selection: $assignedUserId) {
                        Text("Unassigned").tag(UUID?.none)
                        ForEach(userViewModel.getUserViewModels(), id: \.id) { user in
                            Text(user._name ?? "Unnamed").tag(user._id)
                        }
                    }
                    .pickerStyle(MenuPickerStyle())
                }

                Section {
                    Button(role: .destructive) {
                        calendarViewModel.deleteEntry(entryToEdit)
                        dismiss()
                    } label: {
                        Label("Delete Entry", systemImage: "trash")
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

                        entryToEdit._title = title
                        entryToEdit._notes = notes.isEmpty ? nil : notes
                        entryToEdit._date = selectedDate
                        entryToEdit._start_time = fullStart
                        entryToEdit._end_time = fullEnd
                        let oldUserId = entryToEdit._user_id
                        entryToEdit._user_id = assignedUserId
                        if oldUserId != assignedUserId {
                            calendarViewModel.refreshGroupings(triggerFor: [oldUserId, assignedUserId])
                        }
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
            title = entryToEdit._title ?? ""
            notes = entryToEdit._notes ?? ""
            selectedDate = entryToEdit._date ?? Date()
            startTime = entryToEdit._start_time ?? Date()
            endTime = entryToEdit._end_time ?? entryToEdit._start_time ?? Date()
            assignedUserId = entryToEdit._user_id
        }
    }
}
