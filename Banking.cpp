#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <sys/stat.h>
#include <direct.h>
using namespace std;

// ==================== ENCRYPTION UTILITIES ====================
string encrypt(string text) {
    string result = "";
    for (int i = 0; i < text.length(); i++) {
        result += text[i] + 3;
    }
    return result;
}

string decrypt(string text) {
    string result = "";
    for (int i = 0; i < text.length(); i++) {
        result += text[i] - 3;
    }
    return result;
}

string getCurrentTime() {
    time_t now = time(0);
    string dt = ctime(&now);
    dt.pop_back();
    return dt;
}

string getCurrentDate() {
    time_t now = time(0);
    char* dt = ctime(&now);
    string date(dt);
    return date.substr(0, 10);
}

// ==================== BRANCH STRUCT ====================
struct Branch {
    char code[10];
    char name[100];
    char location[200];
    int customerCount;
    double totalBalance;
};

// ==================== ACCOUNT HIERARCHY ====================
class Account {
protected:
    char accNumber[30];
    char name[100];
    char idNumber[20];
    char phone[15];
    char email[100];
    char address[200];
    char dob[15];
    char branchCode[10];
    char pin[50];  // encrypted
    double balance;
    bool locked;
    int failedAttempts;

public:
    Account() {
        balance = 0;
        locked = false;
        failedAttempts = 0;
        memset(accNumber, 0, 30);
        memset(name, 0, 100);
        memset(idNumber, 0, 20);
        memset(phone, 0, 15);
        memset(email, 0, 100);
        memset(address, 0, 200);
        memset(dob, 0, 15);
        memset(branchCode, 0, 10);
        memset(pin, 0, 50);
    }

    virtual double getMinBalance() = 0;
    virtual double getInterestRate() = 0;
    virtual string getAccountType() = 0;

    void setAccNumber(string num) { strcpy(accNumber, num.c_str()); }
    string getAccNumber() { return string(accNumber); }

    void setName(string n) { strcpy(name, n.c_str()); }
    string getName() { return string(name); }

    void setIdNumber(string id) { strcpy(idNumber, id.c_str()); }
    string getIdNumber() { return string(idNumber); }

    void setPhone(string p) { strcpy(phone, p.c_str()); }
    string getPhone() { return string(phone); }

    void setEmail(string e) { strcpy(email, e.c_str()); }
    string getEmail() { return string(email); }

    void setAddress(string a) { strcpy(address, a.c_str()); }
    string getAddress() { return string(address); }

    void setDob(string d) { strcpy(dob, d.c_str()); }
    string getDob() { return string(dob); }

    void setBranchCode(string bc) { strcpy(branchCode, bc.c_str()); }
    string getBranchCode() { return string(branchCode); }

    void setPin(string p) { strcpy(pin, encrypt(p).c_str()); }
    string getPin() { return string(pin); }

    void setBalance(double b) { balance = b; }
    double getBalance() { return balance; }

    void setLocked(bool l) { locked = l; }
    bool isLocked() { return locked; }

    void setFailedAttempts(int fa) { failedAttempts = fa; }
    int getFailedAttempts() { return failedAttempts; }

    bool verifyPin(string inputPin) {
        if (locked) return false;
        if (encrypt(inputPin) == string(pin)) {
            failedAttempts = 0;
            return true;
        }
        failedAttempts++;
        if (failedAttempts >= 3) locked = true;
        return false;
    }

    void changePin(string oldPin, string newPin) {
        if (verifyPin(oldPin) && newPin.length() == 5) {
            setPin(newPin);
        }
    }

    virtual void deposit(double amount) {
        if (amount > 0) balance += amount;
    }

    virtual bool withdraw(double amount) {
        if (amount > 0 && balance - amount >= getMinBalance()) {
            balance -= amount;
            return true;
        }
        return false;
    }

    virtual void display() {
        cout << "Account: " << accNumber << endl;
        cout << "Name: " << name << endl;
        cout << "Type: " << getAccountType() << endl;
        cout << "Balance: R" << fixed << setprecision(2) << balance << endl;
    }
};

