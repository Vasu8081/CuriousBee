import SwiftUI

struct LoginViewForm: View {
    @Environment(\.dismiss) var dismiss

    @State private var email = ""
    @State private var password = ""
    @State private var isLoading = false

    @ObservedObject private var auth = AuthenticateViewModel.shared

    var body: some View {
        NavigationStack {
            VStack(spacing: 20) {
                Text("Sign In")
                    .font(.largeTitle.bold())
                    .padding(.top, 40)

                TextField("Email", text: $email)
                    .keyboardType(.emailAddress)
                    .textContentType(.emailAddress)
                    .autocapitalization(.none)
                    .textFieldStyle(RoundedBorderTextFieldStyle())

                SecureField("Password", text: $password)
                    .textContentType(.password)
                    .textFieldStyle(RoundedBorderTextFieldStyle())

                if let error = auth.errorMessage {
                    Text(error)
                        .foregroundColor(.red)
                        .multilineTextAlignment(.center)
                }

                Button(action: login) {
                    if isLoading {
                        ProgressView()
                    } else {
                        Text("Login")
                            .frame(maxWidth: .infinity)
                            .padding()
                            .background(Color.blue)
                            .foregroundColor(.white)
                            .cornerRadius(10)
                    }
                }
                .disabled(isLoading || email.isEmpty || password.isEmpty)

                Spacer()
            }
            .padding()
            .navigationTitle("Login")
            .toolbar {
                ToolbarItem(placement: .cancellationAction) {
                    Button("Cancel") {
                        dismiss()
                    }
                }
            }
        }
    }

    private func login() {
        isLoading = true
        auth.signIn(email: email, password: password)
        
        // Monitor sign-in and dismiss on success
        DispatchQueue.main.asyncAfter(deadline: .now() + 1.5) {
            isLoading = false
            if auth.isSignedIn {
                dismiss()
            }
        }
    }
}
