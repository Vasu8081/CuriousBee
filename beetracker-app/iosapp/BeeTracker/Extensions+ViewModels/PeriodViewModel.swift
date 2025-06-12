import Foundation

class PeriodViewModel: ObservableObject {
    @Published var periodInfosViewModel: PeriodInfosViewModel = PeriodInfosViewModel(model: PeriodInfos())
    @Published var symptomsViewModel: [SymptomsViewModel] = []
    
    private var symptomIdToNameMap: [String: String] = [:]
    init(){
        
    }
    
    func reload(){
        symptomsViewModel.removeAll()
        symptomIdToNameMap.removeAll()
        ServerEndPoints.shared.getAllPeriodInfos { result in
            switch result {
            case .success(let periodInfo):
                DispatchQueue.main.async {
                    self.periodInfosViewModel = PeriodInfosViewModel(model: periodInfo[0])
                }
            case .failure(let error):
                print("Failed to fetch period infos: \(error)")
            }
        }
        
        ServerEndPoints.shared.getAllSymptoms { result in
            switch result {
            case .success(let symptoms):
                DispatchQueue.main.async {
                    for symptom in symptoms {
                        self.symptomsViewModel.append(SymptomsViewModel(model: symptom))
                        self.symptomIdToNameMap[symptom.id?.uuidString ?? ""] = symptom.name
                    }
                }
            case .failure(let error):
                print("Failed to fetch symptoms: \(error)")
            }
        }
    }
    
    func getAverageCycleLength() -> Int {
        return periodInfosViewModel.average_cycle_length ?? 25
    }
    
    func getAverageDurationOfPeriod() -> Int {
        return periodInfosViewModel.average_period_duration ?? 5
    }
    
    func getPeriodInfoId() -> UUID {
        return periodInfosViewModel.id ?? UUID()
    }
    
    func addPeriodEntry(entry: PeriodEntries){
        let model = PeriodEntriesViewModel(model: entry)
        model.save()
        self.periodInfosViewModel.periodEntries.append(model)
        self.periodInfosViewModel = self.periodInfosViewModel
    }
    
    func getSymptomName(for symptomId: UUID?) -> String {
        guard let symptomId = symptomId else { return "NA" }
        return symptomIdToNameMap[symptomId.uuidString] ?? "NA"
    }
    
    func deletePeriodEntry(entry: PeriodEntriesViewModel) {
        guard let id = entry.id else {
            print("Cannot delete entry: id is nil")
            return
        }
        ServerEndPoints.shared.deletePeriodEntries(id: id) { result in
            switch result {
            case .success:
                self.periodInfosViewModel.periodEntries.removeAll { $0.id == entry.id }
            case .failure(let error):
                print("Failed to delete task entry: \(error)")
            }
        }
        self.periodInfosViewModel = self.periodInfosViewModel
    }
    
    func getAllSymptoms() -> [SymptomsViewModel] {
        return self.symptomsViewModel
    }
    
    func getOngoingPeriodViewModel() -> PeriodEntriesViewModel? {
        return self.periodInfosViewModel.periodEntries.first { entry in
            guard let isEnded = entry.is_ended else { return false }
            return !isEnded
        }
    }
    
    func startNextExpectedPeriod(_ startDate: Date) {
        let new_id = UUID()
        
        let finalEndDate = Calendar.current.date(
            byAdding: .day,
            value: self.getAverageDurationOfPeriod() - 1,
            to: startDate
        ) ?? startDate

        let newEntry = PeriodEntries(
            id: new_id,
            period_info_id: self.getPeriodInfoId(),
            start_date: startDate,
            end_date: finalEndDate,
            notes: nil,
            is_ended: false,
            symptoms: []
        )

        self.addPeriodEntry(entry: newEntry)
    }
    
    func getNextExpectedPeriodDate() -> Date? {
        return self.periodInfosViewModel.predicted_next_period_date
    }
    
    func endOngoingPeriod() {
        guard let ongoingPeriodViewModel = self.getOngoingPeriodViewModel() else { return }
        ongoingPeriodViewModel.is_ended = true
        ongoingPeriodViewModel.end_date = Date()
        if let index = self.periodInfosViewModel.periodEntries.firstIndex(where: { $0.id == ongoingPeriodViewModel.id }) {
            self.periodInfosViewModel.periodEntries[index].is_ended = true
            self.periodInfosViewModel.periodEntries[index].end_date = ongoingPeriodViewModel.end_date
        }
        ongoingPeriodViewModel.save()
        self.periodInfosViewModel = self.periodInfosViewModel
    }
    
    func deleteOngoingPeriod() {
        guard let ongoingPeriodViewModel = self.getOngoingPeriodViewModel() else { return }
        self.deletePeriodEntry(entry: ongoingPeriodViewModel)
    }
    
    func historyEntriesSorted() -> [PeriodEntriesViewModel] {
        self.periodInfosViewModel.periodEntries
            .filter { $0.is_ended == true || Calendar.current.startOfDay(for: $0.end_date ?? .distantPast) < Calendar.current.startOfDay(for: Date()) }
            .sorted { ($0.start_date ?? .distantPast) > ($1.start_date ?? .distantPast) }
    }
}
