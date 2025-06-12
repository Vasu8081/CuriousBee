import SwiftUI

struct TaskScheduleForm: View {
    @Environment(\.dismiss) var dismiss
    @EnvironmentObject var calendarViewModel: CalendarViewModel
    @EnvironmentObject var userViewModel: UserViewModel

    @ObservedObject var viewModel: TasksViewModel

    @State private var date: Date = Date()
    @State private var startTime: Date = Date()
    @State private var endTime: Date = Date().addingTimeInterval(3600)
    @State private var assignedTo: UUID? = nil

    var body: some View {
        NavigationStack {
            Form {
                Section(header: Text("Schedule for \(viewModel.title ?? "")")) {
                    DatePicker("Date", selection: $date, displayedComponents: .date)
                    DatePicker("Start Time", selection: $startTime, displayedComponents: .hourAndMinute)
                    DatePicker("End Time", selection: $endTime, displayedComponents: .hourAndMinute)

                    Picker("Assign To", selection: $assignedTo) {
                        Text("Unassigned").tag(UUID?.none)
                        ForEach(userViewModel.getUserViewModels(), id: \.id) { user in
                            if let id = user.id {
                                Text(user.name ?? "Unnamed").tag(id) // Use flat UUID
                            }
                        }
                    }
                    .pickerStyle(MenuPickerStyle())
                    .padding()
                    .background(Color(.systemGray6))
                    .cornerRadius(8)
                }
            }
            .navigationTitle("Schedule Task")
            .toolbar {
                ToolbarItem(placement: .confirmationAction) {
                    Button("Add") {
                        let calendar = Calendar.current
                        let fullStart = calendar.date(
                            bySettingHour: calendar.component(.hour, from: startTime),
                            minute: calendar.component(.minute, from: startTime),
                            second: 0,
                            of: date
                        ) ?? date

                        let fullEnd = calendar.date(
                            bySettingHour: calendar.component(.hour, from: endTime),
                            minute: calendar.component(.minute, from: endTime),
                            second: 0,
                            of: date
                        )
                        
                        var newEntry = CalendarEntries(
                            id: UUID(),
                            user_id: assignedTo,
                            title: viewModel.title,
                            notes: viewModel.notes,
                            date: date,
                            start_time: fullStart,
                            end_time: fullEnd
                        )
                        
                        if viewModel.other_users_presence_necessary ?? false {
                            newEntry.group_id = UUID(uuidString: AuthenticateViewModel.shared.getGroupId())
                        }
                        
                        viewModel.calendarEntry = CalendarEntriesViewModel(model: newEntry)
                        calendarViewModel.addCalendarEntry(viewModel.calendarEntry)
                        viewModel.save()
                        dismiss()
                    }
                    .disabled((viewModel.title ?? "").trimmingCharacters(in: .whitespaces).isEmpty)
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