class SavingsAccount : public Account {
public:
    double getMinBalance() { return 1000; }
    double getInterestRate() { return 0.045; }
    string getAccountType() { return "Savings"; }
};

class ChequeAccount : public Account {
public:
    double getMinBalance() { return 5000; }
    double getInterestRate() { return 0.02; }
    string getAccountType() { return "Cheque"; }
};

class FixedDepositAccount : public Account {
private:
    int maturityMonths;
public:
    FixedDepositAccount() : maturityMonths(12) {}

    double getMinBalance() { return 10000; }
    double getInterestRate() { return 0.075; }
    string getAccountType() { return "Fixed Deposit"; }

    void setMaturityMonths(int months) { maturityMonths = months; }
    int getMaturityMonths() { return maturityMonths; }
};

class StudentAccount : public Account {
public:
    double getMinBalance() { return 0; }
    double getInterestRate() { return 0.01; }
    string getAccountType() { return "Student"; }
};

// ==================== TELLER CLASS ====================
class Teller {
public:
    char id[10];
    char name[100];
    char password[50];
    char branchCode[10];
    int failedAttempts;
    bool locked;

    Teller() {
        failedAttempts = 0;
        locked = false;
        memset(id, 0, 10);
        memset(name, 0, 100);
        memset(password, 0, 50);
        memset(branchCode, 0, 10);
    }

    bool login() {
        if (locked) return false;

        string inputId, inputPass;
        cout << "Teller ID: ";
        cin >> inputId;
        cout << "Password: ";
        cin >> inputPass;

        ifstream file("tellers.dat", ios::binary);
        if (!file.is_open()) return false;

        Teller t;
        while (file.read(reinterpret_cast<char*>(&t), sizeof(Teller))) {
            if (inputId == string(t.id) && encrypt(inputPass) == string(t.password)) {
                *this = t;
                cout << "Login successful! Welcome " << t.name << endl;
                file.close();
                return true;
            }
        }
        file.close();

        failedAttempts++;
        if (failedAttempts >= 3) locked = true;
        cout << "Login failed!\n";
        return false;
    }
};

// ==================== TRANSACTION STRUCT ====================
struct Transaction {
    char accNumber[30];
    char type[20];
    double amount;
    char dateTime[50];
};

// ==================== GLOBAL VARIABLES ====================
vector<Account*> accounts;
vector<Teller> tellers;
vector<Branch> branches;
vector<Transaction> transactions;

// ==================== FUNCTION PROTOTYPES ====================
void loadData();
void saveData();
void setupBranches();
void backupData();
void restoreData();
void exportToCSV();
void exportToText();
void calculateInterest();
void searchCustomers();
void tellerMenu(Teller& teller);
void customerMenu(Account* customer);
void registerCustomer(Teller& teller);
void viewCustomer();
void processTransaction(Teller& teller);
void branchReport(Teller& teller);
void viewBranches();
void interBranchCompare();
void dailyReport();
void customerSummary();
void branchPerformance();
Account* findAccount(string accNum);
void addTransaction(string accNum, string type, double amount);
void showStatement(Account* acc);

// ==================== MAIN FUNCTION ====================
int main() {
    setupBranches();
    loadData();

    int choice;
    do {
        cout << "\n========================================\n";
        cout << "     MULTI-BRANCH BANKING SYSTEM       \n";
        cout << "========================================\n";
        cout << "1. Teller Login\n";
        cout << "2. Customer Login\n";
        cout << "3. View All Branches\n";
        cout << "4. Backup Data\n";
        cout << "5. Restore Data\n";
        cout << "6. Export Reports\n";
        cout << "7. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        try {
            if (choice == 1) {
                Teller teller;
                if (teller.login()) {
                    tellerMenu(teller);
                }
            }
            else if (choice == 2) {
                string accNum, pin;
                cout << "Account Number: ";
                cin >> accNum;
                cout << "PIN: ";
                cin >> pin;

                Account* customer = findAccount(accNum);
                if (customer && customer->verifyPin(pin)) {
                    customerMenu(customer);
                }
                else {
                    cout << "Invalid credentials!\n";
                }
            }
            else if (choice == 3) {
                viewBranches();
            }
            else if (choice == 4) {
                backupData();
            }
            else if (choice == 5) {
                restoreData();
            }
            else if (choice == 6) {
                int expChoice;
                cout << "1. Export to CSV\n2. Export to Text\nChoice: ";
                cin >> expChoice;
                if (expChoice == 1) exportToCSV();
                else exportToText();
            }
        }
        catch (exception& e) {
            cout << "Error: " << e.what() << endl;
        }

    } while (choice != 7);

    saveData();

    for (Account* a : accounts) delete a;

    cout << "\nPress Enter to exit...";
    cin.ignore();
    cin.get();

    return 0;
}

