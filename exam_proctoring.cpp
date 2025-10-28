// main.cpp
// Combined Online Exam Proctoring System (In-memory, Single File)
// Features: Login, Registration, Faculty Panel, Proctoring, Time Alerts, Severity Verdict

#include <bits/stdc++.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <thread>
#include <chrono>
using namespace std;
using namespace std::chrono;

// ----------------- Color Helpers -----------------
#ifdef _WIN32
void enableVirtualTerminalProcessing() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

namespace Color {
    const string RESET = "\x1b[0m";
    const string RED = "\x1b[31m";
    const string GREEN = "\x1b[32m";
    const string YELLOW = "\x1b[33m";
    const string BLUE = "\x1b[34m";
    const string CYAN = "\x1b[36m";
    const string BOLD = "\x1b[1m";
    string red(const string &s)   { return RED + s + RESET; }
    string green(const string &s) { return GREEN + s + RESET; }
    string yellow(const string &s){ return YELLOW + s + RESET; }
    string blue(const string &s)  { return BLUE + s + RESET; }
    string cyan(const string &s)  { return CYAN + s + RESET; }
    string bold(const string &s)  { return BOLD + s + RESET; }
}

// ----------------- Data Models -----------------
enum QType { MCQ = 1, TEXT = 2 };

struct Question {
    int id;
    QType type;
    string prompt;
    vector<string> options;
    char correct;
};

struct User {
    string username;
    string password;
    string role;
    double score = 0.0;
    time_point<steady_clock> startTime;
    time_point<steady_clock> endTime;
};

struct Alert {
    string type;
    string message;
    string severity; // Low, Medium, High
};

// ----------------- Global Stores -----------------
map<string, User> users;
vector<Question> questions;
queue<Alert> proctorAlerts;
stack<string> loginHistory;
set<string> examAttemptedUsers;

// ----------------- Utilities -----------------
void pauseForMs(int ms) { this_thread::sleep_for(chrono::milliseconds(ms)); }

void printHeader() {
    cout << Color::cyan("==================================================\n");
    cout << Color::bold("   ONLINE EXAM PROCTORING SYSTEM (IN-MEMORY)     \n");
    cout << Color::cyan("==================================================\n\n");
}

