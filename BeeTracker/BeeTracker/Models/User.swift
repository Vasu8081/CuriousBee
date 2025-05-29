import Foundation

struct User: Codable {
    var lastModified: Date = Date()
    var name: String = ""
    var aliasses: [String] = ["baby", "love", "sweetheart"]
    var cuteAliasses: [String] = ["cutie", "bunny", "snugglebug"]
    var hotAliasses: [String] = ["hottie", "fire", "bombshell"]
    var naughtyAliasses: [String] = ["naughty girl", "bad baby", "minx"]
    var periodTracker: PeriodTracker = PeriodTracker()
    var products: [Product] = []
    var calendarEntries: [CalendarEntry] = []
    var tasks: [Task] = []
}