// ==================== SETUP BRANCHES ====================
void setupBranches() {
    Branch b1 = { "001", "Johannesburg Main", "100 Main Street, JHB", 0, 0 };
    Branch b2 = { "002", "Cape Town Central", "50 Long Street, CT", 0, 0 };
    Branch b3 = { "003", "Durban North", "75 Florida Road, DBN", 0, 0 };
    branches.push_back(b1);
    branches.push_back(b2);
    branches.push_back(b3);
}

// ==================== BINARY FILE HANDLING ====================
void loadData() {
    // Load tellers
    ifstream tFile("tellers.dat", ios::binary);
    if (tFile.is_open()) {
        Teller t;
        while (tFile.read(reinterpret_cast<char*>(&t), sizeof(Teller))) {
            tellers.push_back(t);
        }
        tFile.close();
    }
    else {
        Teller defaultTeller;
        strcpy(defaultTeller.id, "T001");
        strcpy(defaultTeller.name, "Admin Teller");
        strcpy(defaultTeller.password, encrypt("admin123").c_str());
        strcpy(defaultTeller.branchCode, "001");
        tellers.push_back(defaultTeller);
        saveData();
    }

    // Load accounts
    ifstream aFile("customers.dat", ios::binary);
    if (aFile.is_open()) {
        char type[20];
        while (aFile.read(type, 20)) {
            Account* acc = nullptr;
            string accType(type);

            if (accType == "Savings") acc = new SavingsAccount();
            else if (accType == "Cheque") acc = new ChequeAccount();
            else if (accType == "Fixed Deposit") acc = new FixedDepositAccount();
            else if (accType == "Student") acc = new StudentAccount();

            if (acc) {
                aFile.read(reinterpret_cast<char*>(acc), sizeof(Account) - 20);
                accounts.push_back(acc);
            }
        }
        aFile.close();
    }

    // Load branches
    ifstream bFile("branches.dat", ios::binary);
    if (bFile.is_open()) {
        Branch b;
        while (bFile.read(reinterpret_cast<char*>(&b), sizeof(Branch))) {
            // Update branches instead of loading
        }
        bFile.close();
    }
    else {
        ofstream outB("branches.dat", ios::binary);
        for (Branch b : branches) {
            outB.write(reinterpret_cast<char*>(&b), sizeof(Branch));
        }
        outB.close();
    }

    // Load transactions
    ifstream trFile("transactions.dat", ios::binary);
    if (trFile.is_open()) {
        Transaction t;
        while (trFile.read(reinterpret_cast<char*>(&t), sizeof(Transaction))) {
            transactions.push_back(t);
        }
        trFile.close();
    }
}

void saveData() {
    ofstream tFile("tellers.dat", ios::binary);
    for (Teller t : tellers) {
        tFile.write(reinterpret_cast<char*>(&t), sizeof(Teller));
    }
    tFile.close();

    ofstream aFile("customers.dat", ios::binary);
    for (Account* acc : accounts) {
        string type = acc->getAccountType();
        aFile.write(type.c_str(), 20);
        aFile.write(reinterpret_cast<char*>(acc), sizeof(Account) - 20);
    }
    aFile.close();

    ofstream bFile("branches.dat", ios::binary);
    for (Branch b : branches) {
        bFile.write(reinterpret_cast<char*>(&b), sizeof(Branch));
    }
    bFile.close();

    ofstream trFile("transactions.dat", ios::binary);
    for (Transaction t : transactions) {
        trFile.write(reinterpret_cast<char*>(&t), sizeof(Transaction));
    }
    trFile.close();

    ofstream config("system_config.txt");
    config << "BANKING_SYSTEM_V2.0\n";
    config << "LastBackup: " << getCurrentTime() << "\n";
    config << "TotalAccounts: " << accounts.size() << "\n";
    config.close();
}

