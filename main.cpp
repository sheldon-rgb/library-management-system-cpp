// Library Management System with File I/O
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <limits>
#include <sstream>

using namespace std;

// ======================= Utility =======================
string toLowerCase(const string& s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// ======================= Book Class =======================
class Book {
public:
    int id;
    string Title, Author, Type;
    bool available;

    Book(int id, string t, string a, string g)
        : id(id), Title(t), Author(a), Type(g), available(true) {}

    void display() const {
        cout << id << "\t" << Title << "\t" << Author << "\t" << Type << "\t" << (available ? "Yes" : "No") << endl;
    }

    string toFileString() const {
        return to_string(id) + "|" + Title + "|" + Author + "|" + Type + "|" + (available ? "1" : "0");
    }
};

// ======================= Library Class =======================
class Library {
    vector<Book> books;
    int nextBookId = 1;

public:
    void addBook() {
        string title, author, genre;
        cout << "Enter book title: ";
        getline(cin, title);
        cout << "Enter author: ";
        getline(cin, author);
        cout << "Enter Type: ";
        getline(cin, genre);
        books.emplace_back(nextBookId++, title, author, genre);
        cout << "Book added successfully.\n";
    }

    void removeBook() {
        int id;
        cout << "Enter book ID to remove: ";
        cin >> id;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        auto it = remove_if(books.begin(), books.end(), [id](Book& b) { return b.id == id; });
        if (it != books.end()) {
            books.erase(it, books.end());
            cout << "Book removed.\n";
        } else {
            cout << "Book ID not found.\n";
        }
    }

    void listBooks() const {
        if (books.empty()) {
            cout << "No books in library.\n";
            return;
        }
        cout << "ID\tTitle\tAuthor\tType\tAvailable\n";
        for (const auto& b : books) b.display();
    }

    Book* borrowBook(int id) {
        for (auto& b : books) {
            if (b.id == id && b.available) {
                b.available = false;
                return &b;
            }
        }
        return nullptr;
    }

    void returnBook(int id) {
        for (auto& b : books) {
            if (b.id == id) {
                b.available = true;
                return;
            }
        }
    }

    void loadBooks(const string& filename) {
        ifstream file(filename);
        if (!file) return;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string idStr, title, author, type, availStr;
            getline(ss, idStr, '|');
            getline(ss, title, '|');
            getline(ss, author, '|');
            getline(ss, type, '|');
            getline(ss, availStr, '|');
            int id = stoi(idStr);
            bool available = (availStr == "1");
            books.emplace_back(id, title, author, type);
            books.back().available = available;
            if (id >= nextBookId) nextBookId = id + 1;
        }
    }

    void saveBooks(const string& filename) const {
        ofstream file(filename);
        for (const auto& b : books) {
            file << b.toFileString() << endl;
        }
    }
};

// ======================= Base User Class =======================
class User {
protected:
    string username, password, email, phone;

public:
    User(string uname, string pass, string mail, string ph)
        : username(uname), password(pass), email(mail), phone(ph) {}

    virtual ~User() {}
    string getUsername() const { return username; }
    bool checkPassword(const string& pass) const { return password == pass; }

    virtual string getRole() const = 0;
    virtual void showMenu() = 0;
    virtual string toFileString() const = 0;
};

// ======================= Admin Class =======================
class Admin : public User {
    Library* library;

public:
    Admin(string uname, string pass, string mail, string ph, Library* lib)
        : User(uname, pass, mail, ph), library(lib) {}

    string getRole() const override { return "Admin"; }

    void showMenu() override {
        int choice;
        do {
            cout << "\n[===Admin Menu===] \n1. Add Book \n2. Remove Book \n3. View Books \n4. Logout\nChoice: ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            switch (choice) {
                case 1: library->addBook(); break;
                case 2: library->removeBook(); break;
                case 3: library->listBooks(); break;
                case 4: cout << "Logging out...\n"; break;
                default: cout << "Invalid choice.\n";
            }
        } while (choice != 4);
    }

    string toFileString() const override {
        return "admin|" + username + "|" + password + "|" + email + "|" + phone;
    }
};

// ======================= Student Class =======================
class Student : public User {
    Library* library;
    int borrowedBookId = -1;
    string department;

public:
    Student(string uname, string pass, string mail, string ph, string dept, Library* lib)
        : User(uname, pass, mail, ph), department(dept), library(lib) {}

    string getRole() const override { return "Student"; }

    string getDepartment() const { return department; }

