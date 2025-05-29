import SwiftUI

struct WrapView<T: Hashable, Content: View>: View {
    let tags: [T]
    let content: (T) -> Content

    var body: some View {
        FlexibleView(data: tags, spacing: 8, alignment: .leading, content: content)
    }
}