// ==================== BACKUP & RECOVERY ====================
void backupData() {
    string backupDir = "backup_" + getCurrentDate();
    _mkdir(backupDir.c_str());

    ifstream src1("tellers.dat", ios::binary);
    ofstream dst1(backupDir + "/tellers.dat", ios::binary);
    dst1 << src1.rdbuf();
    src1.close(); dst1.close();

    ifstream src2("customers.dat", ios::binary);
    ofstream dst2(backupDir + "/customers.dat", ios::binary);
    dst2 << src2.rdbuf();
    src2.close(); dst2.close();

    ifstream src3("branches.dat", ios::binary);
    ofstream dst3(backupDir + "/branches.dat", ios::binary);
    dst3 << src3.rdbuf();
    src3.close(); dst3.close();

    ifstream src4("transactions.dat", ios::binary);
    ofstream dst4(backupDir + "/transactions.dat", ios::binary);
    dst4 << src4.rdbuf();
    src4.close(); dst4.close();

    ifstream src5("system_config.txt");
    ofstream dst5(backupDir + "/system_config.txt");
    dst5 << src5.rdbuf();
    src5.close(); dst5.close();

    cout << "Backup created in: " << backupDir << endl;
}

void restoreData() {
    string backupDir;
    cout << "Enter backup directory name: ";
    cin >> backupDir;

    ifstream src1(backupDir + "/tellers.dat", ios::binary);
    ofstream dst1("tellers.dat", ios::binary);
    dst1 << src1.rdbuf();
    src1.close(); dst1.close();

    ifstream src2(backupDir + "/customers.dat", ios::binary);
    ofstream dst2("customers.dat", ios::binary);
    dst2 << src2.rdbuf();
    src2.close(); dst2.close();

    cout << "Data restored successfully! Please restart the application.\n";
}

// ==================== EXPORT FUNCTIONS ====================
void exportToCSV() {
    ofstream csv("customers_export.csv");
    csv << "Account Number,Name,ID Number,Phone,Email,Type,Balance,Branch\n";
    for (Account* acc : accounts) {
        csv << acc->getAccNumber() << ","
            << acc->getName() << ","
            << acc->getIdNumber() << ","
            << acc->getPhone() << ","
            << acc->getEmail() << ","
            << acc->getAccountType() << ","
            << acc->getBalance() << ","
            << acc->getBranchCode() << "\n";
    }
    csv.close();

    ofstream transCSV("transactions_export.csv");
    transCSV << "Account Number,Type,Amount,Date/Time\n";
    for (Transaction t : transactions) {
        transCSV << t.accNumber << ","
            << t.type << ","
            << t.amount << ","
            << t.dateTime << "\n";
    }
    transCSV.close();

    cout << "Exported to customers_export.csv and transactions_export.csv\n";
}

void exportToText() {
    ofstream txt("bank_report.txt");
    txt << "========== BANK MANAGEMENT REPORT ==========\n";
    txt << "Generated: " << getCurrentTime() << "\n\n";

    txt << "=== CUSTOMER SUMMARY ===\n";
    for (Account* acc : accounts) {
        txt << "Account: " << acc->getAccNumber() << "\n";
        txt << "Name: " << acc->getName() << "\n";
        txt << "Type: " << acc->getAccountType() << "\n";
        txt << "Balance: R" << fixed << setprecision(2) << acc->getBalance() << "\n";
        txt << "-----------------------------------\n";
    }
    txt.close();
    cout << "Exported to bank_report.txt\n";
}

// ==================== INTEREST CALCULATION ====================
void calculateInterest() {
    double totalInterest = 0;
    for (Account* acc : accounts) {
        double interest = acc->getBalance() * acc->getInterestRate();
        totalInterest += interest;
        cout << acc->getAccountType() << " account " << acc->getAccNumber()
            << " earned R" << fixed << setprecision(2) << interest << " interest\n";
    }
    cout << "Total interest accrued: R" << totalInterest << endl;
}

