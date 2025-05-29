import SwiftUI

struct TaskListView: View {
    @StateObject private var viewModel = TaskViewModel()
    @State private var showForm = false
    @State private var showCompleted = false
    @State private var formMode: Bool? = nil // true = deadline, false = non-deadline
    @State private var taskToEdit: Task? = nil
    @State private var taskToSchedule: Task? = nil

    var body: some View {
        NavigationStack {
            VStack {
                Picker("Filter", selection: $showCompleted) {
                    Text("Pending").tag(false)
                    Text("Completed").tag(true)
                }
                .pickerStyle(.segmented)
                .padding([.top, .horizontal])

                List {
                    ForEach(sortedTasks(), id: \.id) { task in
                        VStack(alignment: .leading, spacing: 8) {
                            HStack(alignment: .top) {
                                Button(action: {
                                    viewModel.toggleComplete(task)
                                }) {
                                    Image(systemName: task.isCompleted ? "checkmark.circle.fill" : "circle")
                                        .foregroundColor(task.isCompleted ? .green : .gray)
                                }
                                .buttonStyle(.plain)

                                VStack(alignment: .leading, spacing: 4) {
                                    HStack {
                                        Text(task.interactionStyle.emoji)
                                        Text(task.title)
                                            .fontWeight(.medium)
                                            .strikethrough(task.isCompleted, color: .gray)
                                    }

                                    if let deadline = task.deadline {
                                        Text("Due: \(deadline.formatted(date: .abbreviated, time: .omitted))")
                                            .font(.caption2)
                                            .foregroundColor(.secondary)
                                    }

                                    if let notes = task.notes {
                                        Text(notes)
                                            .font(.caption2)
                                            .foregroundColor(.gray)
                                    }

                                    Text(task.interactionStyle.description)
                                        .font(.caption2)
                                        .foregroundColor(.accentColor)
                                }

                                Spacer()

                                if !task.isCompleted {
                                    if task.scheduledCalendarEntryId == nil {
                                        Button(action: {
                                            taskToSchedule = task
                                        }) {
                                            Image(systemName: "calendar.badge.plus")
                                                .foregroundColor(.blue)
                                        }
                                        .buttonStyle(.plain)
                                    }

                                    Button(role: .destructive) {
                                        viewModel.deleteTask(task)
                                    } label: {
                                        Image(systemName: "trash")
                                    }
                                    .buttonStyle(.plain)
                                }
                            }
                            .contentShape(Rectangle())
                            .onTapGesture {
                                if !task.isCompleted {
                                    taskToEdit = task
                                }
                            }
                        }
                        .padding(.vertical, 4)
                    }
                }
                .listStyle(.plain)
            }
            .navigationTitle("Tasks")
            .overlay(alignment: .bottom) {
                HStack(spacing: 16) {
                    Button(action: {
                        formMode = true // deadline task
                        showForm = true
                    }) {
                        HStack(spacing: 6) {
                            Image(systemName: "calendar.badge.clock")
                            Text("Deadline")
                        }
                        .padding(.horizontal, 16)
                        .padding(.vertical, 10)
                        .background(Color.accentColor)
                        .foregroundColor(.white)
                        .cornerRadius(20)
                    }

                    Button(action: {
                        formMode = false // non-deadline task
                        showForm = true
                    }) {
                        HStack(spacing: 6) {
                            Image(systemName: "text.badge.plus")
                            Text("Quick Note")
                        }
                        .padding(.horizontal, 16)
                        .padding(.vertical, 10)
                        .background(Color.secondary)
                        .foregroundColor(.white)
                        .cornerRadius(20)
                    }
                }
                .padding()
            }
            .sheet(isPresented: $showForm) {
                TaskFormView(viewModel: viewModel, isDeadlineMode: formMode ?? true)
            }
            .sheet(item: $taskToEdit) { task in
                TaskEditFormView(viewModel: viewModel, task: task)
            }
            .sheet(item: $taskToSchedule) { task in
                TaskScheduleForm(task: task) { entry in
                    UserViewModel.shared.addCalendarEntry(entry)
                    viewModel.linkTaskToCalendar(task: task, entryId: entry.id)
                }
            }
        }
    }

    private func sortedTasks() -> [Task] {
        let tasks = showCompleted ? viewModel.completedTasks : viewModel.pendingTasks
        return tasks.sorted { (lhs, rhs) in
            switch (lhs.deadline, rhs.deadline) {
            case let (l?, r?): return l < r
            case (_?, nil): return true
            case (nil, _?): return false
            default: return lhs.createdDate < rhs.createdDate
            }
        }
    }
}
