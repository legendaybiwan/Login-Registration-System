/*
 * TASK 2: Login and Registration System
 * --------------------------------------
 * - Registration function that takes username and password as input.
 * - Validates the inputs and checks for duplicate usernames.
 * - Stores user credentials in a file (users.txt) - one line per user,
 *   in the format:  username|salt|passwordHash
 * - A login function reads credentials from the file and verifies user
 *   identity.
 * - Prints appropriate success / error messages for registration and login.
 *
 * NOTE ON PASSWORD SECURITY:
 * The C++ standard library does not ship a cryptographic hash function
 * (no SHA-256/bcrypt) without pulling in an external library like OpenSSL.
 * To keep this project dependency-free and easy to compile anywhere, a
 * simple salted hash (based on std::hash + a manual mixing step) is used
 * instead of storing the password in plain text. This is fine for a
 * classroom/learning project, but for real production systems you should
 * use a proper library such as OpenSSL (SHA-256) or bcrypt/Argon2.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <cctype>
#include <algorithm>

using namespace std;

const string DATA_FILE = "users.txt";

// ---------------------------------------------------------------------------
// Simple salted hashing (NOT cryptographically secure - see note above)
// ---------------------------------------------------------------------------
string generateSalt(int length = 16) {
    static const char chars[] = "0123456789abcdef";
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, 15);
    string salt;
    for (int i = 0; i < length; i++) {
        salt += chars[dist(gen)];
    }
    return salt;
}

string hashPassword(const string &password, const string &salt) {
    string combined = salt + password;
    hash<string> hasher;
    size_t h = hasher(combined);

    // Mix again to make the hash less trivially reversible
    hash<string> hasher2;
    size_t h2 = hasher2(combined + to_string(h));

    stringstream ss;
    ss << hex << h << h2;
    return ss.str();
}

// ---------------------------------------------------------------------------
// User record + file storage helpers
// ---------------------------------------------------------------------------
struct UserRecord {
    string username;
    string salt;
    string passwordHash;
};

vector<UserRecord> loadUsers() {
    vector<UserRecord> users;
    ifstream file(DATA_FILE);
    if (!file.is_open()) return users;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string username, salt, passwordHash;
        getline(ss, username, '|');
        getline(ss, salt, '|');
        getline(ss, passwordHash, '|');
        users.push_back({username, salt, passwordHash});
    }
    return users;
}

void appendUser(const UserRecord &user) {
    ofstream file(DATA_FILE, ios::app);
    file << user.username << "|" << user.salt << "|" << user.passwordHash << "\n";
}

bool usernameExists(const vector<UserRecord> &users, const string &username) {
    for (auto &u : users) {
        if (u.username == username) return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Input validation
// ---------------------------------------------------------------------------
bool isValidUsername(const string &username, string &errorMsg) {
    if (username.length() < 3) {
        errorMsg = "Username must be at least 3 characters long.";
        return false;
    }
    for (char c : username) {
        if (!isalnum(static_cast<unsigned char>(c))) {
            errorMsg = "Username must contain only letters and numbers.";
            return false;
        }
    }
    return true;
}

bool isValidPassword(const string &password, string &errorMsg) {
    if (password.length() < 6) {
        errorMsg = "Password must be at least 6 characters long.";
        return false;
    }
    bool hasDigit = false, hasAlpha = false;
    for (char c : password) {
        if (isdigit(static_cast<unsigned char>(c))) hasDigit = true;
        if (isalpha(static_cast<unsigned char>(c))) hasAlpha = true;
    }
    if (!hasDigit) {
        errorMsg = "Password must contain at least one digit.";
        return false;
    }
    if (!hasAlpha) {
        errorMsg = "Password must contain at least one letter.";
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Core features
// ---------------------------------------------------------------------------
void registerUser() {
    vector<UserRecord> users = loadUsers();

    string username, password, confirmPassword, errorMsg;

    cout << "Choose a username: ";
    cin >> username;
    if (!isValidUsername(username, errorMsg)) {
        cout << "Registration failed: " << errorMsg << "\n";
        return;
    }

    if (usernameExists(users, username)) {
        cout << "Registration failed: Username already exists. Please choose another.\n";
        return;
    }

    cout << "Choose a password: ";
    cin >> password;
    if (!isValidPassword(password, errorMsg)) {
        cout << "Registration failed: " << errorMsg << "\n";
        return;
    }

    cout << "Confirm password: ";
    cin >> confirmPassword;
    if (password != confirmPassword) {
        cout << "Registration failed: Passwords do not match.\n";
        return;
    }

    string salt = generateSalt();
    string passwordHash = hashPassword(password, salt);
    appendUser({username, salt, passwordHash});

    cout << "Registration successful! Welcome, " << username << ". You can now log in.\n";
}

bool loginUser() {
    vector<UserRecord> users = loadUsers();

    string username, password;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    for (auto &u : users) {
        if (u.username == username) {
            string attemptedHash = hashPassword(password, u.salt);
            if (attemptedHash == u.passwordHash) {
                cout << "Login successful! Welcome back, " << username << ".\n";
                return true;
            } else {
                cout << "Login failed: Incorrect password.\n";
                return false;
            }
        }
    }

    cout << "Login failed: Username not found.\n";
    return false;
}

// ---------------------------------------------------------------------------
// Menu driven program
// ---------------------------------------------------------------------------
int main() {
    cout << string(50, '=') << "\n";
    cout << "        LOGIN AND REGISTRATION SYSTEM\n";
    cout << string(50, '=') << "\n";

    while (true) {
        cout << "\n1. Register\n2. Login\n3. Exit\n";
        cout << "Enter your choice (1-3): ";
        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice. Please select 1, 2, or 3.\n";
            continue;
        }

        if (choice == 1) {
            registerUser();
        } else if (choice == 2) {
            loginUser();
        } else if (choice == 3) {
            cout << "Goodbye!\n";
            break;
        } else {
            cout << "Invalid choice. Please select 1, 2, or 3.\n";
        }
    }

    return 0;
}