// ==================== SEARCH FUNCTIONALITY ====================
void searchCustomers() {
    int searchChoice;
    cout << "Search by:\n1. Account Number\n2. Name\n3. ID Number\n4. Phone\nChoice: ";
    cin >> searchChoice;

    string keyword;
    cout << "Enter search term: ";
    cin.ignore();
    getline(cin, keyword);

    vector<Account*> results;

    for (Account* acc : accounts) {
        switch (searchChoice) {
        case 1:
            if (acc->getAccNumber().find(keyword) != string::npos)
                results.push_back(acc);
            break;
        case 2:
            if (acc->getName().find(keyword) != string::npos)
                results.push_back(acc);
            break;
        case 3:
            if (acc->getIdNumber() == keyword)
                results.push_back(acc);
            break;
        case 4:
            if (acc->getPhone() == keyword)
                results.push_back(acc);
            break;
        }
    }

    cout << "\n=== SEARCH RESULTS (" << results.size() << " found) ===\n";
    for (Account* acc : results) {
        acc->display();
        cout << "------------------------\n";
    }
}

// ==================== FIND ACCOUNT ====================
Account* findAccount(string accNum) {
    for (Account* acc : accounts) {
        if (acc->getAccNumber() == accNum) {
            return acc;
        }
    }
    return nullptr;
}

// ==================== ADD TRANSACTION ====================
void addTransaction(string accNum, string type, double amount) {
    Transaction t;
    strcpy(t.accNumber, accNum.c_str());
    strcpy(t.type, type.c_str());
    t.amount = amount;
    strcpy(t.dateTime, getCurrentTime().c_str());
    transactions.push_back(t);
}

// ==================== SHOW STATEMENT ====================
void showStatement(Account* acc) {
    cout << "\n=== STATEMENT FOR " << acc->getAccNumber() << " ===\n";
    cout << "Name: " << acc->getName() << endl;
    cout << "Type: " << acc->getAccountType() << endl;
    cout << "Balance: R" << fixed << setprecision(2) << acc->getBalance() << endl;
    cout << "\nRecent Transactions:\n";

    int count = 0;
    for (int i = transactions.size() - 1; i >= 0 && count < 10; i--) {
        if (string(transactions[i].accNumber) == acc->getAccNumber()) {
            cout << transactions[i].type << " R" << transactions[i].amount
                << " - " << transactions[i].dateTime << endl;
            count++;
        }
    }
    cout << "========================\n";
}

// ==================== TELLER MENU ====================
void tellerMenu(Teller& teller) {
    int choice;
    do {
        cout << "\n===== TELLER MENU =====\n";
        cout << "1. Register Customer\n";
        cout << "2. View Customer\n";
        cout << "3. Process Transaction\n";
        cout << "4. Branch Report\n";
        cout << "5. View Branches\n";
        cout << "6. Compare Branches\n";
        cout << "7. Daily Report\n";
        cout << "8. Customer Summary\n";
        cout << "9. Branch Performance\n";
        cout << "10. Calculate Interest\n";
        cout << "11. Search Customers\n";
        cout << "12. Logout\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
        case 1: registerCustomer(teller); break;
        case 2: viewCustomer(); break;
        case 3: processTransaction(teller); break;
        case 4: branchReport(teller); break;
        case 5: viewBranches(); break;
        case 6: interBranchCompare(); break;
        case 7: dailyReport(); break;
        case 8: customerSummary(); break;
        case 9: branchPerformance(); break;
        case 10: calculateInterest(); break;
        case 11: searchCustomers(); break;
        }
    } while (choice != 12);
}