string inputLineTrimmed() {
    string s;
    getline(cin, s);
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

int readInt() {
    string line;
    while (true) {
        getline(cin, line);
        stringstream ss(line);
        int x;
        if (ss >> x && ss.eof()) return x;
        cout << Color::red("Invalid number, try again: ");
    }
}

// ----------------- Proctoring Simulation -----------------
void generateAlert(const string &username, const string &type, const string &message, const string &severity) {
    proctorAlerts.push({type, "[" + username + "] " + message, severity});
}

void maybeGenerateRandomAlert(const string &username) {
    int r = rand() % 100;
    if (r < 12) {
        vector<tuple<string, string, string>> alerts = {
            {"Window Switch", "Window switch detected", "Medium"},
            {"Attention Drop", "Attention drop detected (simulated)", "Medium"},
            {"Background Voice", "Background voice detected", "Low"},
            {"Multiple Faces", "Multiple faces in frame (simulated)", "High"},
            {"Audio Spike", "Unexpected audio spike", "Low"}
        };
        auto [type, msg, sev] = alerts[rand() % alerts.size()];
        generateAlert(username, type, msg, sev);
    }
}

void showAllAlerts() {
    if (proctorAlerts.empty()) {
        cout << Color::green("No proctor alerts at the moment.\n");
        return;
    }
    cout << Color::yellow("\n--- Proctor Alerts Log (FIFO) ---\n");
    cout << left << setw(15) << "TYPE" << setw(50) << "MESSAGE" << setw(10) << "SEVERITY\n";
    cout << string(80, '-') << "\n";
    queue<Alert> copy = proctorAlerts;
    while (!copy.empty()) {
        auto a = copy.front(); copy.pop();
        string color = (a.severity == "High") ? Color::RED :
                       (a.severity == "Medium") ? Color::YELLOW : Color::CYAN;
        cout << color << left << setw(15) << a.type << setw(50) << a.message
             << setw(10) << a.severity << Color::RESET << "\n";
    }
    cout << Color::yellow("----------------------------------------\n");
}

// ----------------- Faculty Features -----------------
void facultyAddQuestion() {
    cout << Color::blue("\nAdd a new question\n");
    Question q;
    cout << "Enter QID (integer): ";
    q.id = readInt();
    cout << "Choose type: 1) MCQ 2) Short Answer\nChoice: ";
    int t = readInt();
    if (t == 1) q.type = MCQ; else q.type = TEXT;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter the question prompt:\n";
    q.prompt = inputLineTrimmed();
    if (q.type == MCQ) {
        q.options.resize(4);
        for (int i = 0; i < 4; ++i) {
            cout << "Option " << char('A' + i) << ": ";
            q.options[i] = inputLineTrimmed();
        }
        cout << "Enter correct option (A-D): ";
        char c;
        while (true) {
            string s; getline(cin, s);
            if (s.size() >= 1) {
                c = toupper(s[0]);
                if (c >= 'A' && c <= 'D') break;
            }
            cout << Color::red("Invalid input. Enter A-D: ");
        }
        q.correct = c;
    } else {
        q.options.clear(); q.correct = 0;
    }
    for (auto &qq : questions)
        if (qq.id == q.id) {
            cout << Color::red("QID already exists. Not added.\n");
            return;
        }
    questions.push_back(q);
    cout << Color::green("Question added successfully.\n");
}

void facultyViewQuestions() {
    if (questions.empty()) {
        cout << Color::yellow("No questions available yet.\n");
        return;
    }
    cout << Color::blue("\n--- Question Bank ---\n");
    for (auto &q : questions) {
        cout << Color::bold("QID ") << q.id << ": " << q.prompt << "\n";
        if (q.type == MCQ) {
            for (int i = 0; i < 4; ++i)
                cout << " " << char('A' + i) << ") " << q.options[i] << "\n";
            cout << " (Correct: " << q.correct << ")\n";
        } else cout << " (Short answer)\n";
        cout << "-------------------------\n";
    }
}

void facultyMenu(const string &username) {
    while (true) {
        cout << Color::cyan("\n--- Faculty Panel ---\n");
        cout << "1) Add Question\n2) View All Questions\n3) View Proctor Alerts\n4) Back to Main\nChoice: ";
        int ch = readInt();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (ch == 1) facultyAddQuestion();
        else if (ch == 2) facultyViewQuestions();
        else if (ch == 3) showAllAlerts();
        else if (ch == 4) break;
        else cout << Color::red("Invalid choice.\n");
    }
}

// ----------------- Student Features -----------------
void countdownStart() {
    cout << Color::yellow("\nExam starting in: ");
    for (int i = 3; i >= 1; --i) {
        cout << Color::bold(to_string(i)) << " ";
        cout.flush();
        pauseForMs(800);
    }
    cout << "\n\n";
}

void showProctorSummary(const string &username) {
    auto &user = users[username];
    auto duration = duration_cast<seconds>(user.endTime - user.startTime).count();
    int totalQ = questions.size();
    int maxScore = totalQ * 10;
    int obtained = (int)user.score;

    cout << Color::blue("\n=== EXAM SUMMARY REPORT ===\n");
    cout << "Student: " << username << "\n";
    cout << "Score: " << obtained << "/" << maxScore << " (" 
         << fixed << setprecision(1) << (obtained * 100.0 / maxScore) << "%)\n";
    cout << "Time Taken: " << duration / 60 << " min " << duration % 60 << " sec\n";

    // Time-based alerts
    if (duration < 60) {
        generateAlert(username, "Suspicious Speed", 
                      "Exam completed in under 1 minute — possible malpractice.", "High");
    } else if (duration < 120) {
        generateAlert(username, "Fast Completion", 
                      "Exam completed faster than expected.", "Medium");
    }

    // Count alerts
    int wrong = 0, high = 0, med = 0, low = 0;
    queue<Alert> copy = proctorAlerts;
    while (!copy.empty()) {
        auto a = copy.front(); copy.pop();
        if (a.message.find("[" + username + "]") == string::npos) continue;
        if (a.type.find("Wrong") != string::npos) wrong++;
        if (a.severity == "High") high++;
        else if (a.severity == "Medium") med++;
        else low++;
    }

    cout << "\nTotal Alerts: " << (wrong + high + med + low) << "\n";
    cout << " - Wrong Answers: " << wrong << "\n";
    cout << " - High Severity: " << high << "\n";
    cout << " - Medium Severity: " << med << "\n";
    cout << " - Low Severity: " << low << "\n";

    if (high > 0)
        cout << Color::red("\nFinal Verdict: Manual Review Required [Warning]\n");
    else if (med > 1 || wrong > 1)
        cout << Color::yellow("\nFinal Verdict: Needs Caution [Warning]\n");
    else
        cout << Color::green("\nFinal Verdict: Clear [Checkmark]\n");
}

void studentTakeExam(const string &username) {
    if (questions.empty()) {
        cout << Color::red("No exam available yet.\n");
        return;
    }
    if (examAttemptedUsers.count(username)) {
        cout << Color::red("You have already attempted the exam. Re-attempt not allowed.\n");
        return;
    }

    cout << Color::green("\nIdentity check... OK\n");
    countdownStart();

    auto &user = users[username];
    user.startTime = steady_clock::now();
    proctorAlerts = queue<Alert>{}; // Clear old alerts for this session

    int mcqCount = 0, textCount = 0, scoreMCQ = 0, scoreText = 0;

    for (auto &q : questions) {
        cout << Color::bold("Q" + to_string(q.id) + ": ") << q.prompt << "\n";
        if (q.type == MCQ) {
            for (int i = 0; i < 4; ++i)
                cout << " " << char('A' + i) << ") " << q.options[i] << "\n";
            cout << "Your answer (A-D): ";
            char ans;
            while (true) {
                string s; getline(cin, s);
                if (s.size() >= 1) { ans = toupper(s[0]); break; }
                cout << Color::red("Enter A/B/C/D: ");
            }
            if (ans == q.correct) {
                ++scoreMCQ; cout << Color::green("Correct!\n");
            } else {
                cout << Color::red("Incorrect. Correct: ") << q.correct << "\n";
                generateAlert(username, "Wrong Answer", 
                              "Incorrect answer to Q" + to_string(q.id), "Low");
            }
            ++mcqCount;
        } else {
            cout << "Your short answer:\n> ";
            string resp = inputLineTrimmed();
            if (resp.size() > 5) {
                ++scoreText;
                cout << Color::green("Answer recorded.\n");
            } else {
                cout << Color::yellow("Too short, review suggested.\n");
            }
            ++textCount;
        }
        maybeGenerateRandomAlert(username);
        cout << "-------------------------------------\n";
        pauseForMs(400);
    }

    user.endTime = steady_clock::now();
    int total = mcqCount + textCount;
    int obtained = scoreMCQ * 10 + scoreText * 10;
    double percent = total ? (obtained * 100.0 / (total * 10)) : 0;
    user.score = percent;

    cout << Color::cyan("\n=== Exam Completed ===\n");
    cout << "MCQ Correct: " << scoreMCQ << "/" << mcqCount << "\n";
    cout << "Text Accepted: " << scoreText << "/" << textCount << "\n";
    cout << Color::green("Final Score: " + to_string((int)percent) + "%\n");

    examAttemptedUsers.insert(username);
    showProctorSummary(username);
}

void studentMenu(const string &username) {
    while (true) {
        cout << Color::cyan("\n--- Student Panel ---\n");
        cout << "1) Take Exam\n2) View Score\n3) View Proctor Summary\n4) Logout\nChoice: ";
        int ch = readInt();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (ch == 1) studentTakeExam(username);
        else if (ch == 2)
            cout << Color::blue("Your score: ") << Color::green(to_string((int)users[username].score) + "%\n");
        else if (ch == 3) showProctorSummary(username);
        else if (ch == 4) { cout << Color::yellow("Logging out...\n"); break; }
        else cout << Color::red("Invalid option.\n");
    }
}

// ----------------- Registration & Login -----------------
void registerUser() {
    cout << Color::cyan("\n--- Register ---\n");
    cout << "Enter username: ";
    string uname; getline(cin, uname);
    if (users.find(uname) != users.end()) {
        cout << Color::red("Username already exists.\n");
        return;
    }
    cout << "Enter password: ";
    string pw; getline(cin, pw);
    cout << "Role (S)tudent or (F)aculty? ";
    char ch; cin >> ch; cin.ignore();
    string role = (toupper(ch) == 'F') ? "faculty" : "student";
    users[uname] = {uname, pw, role};
    cout << Color::green("Registered as " + role + ".\n");
}

bool loginUser(string &outUser, string &outRole) {
    cout << Color::cyan("\n--- Login ---\n");
    cout << "Username: ";
    string uname; getline(cin, uname);
    cout << "Password: ";
    string pw; getline(cin, pw);
    auto it = users.find(uname);
    if (it == users.end() || it->second.password != pw) {
        cout << Color::red("Invalid credentials.\n");
        return false;
    }
    outUser = uname;
    outRole = it->second.role;
    loginHistory.push(uname);
    cout << Color::green("Login successful as " + outRole + ".\n");
    return true;
}

// ----------------- Main -----------------
int main() {
    srand((unsigned)time(nullptr));
#ifdef _WIN32
    enableVirtualTerminalProcessing();
#endif
    system("cls || clear");
    printHeader();

    // Default faculty
    users["faculty1"] = {"faculty1", "admin", "faculty"};

    // Preload sample questions
    questions = {
        {1, MCQ, "Which data structure uses LIFO?", {"Queue", "Stack", "Tree", "Graph"}, 'B'},
        {2, MCQ, "Best average sorting complexity?", {"O(n²)", "O(n log n)", "O(n)", "O(1)"}, 'B'},
        {3, TEXT, "Explain polymorphism in one sentence.", {}, 0}
    };

    while (true) {
        cout << Color::blue("\nMain Menu:\n");
        cout << "1) Register\n2) Login\n3) View Proctor Alerts\n4) View Login History\n5) Exit\nChoice: ";
        int choice = readInt();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1) registerUser();
        else if (choice == 2) {
            string username, role;
            if (loginUser(username, role)) {
                if (role == "faculty") facultyMenu(username);
                else studentMenu(username);
            }
        }
        else if (choice == 3) showAllAlerts();
        else if (choice == 4) {
            if (loginHistory.empty()) {
                cout << Color::yellow("No login history.\n");
            } else {
                cout << Color::cyan("Recent Logins (LIFO):\n");
                stack<string> temp = loginHistory;
                while (!temp.empty()) {
                    cout << "  → " << temp.top() << "\n";
                    temp.pop();
                }
            }
        }
        else if (choice == 5) {
            cout << Color::green("Goodbye! Exam system shutting down.\n");
            break;
        }
        else cout << Color::red("Invalid choice.\n");
    }
    return 0;
}