import SwiftUI

enum TaskTab: String, CaseIterable {
    case withDeadline = "With Deadline"
    case withoutDeadline = "No Deadline"
    case completed = "Completed"
}

struct TaskListView: View {
    @EnvironmentObject private var taskViewModel: TaskViewModel
    @State private var showForm = false
    @State private var formMode: Bool? = nil
    @State private var taskToEdit: TasksViewModel? = nil
    @State private var taskToSchedule: TasksViewModel? = nil
    @State private var selectedTab: TaskTab = .withDeadline
    @State private var selectedTaskType: TaskType? = nil

    var body: some View {
        NavigationStack {
            VStack {
                HStack {
                    Picker("View", selection: $selectedTab) {
                        ForEach(TaskTab.allCases, id: \.self) {
                            Text($0.rawValue)
                        }
                    }
                    .pickerStyle(.segmented)

                    Spacer()

                    Menu {
                        Button("All Types", action: { selectedTaskType = nil })
                        Divider()
                        ForEach(TaskType.allCases, id: \.self) { type in
                            Button(type.rawValue.replacingOccurrences(of: "_", with: " ").capitalized) {
                                selectedTaskType = type
                            }
                        }
                    } label: {
                        Label(
                            selectedTaskType?.rawValue.replacingOccurrences(of: "_", with: " ").capitalized ?? "Filter",
                            systemImage: "line.3.horizontal.decrease.circle"
                        )
                        .labelStyle(.iconOnly)
                        .foregroundColor(.primary)
                        .padding(.horizontal)
                    }
                }
                .padding(.horizontal)

                List {
                    ForEach(filteredTasks(for: selectedTab), id: \.id) { task in
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
                Button(action: {
                    formMode = false
                    showForm = true
                }) {
                    HStack(spacing: 6) {
                        Image(systemName: "text.badge.plus")
                        Text("Add Task")
                    }
                    .padding(.horizontal, 16)
                    .padding(.vertical, 10)
                    .background(Color.accentColor)
                    .foregroundColor(.white)
                    .cornerRadius(20)
                }
                .padding()
            }
            .sheet(isPresented: $showForm) {
                TaskFormView()
            }
            .sheet(item: $taskToEdit) { task in
                TaskEditFormView(task: task)
            }
            .sheet(item: $taskToSchedule) { task in
                TaskScheduleForm(viewModel: task)
            }
        }
    }

    private func filteredTasks(for tab: TaskTab) -> [TasksViewModel] {
        let baseTasks: [TasksViewModel] = {
            switch tab {
            case .withDeadline:
                return taskViewModel.getUncompletedTasks().filter { $0.deadline != nil }
            case .withoutDeadline:
                return taskViewModel.getUncompletedTasks().filter { $0.deadline == nil }
            case .completed:
                return taskViewModel.getCompletedTasks()
            }
        }()

        let typeFiltered = selectedTaskType == nil
            ? baseTasks
            : baseTasks.filter { $0.type == selectedTaskType }

        return typeFiltered.sorted(by: deadlineSorter)
    }

    private func deadlineSorter(lhs: TasksViewModel, rhs: TasksViewModel) -> Bool {
        switch (lhs.deadline, rhs.deadline) {
        case let (l?, r?): return l < r
        case (_?, nil): return true
        case (nil, _?): return false
        default: return true
        }
    }
}