// ==================== REGISTER CUSTOMER ====================
void registerCustomer(Teller& teller) {
    Account* newAcc = nullptr;
    int typeChoice;

    cout << "\n=== REGISTER NEW CUSTOMER ===\n";
    cout << "Account Type:\n";
    cout << "1. Savings (Min R1000, 4.5% interest)\n";
    cout << "2. Cheque (Min R5000, 2% interest)\n";
    cout << "3. Fixed Deposit (Min R10000, 7.5% interest)\n";
    cout << "4. Student (Min R0, 1% interest)\n";
    cout << "Choice: ";
    cin >> typeChoice;

    switch (typeChoice) {
    case 1: newAcc = new SavingsAccount(); break;
    case 2: newAcc = new ChequeAccount(); break;
    case 3: newAcc = new FixedDepositAccount(); break;
    case 4: newAcc = new StudentAccount(); break;
    default: return;
    }

    static int counter = 10000;
    string accNum = "ACC-" + string(teller.branchCode) + "-" + to_string(counter++);
    newAcc->setAccNumber(accNum);

    string input;
    cout << "Full Name: ";
    cin.ignore();
    getline(cin, input);
    newAcc->setName(input);

    do {
        cout << "SA ID Number (13 digits): ";
        cin >> input;
        if (input.length() != 13) cout << "ID must be 13 digits!\n";
    } while (input.length() != 13);
    newAcc->setIdNumber(input);

    cout << "Phone (10 digits): ";
    cin >> input;
    newAcc->setPhone(input);

    do {
        cout << "Email: ";
        cin >> input;
        if (input.find('@') == string::npos) cout << "Invalid email!\n";
    } while (input.find('@') == string::npos);
    newAcc->setEmail(input);

    cout << "Address: ";
    cin.ignore();
    getline(cin, input);
    newAcc->setAddress(input);

    cout << "Date of Birth (DD/MM/YYYY): ";
    cin >> input;
    newAcc->setDob(input);

    double deposit;
    do {
        cout << "Initial Deposit (Min R" << newAcc->getMinBalance() << "): R";
        cin >> deposit;
        if (deposit < newAcc->getMinBalance()) {
            cout << "Amount too low!\n";
        }
    } while (deposit < newAcc->getMinBalance());

    newAcc->setBalance(deposit);
    newAcc->setBranchCode(teller.branchCode);

    srand(time(0));
    string pinCode = to_string(10000 + rand() % 90000);
    newAcc->setPin(pinCode);

    accounts.push_back(newAcc);

    cout << "\n✅ Account Created Successfully!\n";
    cout << "Account Number: " << accNum << endl;
    cout << "PIN: " << pinCode << " (SAVE THIS!)\n";
    cout << "Initial Balance: R" << fixed << setprecision(2) << deposit << endl;

    addTransaction(accNum, "OPENING", deposit);
}

// ==================== VIEW CUSTOMER ====================
void viewCustomer() {
    string accNum;
    cout << "Enter Account Number: ";
    cin >> accNum;

    Account* acc = findAccount(accNum);
    if (acc) {
        acc->display();
        cout << "Phone: " << acc->getPhone() << endl;
        cout << "Email: " << acc->getEmail() << endl;
        cout << "Address: " << acc->getAddress() << endl;
        cout << "DOB: " << acc->getDob() << endl;
    }
    else {
        cout << "Account not found!\n";
    }
}

// ==================== PROCESS TRANSACTION ====================
void processTransaction(Teller& teller) {
    string accNum, pin;
    cout << "Customer Account: ";
    cin >> accNum;
    cout << "Customer PIN: ";
    cin >> pin;

    Account* customer = findAccount(accNum);
    if (!customer || !customer->verifyPin(pin)) {
        cout << "Invalid credentials!\n";
        return;
    }

    int choice;
    double amount;
    cout << "\n1. Deposit\n2. Withdraw\n3. Transfer\nChoice: ";
    cin >> choice;

    if (choice == 1) {
        cout << "Amount: R";
        cin >> amount;
        if (amount > 0) {
            customer->deposit(amount);
            cout << "Deposited R" << amount << ". New balance: R" << customer->getBalance() << endl;
            addTransaction(accNum, "DEPOSIT", amount);
        }
    }
    else if (choice == 2) {
        cout << "Amount: R";
        cin >> amount;
        if (customer->withdraw(amount)) {
            cout << "Withdrew R" << amount << ". New balance: R" << customer->getBalance() << endl;
            addTransaction(accNum, "WITHDRAWAL", amount);
        }
        else {
            cout << "Insufficient funds or below minimum balance!\n";
        }
    }
    else if (choice == 3) {
        string toAcc;
        cout << "Destination Account: ";
        cin >> toAcc;
        cout << "Amount: R";
        cin >> amount;

        Account* dest = findAccount(toAcc);
        if (dest && customer->withdraw(amount)) {
            dest->deposit(amount);
            cout << "Transferred R" << amount << " to " << toAcc << endl;
            addTransaction(accNum, "TRANSFER-OUT", amount);
            addTransaction(toAcc, "TRANSFER-IN", amount);
        }
        else {
            cout << "Transfer failed!\n";
        }
    }
}

