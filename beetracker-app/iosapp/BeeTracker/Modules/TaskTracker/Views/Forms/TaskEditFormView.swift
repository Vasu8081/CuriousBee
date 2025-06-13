import SwiftUI

struct TaskEditFormView: View {
    @Environment(\.dismiss) var dismiss
    @EnvironmentObject private var taskViewModel: TaskViewModel
    @EnvironmentObject private var userViewModel: UserViewModel

    var entry: TasksViewModel

    @State private var showDeadlinePicker = false
    @State private var title: String
    @State private var notes: String
    @State private var deadline: Date?
    @State private var isShared: Bool
    @State private var isConscience: Bool
    @State private var primaryDoer: UUID?
    @State private var taskType: TaskType?

    init(task: TasksViewModel) {
        self.entry = task
        _title = State(initialValue: task.title ?? "")
        _notes = State(initialValue: task.notes ?? "")
        _deadline = State(initialValue: task.deadline)
        _isShared = State(initialValue: task.other_users_presence_necessary ?? true)
        _isConscience = State(initialValue: task.interaction_style == "Full Conscience")
        _primaryDoer = State(initialValue: task.primary_doer_user_id)
        _taskType = State(initialValue: task.type)
    }

    var body: some View {
        NavigationStack {
            Form {
                Section(header: Text("Task Details")) {
                    TextField("What needs to be done?", text: $title)
                    TextField("Notes (optional)", text: $notes)
                }
                
                if entry.deadline == nil {
                            Section {
                                Toggle("Add Deadline", isOn: $showDeadlinePicker)
                                if showDeadlinePicker {
                                    DatePicker("Select Deadline", selection: Binding(
                                        get: { deadline ?? Date() },
                                        set: { deadline = $0 }),
                                        displayedComponents: .date)
                                }
                            }
                        } else {
                            Section(header: Text("Deadline")) {
                                DatePicker("Select Deadline", selection: Binding(
                                    get: { deadline ?? Date() },
                                    set: { deadline = $0 }),
                                    displayedComponents: .date)
                            }
                        }
                
                
                Section(header: Text("How will it be done?")) {
                    Picker("How will it be done?", selection: $isShared) {
                        Text("Alone").tag(false)
                        Text("Together").tag(true)
                    }
                    .pickerStyle(.segmented)
                }

                Section(header: Text("Interaction Style")) {
                    Picker("Interaction Style", selection: $isConscience) {
                        Text("Partial").tag(false)
                        Text("Full").tag(true)
                    }
                    .pickerStyle(.segmented)
                }
                
                Section(header: Text("Task Type")) {
                    Picker("Task Type", selection: $taskType) {
                        ForEach(TaskType.allCases, id: \.self) { type in
                            Text(type.rawValue.replacingOccurrences(of: "_", with: " ").capitalized).tag(type)
                        }
                    }
                    .pickerStyle(MenuPickerStyle())
                }

                Section(header: Text("Primary Doer (optional)")) {
                    Picker("Assign To", selection: $primaryDoer) {
                        Text("Unassigned").tag(UUID?.none)
                        ForEach(userViewModel.getUserViewModels(), id: \.id) { user in
                            if let id = user.id {
                                Text(user.name ?? "Unnamed").tag(id) // Use flat UUID
                            }
                        }
                    }
                    .pickerStyle(MenuPickerStyle())
                }
            }
            .navigationTitle("Edit Task")
            .toolbar {
                ToolbarItem(placement: .confirmationAction) {
                    Button("Save") {
                        entry.title = title
                        entry.notes = notes.isEmpty ? nil : notes
                        entry.deadline = deadline
                        entry.interaction_style = isConscience ? "Full Conscience" : "Partial Conscience"
                        entry.primary_doer_user_id = primaryDoer
                        entry.other_users_presence_necessary = isShared
                        entry.type = taskType
                        entry.save()
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
