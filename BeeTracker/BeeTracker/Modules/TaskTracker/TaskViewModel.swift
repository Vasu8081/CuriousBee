import Foundation

class TaskViewModel: ObservableObject {
    @Published var tasks: [Task] = []

    init() {
        loadTasks()
    }

    private func loadTasks() {
        tasks = UserViewModel.shared.userData.tasks
    }

    func addTask(_ task: Task) {
        tasks.append(task)
        persist()
    }

    func updateTask(_ updated: Task) {
        if let index = tasks.firstIndex(where: { $0.id == updated.id }) {
            tasks[index] = updated
            persist()
        }
    }

    func deleteTask(_ task: Task) {
        tasks.removeAll { $0.id == task.id }
        persist()
    }

    func toggleComplete(_ task: Task) {
        guard let index = tasks.firstIndex(where: { $0.id == task.id }) else { return }
        tasks[index].isCompleted.toggle()
        persist()
    }

    private func persist() {
        UserViewModel.shared.userData.tasks = tasks
        UserViewModel.shared.save()
    }

    var pendingTasks: [Task] {
        tasks.filter { !$0.isCompleted }
    }

    var completedTasks: [Task] {
        tasks.filter { $0.isCompleted }
    }
    
    func linkTaskToCalendar(task: Task, entryId: UUID) {
        if let index = tasks.firstIndex(where: { $0.id == task.id }) {
            tasks[index].scheduledCalendarEntryId = entryId
            persist()
        }
    }
}
