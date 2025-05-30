import SwiftUI

enum ToastType {
    case info, success, warning, error

    var backgroundColor: Color {
        switch self {
        case .info: return Color.blue.opacity(0.8)
        case .success: return Color.green.opacity(0.8)
        case .warning: return Color.orange.opacity(0.85)
        case .error: return Color.red.opacity(0.85)
        }
    }

    var prefix: String {
        switch self {
        case .info: return "ℹ️"
        case .success: return "✅"
        case .warning: return "⚠️"
        case .error: return "❌"
        }
    }
}

class ToastManager: ObservableObject {
    static let shared = ToastManager()

    @Published var message: String? = nil
    @Published var isShowing = false
    @Published var type: ToastType = .info
    @Published var isOfflineWarningActive = false

    private init() {}

    func show(_ message: String, type: ToastType = .info, duration: Double = 1.5, persistent: Bool = false) {
        self.message = message
        self.type = type

        if persistent {
            self.isOfflineWarningActive = true
        } else {
            self.isShowing = true
            DispatchQueue.main.asyncAfter(deadline: .now() + duration) {
                self.isShowing = false
            }
        }
    }

    func hidePersistentWarning() {
        self.isOfflineWarningActive = false
    }
}
