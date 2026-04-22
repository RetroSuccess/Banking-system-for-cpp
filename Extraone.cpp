#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
using namespace std;

// ================= CONFIG =================
double SAVINGS_RATE = 0.05;
double CHEQUE_RATE = 0.02;
double FIXED_RATE = 0.08;
double STUDENT_RATE = 0.03;

// ================= ENCRYPT =================
string encrypt(string text) {
    for (int i = 0; i < text.length(); i++) {
        text[i] += 2;
    }
    return text;
}

// ================= ACCOUNT =================
class Account {
public:
    string accNumber, name, idNumber, phone, email, address, dob;
    string branchCode, pin, accountType;
    double balance;

    virtual double getInterestRate() {
        return SAVINGS_RATE;
    }

    virtual void applyInterest() {
        balance += balance * getInterestRate();
    }

    void display() {
        cout << "\nAccount: " << accNumber;
        cout << "\nName: " << name;
        cout << "\nType: " << accountType;
        cout << "\nBranch: " << branchCode;
        cout << "\nBalance: " << balance << endl;
    }
};

// ================= INHERITANCE FIX =================
class Savings : public Account {
public:
    double getInterestRate() override { return SAVINGS_RATE; }
};

class Cheque : public Account {
public:
    double getInterestRate() override { return CHEQUE_RATE; }
};

class FixedDeposit : public Account {
public:
    double getInterestRate() override { return FIXED_RATE; }
};

class Student : public Account {
public:
    double getInterestRate() override { return STUDENT_RATE; }
};

// ================= TELLER =================
class Teller {
public:
    string id, name, password, branch;
};

// ================= BRANCH =================
struct Branch {
    string code, name, location;
};

vector<Branch> branches = {
    {"001", "Cape Town", "Western Cape"},
    {"002", "Johannesburg", "Gauteng"},
    {"003", "Durban", "KZN"}
};

// ================= GENERATORS =================
string generateAccount(string branch) {
    static int num = 10000;
    return "ACC-" + branch + "-" + to_string(num++);
}

string generatePIN() {
    return to_string(rand() % 90000 + 10000);
}

// ================= VALIDATION =================
bool validID(string id) { return id.length() == 13; }
bool validPhone(string phone) { return phone.length() == 10; }
bool validEmail(string email) { return email.find('@') != string::npos; }

// ================= CONFIG FILE =================
void loadConfig() {
    ofstream file("system_config.txt");
    file << "SAVINGS=5%\nCHEQUE=2%\nFIXED=8%\nSTUDENT=3%\n";
    file.close();
}

// ================= FILE OPS =================
void saveTransaction(string text) {
    ofstream file("transactions.dat", ios::app);
    file << text << endl;
}

void updateAccount(Account updated) {
    fstream file("customers.dat", ios::in | ios::out | ios::binary);
    Account acc;

    while (file.read((char*)&acc, sizeof(acc))) {
        if (acc.accNumber == updated.accNumber) {
            file.seekp(-sizeof(acc), ios::cur);
            file.write((char*)&updated, sizeof(updated));
            break;
        }
    }
}

// ================= REGISTER =================
void registerCustomer(string branch) {
    Account acc;

    cout << "Name: "; cin >> acc.name;
    cout << "ID: "; cin >> acc.idNumber;
    if (!validID(acc.idNumber)) return;

    cout << "Phone: "; cin >> acc.phone;
    if (!validPhone(acc.phone)) return;

    cout << "Email: "; cin >> acc.email;
    if (!validEmail(acc.email)) return;

    cout << "Address: "; cin >> acc.address;
    cout << "DOB: "; cin >> acc.dob;

    cout << "Type: "; cin >> acc.accountType;
    cout << "Deposit: "; cin >> acc.balance;

    acc.branchCode = branch;
    acc.accNumber = generateAccount(branch);

    string rawPIN = generatePIN();
    acc.pin = encrypt(rawPIN);

    ofstream file("customers.dat", ios::binary | ios::app);
    file.write((char*)&acc, sizeof(acc));

    cout << "\nAccount Created: " << acc.accNumber;
    cout << "\nPIN: " << rawPIN << endl;
}

// ================= CUSTOMER LOGIN =================
bool customerLogin(Account &acc) {
    string num, pin;
    int tries = 0;

    cout << "Account: ";
    cin >> num;

    while (tries < 3) {
        cout << "PIN: ";
        cin >> pin;

        ifstream file("customers.dat", ios::binary);

        while (file.read((char*)&acc, sizeof(acc))) {
            if (acc.accNumber == num && acc.pin == encrypt(pin)) {
                return true;
            }
        }

        tries++;
    }

    return false;
}

// ================= BINARY SEARCH (NEW) =================
bool binarySearch(string target) {
    ifstream file("customers.dat", ios::binary);

    vector<Account> list;
    Account acc;

    while (file.read((char*)&acc, sizeof(acc))) {
        list.push_back(acc);
    }

    int left = 0, right = list.size() - 1;

    while (left <= right) {
        int mid = (left + right) / 2;

        if (list[mid].accNumber == target) {
            list[mid].display();
            return true;
        }

        if (list[mid].accNumber < target)
            left = mid + 1;
        else
            right = mid - 1;
    }

    return false;
}

// ================= SEARCH =================
void searchCustomer() {
    string num;
    cout << "Account Number: ";
    cin >> num;

    if (!binarySearch(num)) {
        cout << "Not found (binary search)\n";
    }
}

// ================= REPORTS =================
void branchComparison() {
    cout << "\n--- BRANCH COMPARISON ---\n";

    for (int i = 0; i < branches.size(); i++) {
        double total = 0;

        Account acc;
        ifstream file("customers.dat", ios::binary);

        while (file.read((char*)&acc, sizeof(acc))) {
            if (acc.branchCode == branches[i].code) {
                total += acc.balance;
            }
        }

        cout << branches[i].name << " Total: " << total << endl;
    }
}

// ================= INTEREST =================
void applyInterestAll() {
    fstream file("customers.dat", ios::in | ios::out | ios::binary);
    Account acc;

    while (file.read((char*)&acc, sizeof(acc))) {
        acc.applyInterest();

        file.seekp(-sizeof(acc), ios::cur);
        file.write((char*)&acc, sizeof(acc));
    }
}

// ================= MAIN =================
int main() {
    loadConfig();

    int choice;

    do {
        cout << "\n1.Teller\n2.Customer\n3.Search\n4.Branch Report\n5.Exit\n";
        cin >> choice;

        if (choice == 2) {
            Account acc;

            if (customerLogin(acc)) {
                int c;

                do {
                    cout << "\n1.Balance\n2.Deposit\n3.Withdraw\n4.Transfer\n5.Exit\n";
                    cin >> c;

                    if (c == 1) acc.display();
                } while (c != 5);
            }
        }

        if (choice == 3) searchCustomer();
        if (choice == 4) branchComparison();

    } while (choice != 5);

    return 0;
}