// ==================== REPORTS ====================
void branchReport(Teller& teller) {
    cout << "\n=== BRANCH REPORT ===\n";
    cout << "Branch: " << teller.branchCode << endl;
    cout << "Date: " << getCurrentTime() << endl;

    int custCount = 0;
    double totalBalance = 0;

    for (Account* acc : accounts) {
        if (acc->getBranchCode() == teller.branchCode) {
            custCount++;
            totalBalance += acc->getBalance();
        }
    }

    cout << "Customers: " << custCount << endl;
    cout << "Total Balance: R" << fixed << setprecision(2) << totalBalance << endl;
    if (custCount > 0) {
        cout << "Average Balance: R" << totalBalance / custCount << endl;
    }
}

void viewBranches() {
    cout << "\n=== ALL BRANCHES ===\n";
    for (Branch& b : branches) {
        b.customerCount = 0;
        b.totalBalance = 0;
        for (Account* acc : accounts) {
            if (acc->getBranchCode() == b.code) {
                b.customerCount++;
                b.totalBalance += acc->getBalance();
            }
        }
        cout << "Code: " << b.code << " | " << b.name << endl;
        cout << "Location: " << b.location << endl;
        cout << "Customers: " << b.customerCount << endl;
        cout << "Total Balance: R" << fixed << setprecision(2) << b.totalBalance << endl;
        cout << "-------------------\n";
    }
}

void interBranchCompare() {
    cout << "\n=== INTER-BRANCH COMPARISON ===\n";

    for (Branch& b : branches) {
        b.customerCount = 0;
        b.totalBalance = 0;
        for (Account* acc : accounts) {
            if (acc->getBranchCode() == b.code) {
                b.customerCount++;
                b.totalBalance += acc->getBalance();
            }
        }

        cout << "\n" << b.name << " (" << b.code << ")\n";
        cout << "  Customers: " << b.customerCount << endl;
        cout << "  Total Balance: R" << fixed << setprecision(2) << b.totalBalance << endl;
        if (b.customerCount > 0) {
            cout << "  Average: R" << b.totalBalance / b.customerCount << endl;
        }
    }
}

void dailyReport() {
    cout << "\n=== DAILY TRANSACTION REPORT ===\n";
    cout << "Date: " << getCurrentDate() << endl;

    int transCount = 0;
    double totalAmount = 0;
    string today = getCurrentDate();

    for (Transaction t : transactions) {
        string dateTime(t.dateTime);
        if (dateTime.find(today) != string::npos) {
            transCount++;
            totalAmount += t.amount;
        }
    }

    cout << "Total Transactions: " << transCount << endl;
    cout << "Total Volume: R" << fixed << setprecision(2) << totalAmount << endl;
    if (transCount > 0) {
        cout << "Average Transaction: R" << totalAmount / transCount << endl;
    }
}

void customerSummary() {
    cout << "\n=== CUSTOMER SUMMARY ===\n";

    int totalCust = accounts.size();
    double totalBalance = 0;
    int savings = 0, cheque = 0, fixedC = 0, student = 0;

    for (Account* acc : accounts) {
        totalBalance += acc->getBalance();
        if (acc->getAccountType() == "Savings") savings++;
        else if (acc->getAccountType() == "Cheque") cheque++;
        else if (acc->getAccountType() == "Fixed Deposit") fixedC++;
        else student++;
    }

    cout << "Total Customers: " << totalCust << endl;
    cout << "Total Balance: R" << fixed << setprecision(2) << totalBalance << endl;
    if (totalCust > 0) {
        cout << "Average Balance: R" << totalBalance / totalCust << endl;
    }
    cout << "\nBy Account Type:\n";
    cout << "  Savings: " << savings << " accounts\n";
    cout << "  Cheque: " << cheque << " accounts\n";
    cout << "  Fixed Deposit: " << fixedC << " accounts\n";
    cout << "  Student: " << student << " accounts\n";
}

