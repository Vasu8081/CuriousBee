import SwiftUI

struct TaskListView: View {
    @EnvironmentObject private var taskViewModel: TaskViewModel
    @State private var showForm = false
    @State private var showCompleted = false
    @State private var formMode: Bool? = nil
    @State private var taskToEdit: TasksViewModel? = nil
    @State private var taskToSchedule: TasksViewModel? = nil

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
                        TaskCardView(
                            task: task,
                            onEdit: { taskToEdit = task },
                            onSchedule: { taskToSchedule = task },
                            onDelete: { taskViewModel.deleteTask(task) },
                            onToggleComplete: { taskViewModel.toggleComplete(task) }
                        )
                    }
                }
                .listStyle(.plain)
            }
            .navigationTitle("Tasks")
            .overlay(alignment: .bottom) {
                HStack(spacing: 16) {
                    Button(action: {
                        formMode = true
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
                        formMode = false
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
                TaskFormView(isDeadlineMode: formMode ?? true)
            }
            .sheet(item: $taskToEdit) { task in
                TaskEditFormView(task: task)
            }
            .sheet(item: $taskToSchedule) { task in
                TaskScheduleForm(viewModel: task)
            }
        }
    }

    private func sortedTasks() -> [TasksViewModel] {
        let tasks = showCompleted ? taskViewModel.getCompletedTasks() : taskViewModel.getUncompletedTasks()
        return tasks.sorted { lhs, rhs in
            switch (lhs.deadline, rhs.deadline) {
            case let (l?, r?): return l < r
            case (_?, nil): return true
            case (nil, _?): return false
            default: return true
            }
        }
    }
}
