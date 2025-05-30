// CalendarEntryForm.swift
import SwiftUI

struct CalendarEntryForm: View {
    @Environment(\.dismiss) var dismiss
    @EnvironmentObject private var calendarViewModel: CalendarViewModel
    
    var selectedDate: Date
    @State private var title = ""
    @State private var notes = ""
    @State private var startTime = Date()
    @State private var endTime = Date()
    @State private var assignedTo: Users? = nil

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
                    Picker("Assign To", selection: $assignedTo) {
                        ForEach(calendarViewModel.users, id: \.self) { user in
                            Text(user._name ?? "Unnamed").tag(Optional(user))
                        }
                    }
                    .pickerStyle(MenuPickerStyle())
                    .padding()
                    .background(Color(.systemGray6))
                    .cornerRadius(8)
                }
            }
            .navigationTitle("New Entry")
            .toolbar {
                ToolbarItem(placement: .confirmationAction) {
                    Button("Save") {
                        let calendar = Calendar.current
                        let startHour = calendar.component(.hour, from: startTime)
                        let startMinute = calendar.component(.minute, from: startTime)
                        let endHour = calendar.component(.hour, from: endTime)
                        let endMinute = calendar.component(.minute, from: endTime)

                        let fullStart = calendar.date(bySettingHour: startHour, minute: startMinute, second: 0, of: selectedDate) ?? selectedDate
                        let fullEnd = calendar.date(bySettingHour: endHour, minute: endMinute, second: 0, of: selectedDate)

                        let newEntry = CalendarEntries(
                            _user_id: assignedTo?._id,
                            _title: title,
                            _notes: notes.isEmpty ? nil : notes,
                            _date: selectedDate,
                            _start_time: fullStart,
                            _end_time: fullEnd
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