    void showMenu() override {
        int choice;
        do {
            cout << "\n[===User Menu===] \n1. View Books \n2. Borrow Book \n3. Return Book \n4. Logout\nChoice: ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            switch (choice) {
                case 1: library->listBooks(); break;
                case 2: {
                    if (borrowedBookId != -1) {
                        cout << "You already borrowed a book.\n";
                        break;
                    }
                    int id;
                    cout << "Enter book ID to borrow: ";
                    cin >> id;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    Book* b = library->borrowBook(id);
                    if (b) {
                        borrowedBookId = id;
                        cout << "Book borrowed: " << b->Title << "\n";
                    } else {
                        cout << "Book not available.\n";
                    }
                    break;
                }
                case 3:
                    if (borrowedBookId == -1) {
                        cout << "No book to return.\n";
                    } else {
                        library->returnBook(borrowedBookId);
                        borrowedBookId = -1;
                        cout << "Book returned.\n";
                    }
                    break;
                case 4: cout << "Logging out...\n"; break;
                default: cout << "Invalid choice.\n";
            }
        } while (choice != 4);
    }

    string toFileString() const override {
        return "student|" + username + "|" + password + "|" + email + "|" + phone + "|" + department;
    }
};

// ======================= System Controller =======================
class System {
    vector<User*> users;
    Library library;

    bool isValidEmail(const string& email) {
        return email.find('@') != string::npos && email.find(".com") != string::npos;
    }

    bool isValidPhone(const string& phone) {
        return phone.length() == 11 && all_of(phone.begin(), phone.end(), ::isdigit);
    }

public:
    void loadUsers(const string& filename) {
        ifstream file(filename);
        if (!file) return;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string role, uname, pass, mail, phone, dept;
            getline(ss, role, '|');
            getline(ss, uname, '|');
            getline(ss, pass, '|');
            getline(ss, mail, '|');
            getline(ss, phone, '|');
            if (role == "student") {
                getline(ss, dept, '|');
                users.push_back(new Student(uname, pass, mail, phone, dept, &library));
            } else if (role == "admin") {
                users.push_back(new Admin(uname, pass, mail, phone, &library));
            }
        }
    }

    void saveUsers(const string& filename) const {
        ofstream file(filename);
        for (const auto& u : users) {
            file << u->toFileString() << endl;
        }
    }

    void mainMenu() {
        library.loadBooks("books.txt");
        loadUsers("users.txt");

        int choice;
        do {
            cout << "\n==== Library Management System Menu ====\n";
            cout << "1. Register Admin\n2. Admin Login\n3. Register User\n4. User Login\n5. Exit\nEnter choice: ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            string username, password, email, phone, department;

            if (choice == 1 || choice == 3) {
                cout << "Enter username: ";
                getline(cin, username);
                cout << "Enter password: ";
                getline(cin, password);

                while (true) {
                    cout << "Enter email: ";
                    getline(cin, email);
                    if (isValidEmail(email)) break;
                    cout << "Invalid email. Must include '@' and end in '.com'\n";
                }

                while (true) {
                    cout << "Enter phone number (11 digits): ";
                    getline(cin, phone);
                    if (isValidPhone(phone)) break;
                    cout << "Invalid phone. Must be 11 digits, numeric only.\n";
                }

                username = toLowerCase(username);

                if (choice == 3) {
                    cout << "Enter department: ";
                    getline(cin, department);
                    department = toLowerCase(department);
                    users.push_back(new Student(username, password, email, phone, department, &library));
                } else {
                    users.push_back(new Admin(username, password, email, phone, &library));
                }

                cout << "Registered successfully.\n";

            } else if (choice == 2 || choice == 4) {
                cout << "Enter username: ";
                getline(cin, username);
                cout << "Enter password: ";
                getline(cin, password);

                username = toLowerCase(username);

                if (choice == 2) {
                    bool found = false;
                    for (auto user : users) {
                        if (user->getUsername() == username && user->checkPassword(password)
                            && user->getRole() == "Admin") {
                            cout << "Login successful!\n";
                            user->showMenu();
                            found = true;
                            break;
                        }
                    }
                    if (!found) cout << "Invalid admin credentials.\n";
                } else {
                    cout << "Enter department: ";
                    getline(cin, department);
                    department = toLowerCase(department);

                    bool found = false;
                    for (auto user : users) {
                        if (user->getRole() == "Student" && user->getUsername() == username &&
                            user->checkPassword(password)) {
                            Student* s = dynamic_cast<Student*>(user);
                            if (s && s->getDepartment() == department) {
                                cout << "Login successful!\n";
                                s->showMenu();
                                found = true;
                                break;
                            }
                        }
                    }
                    if (!found) cout << "Invalid student credentials or department.\n";
                }
            }

        } while (choice != 5);

        library.saveBooks("books.txt");
        saveUsers("users.txt");
    }

    ~System() {
        for (auto u : users) delete u;
    }
};

int main() {
    System sys;
    sys.mainMenu();
    return 0;
}