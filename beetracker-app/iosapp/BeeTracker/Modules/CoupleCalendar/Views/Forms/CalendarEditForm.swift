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
                            Text(user.name ?? "Unnamed").tag(user.id)
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

                        entryToEdit.title = title
                        entryToEdit.notes = notes.isEmpty ? nil : notes
                        entryToEdit.date = selectedDate
                        entryToEdit.start_time = fullStart
                        entryToEdit.end_time = fullEnd
                        let oldUserId = entryToEdit.user_id
                        entryToEdit.user_id = assignedUserId
                        if oldUserId != assignedUserId {
                            calendarViewModel.refreshGroupings(triggerFor: [oldUserId, assignedUserId])
                        }
                        entryToEdit.save()
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
            title = entryToEdit.title ?? ""
            notes = entryToEdit.notes ?? ""
            selectedDate = entryToEdit.date ?? Date()
            startTime = entryToEdit.start_time ?? Date()
            endTime = entryToEdit.end_time ?? entryToEdit.start_time ?? Date()
            assignedUserId = entryToEdit.user_id
        }
    }
}
