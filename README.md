🧠 Exam Proctoring System (C++ - In-Memory)

An offline C++-based online exam proctoring system that simulates a real-time, AI-driven proctoring environment — all in a single file and without external dependencies.

It includes:

👨‍🏫 Faculty panel for managing questions and viewing alerts

🧑‍🎓 Student panel for taking exams

⚠️ Proctor alerts for suspicious behavior and wrong answers

⏱️ Timers, summaries, and severity-based verdicts

🚀 Features
👩‍🏫 Faculty Features

Add or view MCQ and short-answer questions

View all active proctor alerts (with severity levels)

Monitor suspicious or incorrect student behavior in real time

👨‍🎓 Student Features

Register and log in as a student

Take an exam with MCQs and text questions

Get live feedback and a detailed summary after the test

Triggers proctor alerts automatically when:

Wrong answers are given

Random simulated behaviors occur (window switch, multiple faces, etc.)

Exam is completed suspiciously fast

⚙️ System Features

In-memory data storage (no files or databases needed)

Colored CLI interface with Windows-compatible ANSI support

FIFO queue for alerts and LIFO stack for login history

Time-based verdict classification (Clear / Warning / Review Required)

🧩 Data Structures Used
Data Structure	Purpose
map<string, User>	Stores all user details (username, password, role)
vector<Question>	Stores exam questions dynamically
queue<Alert>	Maintains Proctor Alerts in FIFO order
stack<string>	Stores login history (LIFO order)
set<string>	Tracks users who have already attempted the exam
🖥️ How It Works
🔹 Main Menu

Register or log in (Student / Faculty)

Faculty can:

Add questions

View questions

Monitor alerts

Student can:

Take exam

View score and summary

Review proctoring report

⚠️ Proctor Alerts

Alerts are auto-generated under these conditions:

Type	Trigger	Severity
Wrong Answer	Student gives incorrect answer	Low
Window Switch	Random simulated event	Medium
Attention Drop	Random simulated event	Medium
Multiple Faces	Simulated event	High
Fast Completion	Exam finished suspiciously quickly	Medium/High

Each alert contains:

TYPE             MESSAGE                                         SEVERITY
Wrong Answer     [student] Incorrect answer to Q2                Low
Multiple Faces   [student] Multiple faces detected (simulated)   High

📋 Verdict Summary

At the end of the exam, the system provides an automatic severity-based report:

Condition	Verdict
High severity alerts > 0	Manual Review Required
Medium alerts > 1 or multiple wrong answers	Needs Caution
Otherwise	Clear
🛠️ Compilation & Run
Windows / Linux (g++)
g++ main.cpp -o exam_proctoring
./exam_proctoring


If using Windows and colors don’t show properly, make sure ANSI escape codes are enabled by running the terminal as Administrator.

🧪 Sample Credentials
Role	Username	Password
Faculty	faculty1	admin
📚 Example Flow
Main Menu:
1) Register
2) Login
3) View Proctor Alerts
4) View Login History
5) Exit


Once a student logs in:

Q1: Which data structure uses LIFO?
A) Queue  B) Stack  C) Tree  D) Graph
Your answer: A
Incorrect. Correct: B
⚠ [Proctor Alert] Wrong answer to Q1 (Low)

🏁 Future Enhancements

Webcam and mic input for real proctoring

File export for alert logs and summaries

Graphical dashboard using C++ GUI frameworks

👨‍💻 Author

Atharva Koranne
💡 GitHub: kingak762006