void branchPerformance() {
    cout << "\n=== BRANCH PERFORMANCE REPORT ===\n";

    double totalBankBalance = 0;
    for (Account* acc : accounts) {
        totalBankBalance += acc->getBalance();
    }

    for (Branch& b : branches) {
        b.customerCount = 0;
        b.totalBalance = 0;
        for (Account* acc : accounts) {
            if (acc->getBranchCode() == b.code) {
                b.customerCount++;
                b.totalBalance += acc->getBalance();
            }
        }

        cout << "\n" << b.name << " (" << b.code << "):\n";
        cout << "  Customers: " << b.customerCount << endl;
        cout << "  Total Balance: R" << fixed << setprecision(2) << b.totalBalance << endl;

        double marketShare = 0;
        if (totalBankBalance > 0) {
            marketShare = (b.totalBalance / totalBankBalance) * 100;
        }
        cout << "  Market Share: " << fixed << setprecision(1) << marketShare << "%\n";

        cout << "  Growth Potential: ";
        if (b.customerCount < 30) cout << "HIGH (Expand branch)\n";
        else if (b.customerCount < 70) cout << "MEDIUM (Stable growth)\n";
        else cout << "LOW (Mature market)\n";
    }
}

// ==================== CUSTOMER MENU ====================
void customerMenu(Account* customer) {
    int choice;
    do {
        cout << "\n===== CUSTOMER MENU =====\n";
        cout << "1. View Balance\n";
        cout << "2. Deposit\n";
        cout << "3. Withdraw\n";
        cout << "4. Transfer\n";
        cout << "5. View Statement\n";
        cout << "6. Change PIN\n";
        cout << "7. Logout\n";
        cout << "Choice: ";
        cin >> choice;

        double amount;
        string toAcc, oldPin, newPin;

        switch (choice) {
        case 1:
            cout << "Balance: R" << fixed << setprecision(2) << customer->getBalance() << endl;
            break;

        case 2:
            cout << "Amount: R";
            cin >> amount;
            if (amount > 0) {
                customer->deposit(amount);
                cout << "Deposited R" << amount << endl;
                addTransaction(customer->getAccNumber(), "DEPOSIT", amount);
            }
            break;

        case 3:
            cout << "Amount: R";
            cin >> amount;
            if (customer->withdraw(amount)) {
                cout << "Withdrew R" << amount << endl;
                addTransaction(customer->getAccNumber(), "WITHDRAWAL", amount);
            }
            else {
                cout << "Insufficient funds or below minimum balance!\n";
            }
            break;

        case 4:
            cout << "Destination Account: ";
            cin >> toAcc;
            cout << "Amount: R";
            cin >> amount;

            if (toAcc == customer->getAccNumber()) {
                cout << "Cannot transfer to yourself!\n";
                break;
            }

            {
                Account* dest = findAccount(toAcc);
                if (dest && customer->withdraw(amount)) {
                    dest->deposit(amount);
                    cout << "Transferred R" << amount << " to " << toAcc << endl;
                    addTransaction(customer->getAccNumber(), "TRANSFER-OUT", amount);
                    addTransaction(toAcc, "TRANSFER-IN", amount);
                }
                else {
                    cout << "Transfer failed!\n";
                }
            }
            break;

        case 5:
            showStatement(customer);
            break;

        case 6:
            cout << "Enter current PIN: ";
            cin >> oldPin;
            cout << "Enter new 5-digit PIN: ";
            cin >> newPin;
            customer->changePin(oldPin, newPin);
            cout << "PIN changed successfully!\n";
            break;
        }
    } while (choice != 7);
}
