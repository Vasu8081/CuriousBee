import SwiftUI

struct TaskEditFormView: View {
    @Environment(\.dismiss) var dismiss
    @ObservedObject var viewModel: TaskViewModel

    var original: Task

    @State private var title: String
    @State private var notes: String
    @State private var deadline: Date?
    @State private var interactionStyle: TaskInteractionStyle
    @State private var primaryDoer: TaskDoer?

    init(viewModel: TaskViewModel, task: Task) {
        self.viewModel = viewModel
        self.original = task

        _title = State(initialValue: task.title)
        _notes = State(initialValue: task.notes ?? "")
        _deadline = State(initialValue: task.deadline)
        _interactionStyle = State(initialValue: task.interactionStyle)
        _primaryDoer = State(initialValue: task.primaryDoer)
    }

    var body: some View {
        NavigationStack {
            Form {
                Section(header: Text("Task Details")) {
                    TextField("What needs to be done?", text: $title)
                    TextField("Notes (optional)", text: $notes)
                }

                if original.deadline != nil {
                    Section(header: Text("Deadline")) {
                        DatePicker("Deadline", selection: Binding(
                            get: { deadline ?? Date() },
                            set: { deadline = $0 }),
                            displayedComponents: .date)
                    }
                }

                Section(header: Text("How will it be done?")) {
                    LazyVGrid(columns: Array(repeating: .init(.flexible()), count: 2), spacing: 12) {
                        ForEach(TaskInteractionStyle.allCases, id: \.self) { style in
                            VStack(spacing: 4) {
                                Text(style.emoji)
                                Text(style.description)
                                    .font(.caption2)
                                    .multilineTextAlignment(.center)
                            }
                            .padding(8)
                            .frame(maxWidth: .infinity)
                            .background(interactionStyle == style ? Color.accentColor.opacity(0.2) : Color.gray.opacity(0.1))
                            .cornerRadius(8)
                            .onTapGesture {
                                interactionStyle = style
                            }
                        }
                    }
                    .padding(.vertical, 4)
                }

                Section(header: Text("Primary Doer (optional)")) {
                    Picker("Who's the main person doing it?", selection: Binding(
                        get: { primaryDoer ?? .me },
                        set: { primaryDoer = $0 })) {
                        Text("None").tag(TaskDoer?.none)
                        ForEach(TaskDoer.allCases, id: \.self) { doer in
                            Text(doer.rawValue.capitalized).tag(TaskDoer?.some(doer))
                        }
                    }
                }

                if let entryId = original.scheduledCalendarEntryId,
                   let index = UserViewModel.shared.calendarEntries.firstIndex(where: { $0.id == entryId }) {
                    Section(header: Text("Scheduled Time")) {
                        DatePicker("Date", selection: Binding(
                            get: { UserViewModel.shared.calendarEntries[index].date },
                            set: { UserViewModel.shared.calendarEntries[index].date = $0 }),
                                   displayedComponents: .date)

                        DatePicker("Start Time", selection: Binding(
                            get: { UserViewModel.shared.calendarEntries[index].startTime },
                            set: { UserViewModel.shared.calendarEntries[index].startTime = $0 }),
                                   displayedComponents: .hourAndMinute)

                        DatePicker("End Time", selection: Binding(
                            get: { UserViewModel.shared.calendarEntries[index].endTime ?? Date().addingTimeInterval(3600) },
                            set: { UserViewModel.shared.calendarEntries[index].endTime = $0 }),
                                   displayedComponents: .hourAndMinute)
                    }
                }
            }
            .navigationTitle("Edit Task")
            .toolbar {
                ToolbarItem(placement: .confirmationAction) {
                    Button("Save") {
                        var updated = original
                        updated.title = title
                        updated.notes = notes.isEmpty ? nil : notes
                        updated.deadline = deadline
                        updated.interactionStyle = interactionStyle
                        updated.primaryDoer = primaryDoer
                        viewModel.updateTask(updated)
                        UserViewModel.shared.save()
                        dismiss()
                    }
                    .disabled(title.trimmingCharacters(in: .whitespaces).isEmpty)
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
