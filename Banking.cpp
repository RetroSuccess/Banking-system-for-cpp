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

// The encryption utilities 
string encrypt(string text) {
  string result = "";
    for (int i = 0; i < text.length(); i++) {
        result += text[i] + 3;  // simple but it works
      }
    return result;
}

string decrypt(string text) {  // not really used but kept for later maybe
  string result = "";
    for (int i = 0; i < text.length(); i++) {
      result += text[i] - 3;
        }
    return result;
}

string getCurrentTime() {
	time_t now = time(0);
    string dt = ctime(&now);
	  dt.pop_back();  // remove newline
    return dt;
}

string getCurrentDate() {
  time_t now = time(0);
    char* dt = ctime(&now);
      string date(dt);
    return date.substr(0, 10);  // just get the YYYY-MM-DD basically
}

// Branch struct to hold info
struct Branch {
    char code[10];
      char name[100];
    char location[200];
    int customerCount;
    	double totalBalance;
    double tempVar;  
};

// The account hierarchy
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
    int extraCounter;  

public:
    Account() {
        balance = 0;
    	locked = false;
        failedAttempts = 0;
          extraCounter = 0;
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
    	  if (failedAttempts >= 3) {
            locked = true;
            cout << "Account locked becaause to too many failed attempts!\n";
        }
        return false;
    }

    void changePin(string oldPin, string newPin) {
        if (verifyPin(oldPin) && newPin.length() == 5) {
            setPin(newPin);
              cout << "PIN changed successfuly\n";  
        } else {
            cout << "PIN change failed!\n";
          }
    }

    virtual void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
          } else {
            cout << "Invalid amount!\n";
        }
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

// Teller class to handle the teller logins and info
class Teller {
public:
    char id[10];
      char name[100];
    char password[50];
      char branchCode[10];
    int failedAttempts;
    bool locked;
      int loginCount;  

    Teller() {
        failedAttempts = 0;
          locked = false;
        loginCount = 0;
        memset(id, 0, 10);
    	  memset(name, 0, 100);
        memset(password, 0, 50);
          memset(branchCode, 0, 10);
    }

    bool login() {
        if (locked) {
            cout << "Account locked! Contact admin.\n";
              return false;
        }

        string inputId, inputPass;
        cout << "Teller ID: ";
          cin >> inputId;
        cout << "Password: ";
        cin >> inputPass;

        ifstream file("tellers.dat", ios::binary);
        if (!file.is_open()) {
            cout << "Error opening tellers file!\n";
              return false;
        }

        Teller t;
        bool found = false;
        while (file.read(reinterpret_cast<char*>(&t), sizeof(Teller))) {
            if (inputId == string(t.id) && encrypt(inputPass) == string(t.password)) {
                *this = t;
                found = true;
                cout << "Login successful! Welcome " << t.name << endl;
    			loginCount++;
                file.close();
                return true;
            }
        }
        file.close();

        if (!found) {
            failedAttempts++;
            cout << "Login failed! Attempt " << failedAttempts << " of 3\n";
              if (failedAttempts >= 3) {
                locked = true;
                cout << "Account locked due to too many failed logins!\n";
            }
        }
        return false;
    }
};

// Transaction struct
struct Transaction {
    char accNumber[30];
      char type[20];
    double amount;
      char dateTime[50];
    char extraField[10];  
};

// Gloabl variables to hold all data in memory
vector<Account*> accounts;
  vector<Teller> tellers;
vector<Branch> branches;
  vector<Transaction> transactions;
int globalTemp = 0;  

// Function prototypes
void loadData();
  void saveData();
void setupBranches();
  void backupData();
void restoreData();
  void exportToCSV();
void exportToText();
  void calcInterest();
void searchcustomers();
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

// The main function 
int main() {
    setupBranches();
      loadData();

    int choice;
    do {
        cout << "     MULTI-BRANCH BANKING SYSTEM:      \n";
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
                if (customer != nullptr && customer->verifyPin(pin)) {
                    customerMenu(customer);
                }
                else {
                    cout << "Invalid credentials! Please try again.\n";
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
                if (expChoice == 1) {
                    exportToCSV();
                } else if (expChoice == 2) {
                    exportToText();
                } else {
                    cout << "Invalid choice!\n";
                }
            }
        }
        catch (exception& e) {
            cout << "Error occured: " << e.what() << endl;  
        }

    } while (choice != 7);

    saveData();

    // clean up memory
    for (int i = 0; i < accounts.size(); i++) {
        delete accounts[i];
      }

    cout << "\nPress Enter to exit...";
    cin.ignore();
    cin.get();

    return 0;
}

// Setting up branches with some default data
void setupBranches() {
    // Johannesburg branch
    Branch b1;
      strcpy(b1.code, "001");
    strcpy(b1.name, "Johannesbug Main");  
      strcpy(b1.location, "100 Main Street, JHB");
    b1.customerCount = 0;
      b1.totalBalance = 0;
    b1.tempVar = 0;
      branches.push_back(b1);
    
    // Cape Town branch  
    Branch b2;
      strcpy(b2.code, "002");
    strcpy(b2.name, "Cape Town Centrl");
      strcpy(b2.location, "50 Long Street, Cape Town");
    b2.customerCount = 0;
      b2.totalBalance = 0;
    b2.tempVar = 0;
      branches.push_back(b2);
    
    // Durban branch
    Branch b3;
      strcpy(b3.code, "003");
    strcpy(b3.name, "Durban North Beach");
      strcpy(b3.location, "75 Florida Road, Durban");
    b3.customerCount = 0;
      b3.totalBalance = 0;
    b3.tempVar = 0;
      branches.push_back(b3);
}

// Binary file handling
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
        // create default teller if file doesn't exist
        Teller defaultTeller;
        strcpy(defaultTeller.id, "T001");
          strcpy(defaultTeller.name, "Admin Teller");
        strcpy(defaultTeller.password, encrypt("admin123").c_str());
          strcpy(defaultTeller.branchCode, "001");
        defaultTeller.failedAttempts = 0;
          defaultTeller.locked = false;
        defaultTeller.loginCount = 0;
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

            if (accType == "Savings") {
                acc = new SavingsAccount();
            } else if (accType == "Cheque") {
                acc = new ChequeAccount();
            } else if (accType == "Fixed Deposit") {
                acc = new FixedDepositAccount();
            } else if (accType == "Student") {
                acc = new StudentAccount();
            }

            if (acc != nullptr) {
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
            int x = 0;
            x++;
        }
        bFile.close();
    }
    else {
        ofstream outB("branches.dat", ios::binary);
        for (int i = 0; i < branches.size(); i++) {
            outB.write(reinterpret_cast<char*>(&branches[i]), sizeof(Branch));
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
    // save tellers
    ofstream tFile("tellers.dat", ios::binary);
      for (int i = 0; i < tellers.size(); i++) {
        tFile.write(reinterpret_cast<char*>(&tellers[i]), sizeof(Teller));
    }
    tFile.close();

    // save accounts
    ofstream aFile("customers.dat", ios::binary);
      for (int i = 0; i < accounts.size(); i++) {
        string type = accounts[i]->getAccountType();
        aFile.write(type.c_str(), 20);
        aFile.write(reinterpret_cast<char*>(accounts[i]), sizeof(Account) - 20);
    }
    aFile.close();

    // save branches
    ofstream bFile("branches.dat", ios::binary);
      for (int i = 0; i < branches.size(); i++) {
        bFile.write(reinterpret_cast<char*>(&branches[i]), sizeof(Branch));
    }
    bFile.close();

    // save transactions
    ofstream trFile("transactions.dat", ios::binary);
      for (int i = 0; i < transactions.size(); i++) {
        trFile.write(reinterpret_cast<char*>(&transactions[i]), sizeof(Transaction));
    }
    trFile.close();

    // save config
    ofstream config("system_config.txt");
      config << "BANKING_SYSTEM_V2.0\n";
    config << "LastBackup: " << getCurrentTime() << "\n";
      config << "TotalAccounts: " << accounts.size() << "\n";
    config << "SystemVersion: 1.0\n";
      config.close();
}

// The backup and restore functions
void backupData() {
    string backupDir = "backup_" + getCurrentDate();
      _mkdir(backupDir.c_str());

    // copy all files
    ifstream src1("tellers.dat", ios::binary);
      ofstream dst1(backupDir + "/tellers.dat", ios::binary);
    dst1 << src1.rdbuf();
      src1.close(); 
    dst1.close();

    ifstream src2("customers.dat", ios::binary);
      ofstream dst2(backupDir + "/customers.dat", ios::binary);
    dst2 << src2.rdbuf();
      src2.close(); 
    dst2.close();

    ifstream src3("branches.dat", ios::binary);
      ofstream dst3(backupDir + "/branches.dat", ios::binary);
    dst3 << src3.rdbuf();
      src3.close(); 
    dst3.close();

    ifstream src4("transactions.dat", ios::binary);
      ofstream dst4(backupDir + "/transactions.dat", ios::binary);
    dst4 << src4.rdbuf();
      src4.close(); 
    dst4.close();

    ifstream src5("system_config.txt");
      ofstream dst5(backupDir + "/system_config.txt");
    dst5 << src5.rdbuf();
      src5.close(); 
    dst5.close();

    cout << "Backup created successfuly in: " << backupDir << endl;  
}

void restoreData() {
    string backupDir;
    cout << "Enter backup directory name: ";
      cin >> backupDir;

    // restore from backup
    ifstream src1(backupDir + "/tellers.dat", ios::binary);
      ofstream dst1("tellers.dat", ios::binary);
    dst1 << src1.rdbuf();
      src1.close(); 
    dst1.close();

    ifstream src2(backupDir + "/customers.dat", ios::binary);
      ofstream dst2("customers.dat", ios::binary);
    dst2 << src2.rdbuf();
      src2.close(); 
    dst2.close();

    ifstream src3(backupDir + "/branches.dat", ios::binary);
      ofstream dst3("branches.dat", ios::binary);
    dst3 << src3.rdbuf();
      src3.close(); 
    dst3.close();

    cout << "Data restored! Please restart the application.\n";
}

// Exporting data to CSV and text files
void exportToCSV() {
    ofstream csv("customers_export.csv");
      if (!csv.is_open()) {
        cout << "Error creating CSV file!\n";
        return;
    }
    
    csv << "Account Number,Name,ID Number,Phone,Email,Type,Balance,Branch\n";
      for (int i = 0; i < accounts.size(); i++) {
        csv << accounts[i]->getAccNumber() << ","
            << accounts[i]->getName() << ","
            << accounts[i]->getIdNumber() << ","
            << accounts[i]->getPhone() << ","
            << accounts[i]->getEmail() << ","
            << accounts[i]->getAccountType() << ","
            << accounts[i]->getBalance() << ","
            << accounts[i]->getBranchCode() << "\n";
    }
    csv.close();

    ofstream transCSV("transactions_export.csv");
      transCSV << "Account Number,Type,Amount,Date/Time\n";
    for (int i = 0; i < transactions.size(); i++) {
        transCSV << transactions[i].accNumber << ","
            << transactions[i].type << ","
            << transactions[i].amount << ","
            << transactions[i].dateTime << "\n";
    }
    transCSV.close();

    cout << "Exported to customers_export.csv and transactions_export.csv\n";
}

void exportToText() {
    ofstream txt("bank_report.txt");
      txt << "========== BANK MANAGEMENT REPORT ==========\n";
    txt << "Generated: " << getCurrentTime() << "\n\n";

      txt << "=== CUSTOMER SUMMARY ===\n";
    for (int i = 0; i < accounts.size(); i++) {
        txt << "Account: " << accounts[i]->getAccNumber() << "\n";
          txt << "Name: " << accounts[i]->getName() << "\n";
        txt << "Type: " << accounts[i]->getAccountType() << "\n";
          txt << "Balance: R" << fixed << setprecision(2) << accounts[i]->getBalance() << "\n";
        txt << "-----------------------------------\n";
    }
    txt.close();
    cout << "Exported to bank_report.txt\n";
}

// Interest calculation function
void calcInterest() {
    double totalInterest = 0;
      for (int i = 0; i < accounts.size(); i++) {
        double interest = accounts[i]->getBalance() * accounts[i]->getInterestRate();
        totalInterest = totalInterest + interest;
        cout << accounts[i]->getAccountType() << " account " << accounts[i]->getAccNumber()
            << " earned R" << fixed << setprecision(2) << interest << " interest\n";
    }
    cout << "Total interest accrued: R" << totalInterest << endl;
      cout << "Interest calculation completed!\n";
}

// Search function
void searchcustomers() {
    int searchChoice;
      cout << "Search by:\n1. Account Number\n2. Name\n3. ID Number\n4. Phone\nChoice: ";
    cin >> searchChoice;

    string keyword;
      cout << "Enter search term: ";
    cin.ignore();
      getline(cin, keyword);

    vector<Account*> results;

      for (int i = 0; i < accounts.size(); i++) {
        switch (searchChoice) {
        case 1:
            if (accounts[i]->getAccNumber().find(keyword) != string::npos)
                results.push_back(accounts[i]);
            break;
        case 2:
            if (accounts[i]->getName().find(keyword) != string::npos)
                results.push_back(accounts[i]);
            break;
        case 3:
            if (accounts[i]->getIdNumber() == keyword)
                results.push_back(accounts[i]);
            break;
        case 4:
            if (accounts[i]->getPhone() == keyword)
                results.push_back(accounts[i]);
            break;
        default:
            cout << "Invalid search option!\n";
            return;
        }
    }

    cout << "\n=== SEARCH RESULTS (" << results.size() << " found) ===\n";
      if (results.size() == 0) {
        cout << "No customers found matching your search.\n";
    } else {
        for (int i = 0; i < results.size(); i++) {
            results[i]->display();
            cout << "------------------------\n";
        }
    }
}

// Helper function to find account by account number
Account* findAccount(string accNum) {
    for (int i = 0; i < accounts.size(); i++) {
        if (accounts[i]->getAccNumber() == accNum) {
            return accounts[i];
        }
    }
    return nullptr;
}

// Adding transaction 
void addTransaction(string accNum, string type, double amount) {
    Transaction t;
      strcpy(t.accNumber, accNum.c_str());
    strcpy(t.type, type.c_str());
      t.amount = amount;
    strcpy(t.dateTime, getCurrentTime().c_str());
      strcpy(t.extraField, "N/A");  
    transactions.push_back(t);
}

// The statement function to show recent transactions for a customer
void showStatement(Account* acc) {
    cout << "\n=== STATEMENT FOR " << acc->getAccNumber() << " ===\n";
      cout << "Name: " << acc->getName() << endl;
    cout << "Type: " << acc->getAccountType() << endl;
      cout << "Balance: R" << fixed << setprecision(2) << acc->getBalance() << endl;
    cout << "\nRecent Transactions:\n";

    int count = 0;
      // show last 10 transactions (the most recent first)
    for (int i = transactions.size() - 1; i >= 0 && count < 10; i--) {
        if (string(transactions[i].accNumber) == acc->getAccNumber()) {
            cout << transactions[i].type << " R" << transactions[i].amount
                << " - " << transactions[i].dateTime << endl;
            count++;
        }
    }
    
      if (count == 0) {
        cout << "No transactions found for this account.\n";
    }
    cout << "========================\n";
}

// Menu for the teller
void tellerMenu(Teller& teller) {
    int choice;
      do {
        cout << "\n===== TELLER MENU =====\n";
        cout << "1. Register Customer\n";
          cout << "2. View Customer Details\n";  
        cout << "3. Process Transaction\n";
          cout << "4. Branch Report\n";
        cout << "5. View All Branches\n";
          cout << "6. Compare Branches\n";
        cout << "7. Daily Transaction Report\n";
          cout << "8. Customer Summary Report\n";
        cout << "9. Branch Performance\n";
          cout << "10. Calculate & Apply Interest\n";
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
        case 10: calcInterest(); break;
        case 11: searchcustomers(); break;
        case 12: cout << "Logging out...\n"; break;
        default: cout << "Invalid option! Please try again.\n";
        }
    } while (choice != 12);
}

// Registering a new customer
void registerCustomer(Teller& teller) {
    Account* newAcc = nullptr;
      int typeChoice;

    cout << "\n=== REGISTER NEW CUSTOMER ===\n";
      cout << "Select Account Type:\n";
    cout << "1. Savings (Min Balance: R1000, Interest: 4.5%)\n";
      cout << "2. Cheque (Min Balance: R5000, Interest: 2%)\n";
    cout << "3. Fixed Deposit (Min Balance: R10000, Interest: 7.5%)\n";
      cout << "4. Student (Min Balance: R0, Interest: 1%)\n";
    cout << "Choice: ";
      cin >> typeChoice;

    switch (typeChoice) {
    case 1: newAcc = new SavingsAccount(); break;
    case 2: newAcc = new ChequeAccount(); break;
    case 3: newAcc = new FixedDepositAccount(); break;
    case 4: newAcc = new StudentAccount(); break;
    default: 
        cout << "Invalid account type selected!\n";
        return;
    }

    static int counter = 10000;
      string accNum = "ACC-" + string(teller.branchCode) + "-" + to_string(counter++);
    newAcc->setAccNumber(accNum);

    string input;
      cout << "Enter Full Name: ";
    cin.ignore();
      getline(cin, input);
    newAcc->setName(input);

    do {
        cout << "SA ID Number (13 digits): ";
          cin >> input;
        if (input.length() != 13) {
            cout << "ID number must be exactly 13 digits!\n";
        }
    } while (input.length() != 13);
      newAcc->setIdNumber(input);

    cout << "Contact Number (10 digits): ";
      cin >> input;
    newAcc->setPhone(input);

    do {
        cout << "Email Address: ";
          cin >> input;
        if (input.find('@') == string::npos || input.find('.') == string::npos) {
            cout << "Please enter a valid email address (must contain @ and .)\n";
        }
    } while (input.find('@') == string::npos || input.find('.') == string::npos);
      newAcc->setEmail(input);

    cout << "Physical Address: ";
      cin.ignore();
    getline(cin, input);
      newAcc->setAddress(input);

    cout << "Date of Birth (DD/MM/YYYY): ";
      cin >> input;
    newAcc->setDob(input);

    double deposit;
      do {
        cout << "Initial Deposit Amount (Minimum: R" << newAcc->getMinBalance() << "): R";
        cin >> deposit;
        if (deposit < newAcc->getMinBalance()) {
            cout << "Deposit amount is below minimum requirement!\n";
        }
    } while (deposit < newAcc->getMinBalance());

    newAcc->setBalance(deposit);
      newAcc->setBranchCode(teller.branchCode);

    srand(time(0));
      string pinCode = to_string(10000 + rand() % 90000);
    newAcc->setPin(pinCode);

      accounts.push_back(newAcc);

    cout << "\n============================\n";
      cout << "ACCOUNT CREATED SUCCESSFULLY!\n";
    cout << "==============================\n";
      cout << "Account Number: " << accNum << endl;
    cout << "Generated PIN: " << pinCode << " (PLEASE SAVE THIS!)" << endl;
      cout << "Initial Balance: R" << fixed << setprecision(2) << deposit << endl;
    cout << "==============================" << endl;

      addTransaction(accNum, "ACCOUNT_OPENING", deposit);
}

// Viewing customer details
void viewCustomer() {
    string accNum;
      cout << "Enter Customer Account Number: ";
    cin >> accNum;

      Account* acc = findAccount(accNum);
    if (acc != nullptr) {
        acc->display();
          cout << "Phone Number: " << acc->getPhone() << endl;
        cout << "Email: " << acc->getEmail() << endl;
          cout << "Address: " << acc->getAddress() << endl;
        cout << "Date of Birth: " << acc->getDob() << endl;
          cout << "Branch Code: " << acc->getBranchCode() << endl;
    }
    else {
        cout << "Account number " << accNum << " not found in system!\n";
    }
}

// Processing transactions for a customer
void processTransaction(Teller& teller) {
    string accNum, pin;
      cout << "Enter Customer Account Number: ";
    cin >> accNum;
      cout << "Enter Customer PIN: ";
    cin >> pin;

      Account* customer = findAccount(accNum);
    if (customer == nullptr) {
        cout << "Account not found!\n";
        return;
    }
    
      if (!customer->verifyPin(pin)) {
        cout << "Invalid PIN! Transaction denied.\n";
        return;
    }

    int choice;
      double amount;
    cout << "\nTransaction Type:\n";
      cout << "1. Deposit\n2. Withdrawal\n3. Transfer\nChoice: ";
    cin >> choice;

      if (choice == 1) {
        cout << "Deposit Amount: R";
        cin >> amount;
        if (amount > 0) {
            customer->deposit(amount);
            cout << "Deposit successful! New balance: R" << customer->getBalance() << endl;
            addTransaction(accNum, "DEPOSIT", amount);
        } else {
            cout << "Invalid deposit amount!\n";
        }
    }
    else if (choice == 2) {
        cout << "Withdrawal Amount: R";
          cin >> amount;
        if (customer->withdraw(amount)) {
            cout << "Withdrawal successful! New balance: R" << customer->getBalance() << endl;
            addTransaction(accNum, "WITHDRAWAL", amount);
        }
        else {
            cout << "Withdrawal failed! Insufficient funds or below minimum balance.\n";
        }
    }
    else if (choice == 3) {
        string toAcc;
          cout << "Destination Account Number: ";
        cin >> toAcc;
          cout << "Transfer Amount: R";
        cin >> amount;

        if (toAcc == accNum) {
            cout << "Cannot transfer to the same account!\n";
            return;
        }

        Account* dest = findAccount(toAcc);
          if (dest != nullptr && customer->withdraw(amount)) {
            dest->deposit(amount);
            cout << "Transfer successful! R" << amount << " sent to account " << toAcc << endl;
            addTransaction(accNum, "TRANSFER_OUT", amount);
            addTransaction(toAcc, "TRANSFER_IN", amount);
        }
        else {
            cout << "Transfer failed! Check destination account or insufficient funds.\n";
        }
    }
    else {
        cout << "Invalid transaction type selected!\n";
    }
}

// The reports
void branchReport(Teller& teller) {
    cout << "\n=== BRANCH REPORT ===\n";
      cout << "Branch Code: " << teller.branchCode << endl;
    cout << "Report Date: " << getCurrentTime() << endl;
      cout << "--------------------------------\n";

    int custCount = 0;
      double totalBalance = 0;

    for (int i = 0; i < accounts.size(); i++) {
        if (accounts[i]->getBranchCode() == teller.branchCode) {
            custCount++;
            totalBalance += accounts[i]->getBalance();
        }
    }

    cout << "Total Customers: " << custCount << endl;
      cout << "Total Balance: R" << fixed << setprecision(2) << totalBalance << endl;
    if (custCount > 0) {
        cout << "Average Balance: R" << totalBalance / custCount << endl;
    }
    cout << "================================\n";
}

void viewBranches() {
    cout << "\n=== ALL BRANCHES ===\n";
    
      // update branch statistics
    for (int i = 0; i < branches.size(); i++) {
        branches[i].customerCount = 0;
          branches[i].totalBalance = 0;
        for (int j = 0; j < accounts.size(); j++) {
            if (accounts[j]->getBranchCode() == branches[i].code) {
                branches[i].customerCount++;
                  branches[i].totalBalance += accounts[j]->getBalance();
            }
        }
    }
    
      // display the branches
    for (int i = 0; i < branches.size(); i++) {
        cout << "\nBranch " << (i+1) << ":\n";
          cout << "  Code: " << branches[i].code << endl;
        cout << "  Name: " << branches[i].name << endl;
          cout << "  Location: " << branches[i].location << endl;
        cout << "  Customers: " << branches[i].customerCount << endl;
          cout << "  Total Balance: R" << fixed << setprecision(2) << branches[i].totalBalance << endl;
        cout << "  -------------------\n";
    }
}

void interBranchCompare() {
    cout << "\n=== INTER-BRANCH COMPARISON ===\n";
      cout << "Comparing all branches performance:\n\n";

    // recalc branch stats
      for (int i = 0; i < branches.size(); i++) {
        branches[i].customerCount = 0;
        branches[i].totalBalance = 0;
        for (int j = 0; j < accounts.size(); j++) {
            if (accounts[j]->getBranchCode() == branches[i].code) {
                branches[i].customerCount++;
                  branches[i].totalBalance += accounts[j]->getBalance();
            }
        }
    }

    // find branch with most customers
      int maxCustomers = 0;
    string topBranch = "";
      for (int i = 0; i < branches.size(); i++) {
        if (branches[i].customerCount > maxCustomers) {
            maxCustomers = branches[i].customerCount;
            topBranch = branches[i].name;
        }
    }

      for (int i = 0; i < branches.size(); i++) {
        cout << branches[i].name << " (" << branches[i].code << ")\n";
        cout << "  → Customers: " << branches[i].customerCount << endl;
          cout << "  → Total Balance: R" << fixed << setprecision(2) << branches[i].totalBalance << endl;
        if (branches[i].customerCount > 0) {
            cout << "  → Average Balance: R" << branches[i].totalBalance / branches[i].customerCount << endl;
        }
        cout << endl;
    }
    
      cout << "🏆 Branch with most customers: " << topBranch << " (" << maxCustomers << " customers)\n";
}

void dailyReport() {
    cout << "\n=== DAILY TRANSACTION REPORT ===\n";
      cout << "Date: " << getCurrentDate() << endl;
    cout << "--------------------------------\n";

      int transCount = 0;
    double totalAmount = 0;
      string today = getCurrentDate();

    for (int i = 0; i < transactions.size(); i++) {
        string dateTime(transactions[i].dateTime);
        if (dateTime.find(today) != string::npos) {
            transCount++;
            totalAmount += transactions[i].amount;
        }
    }

    cout << "Total Transactions Today: " << transCount << endl;
      cout << "Total Transaction Volume: R" << fixed << setprecision(2) << totalAmount << endl;
    if (transCount > 0) {
        cout << "Average Transaction Value: R" << totalAmount / transCount << endl;
    } else {
        cout << "No transactions recorded for today.\n";
    }
    cout << "================================\n";
}

void customerSummary() {
    cout << "\n=== CUSTOMER ACCOUNT SUMMARY ===\n";
      cout << "Generated: " << getCurrentTime() << "\n\n";

    int totalCust = accounts.size();
      double totalBalance = 0;
    int savings = 0, cheque = 0, fixedC = 0, student = 0;

      for (int i = 0; i < accounts.size(); i++) {
        totalBalance += accounts[i]->getBalance();
        if (accounts[i]->getAccountType() == "Savings") {
            savings++;
        } else if (accounts[i]->getAccountType() == "Cheque") {
            cheque++;
        } else if (accounts[i]->getAccountType() == "Fixed Deposit") {
            fixedC++;
        } else if (accounts[i]->getAccountType() == "Student") {
            student++;
        }
    }

      cout << "Total Customers: " << totalCust << endl;
    cout << "Total Balance Across All Accounts: R" << fixed << setprecision(2) << totalBalance << endl;
      if (totalCust > 0) {
        cout << "Average Balance per Customer: R" << totalBalance / totalCust << endl;
    }
    
      cout << "\nAccount Type Breakdown:\n";
    cout << "  • Savings Accounts: " << savings << " (Min R1000, 4.5% interest)"<< endl;
      cout << "  • Cheque Accounts: " << cheque << " (Min R5000, 2% interest)" << endl;
    cout << "  • Fixed Deposit Accounts: " << fixedC << " (Min R10000, 7.5% interest)"<< endl;
      cout << "  • Student Accounts: " << student << " (Min R0, 1% interest)"<< endl;
    cout << "====================================" << endl;
}

void branchPerformance() {
    cout << "\n=== BRANCH PERFORMANCE REPORT ===\n";
      cout << "Performance analysis by branch:\n\n";

    double totalBankBalance = 0;
      for (int i = 0; i < accounts.size(); i++) {
        totalBankBalance += accounts[i]->getBalance();
    }

    // update branch stats
      for (int i = 0; i < branches.size(); i++) {
        branches[i].customerCount = 0;
        branches[i].totalBalance = 0;
        for (int j = 0; j < accounts.size(); j++) {
            if (accounts[j]->getBranchCode() == branches[i].code) {
                branches[i].customerCount++;
                branches[i].totalBalance += accounts[j]->getBalance();
            }
        }
    }

      for (int i = 0; i < branches.size(); i++) {
        cout << "📍 " << branches[i].name << " (Code: " << branches[i].code << "):\n";
        cout << "   Customer Base: " << branches[i].customerCount << " accounts\n";
          cout << "   Total Deposits: R" << fixed << setprecision(2) << branches[i].totalBalance << endl;

        double marketShare = 0;
          if (totalBankBalance > 0) {
            marketShare = (branches[i].totalBalance / totalBankBalance) * 100;
        }
        cout << "   Market Share: " << fixed << setprecision(1) << marketShare << "%\n";

        // growth potential assessment
          cout << "   Growth Potential: ";
        if (branches[i].customerCount < 20) {
            cout << "HIGH - Consider expansion" << endl;
        } else if (branches[i].customerCount < 50) {
            cout << "MEDIUM - Stable growth expected" << endl;
        } else {
            cout << "LOW - Mature market segment" << endl;
        }
        cout << endl;
    }
}

// Customer menu 
void customerMenu(Account* customer) {
    int choice;
      do {
        cout << "\n===== CUSTOMER MENU =====\n";
          cout << "1. Check Balance" << endl;
        cout << "2. Make Deposit" << endl;
          cout << "3. Make Withdrawal" << endl;
        cout << "4. Transfer Funds" << endl;
          cout << "5. View Account Statement" << endl;
        cout << "6. Change PIN" << endl;
          cout << "7. Logout"<< endl;
        cout << "Choice: ";
          cin >> choice;

        double amount;
          string toAcc, oldPin, newPin;

        switch (choice) {
        case 1:
            cout << "\nYour current balance is: R" << fixed << setprecision(2) << customer->getBalance() << endl;
            break;

        case 2:
            cout << "Enter deposit amount: R";
              cin >> amount;
            if (amount > 0) {
                customer->deposit(amount);
                cout << "Deposit successful! New balance: R" << customer->getBalance() << endl;
                addTransaction(customer->getAccNumber(), "DEPOSIT", amount);
            } else {
                cout << "Invalid deposit amount!" << endl;
            }
            break;

        case 3:
            cout << "Enter withdrawal amount: R";
              cin >> amount;
            if (customer->withdraw(amount)) {
                cout << "Withdrawal successful! New balance: R" << customer->getBalance() << endl;
                addTransaction(customer->getAccNumber(), "WITHDRAWAL", amount);
            } else {
                cout << "Withdrawal failed! Check funds or minimum balance requirement." << endl;
            }
            break;

        case 4:
            cout << "Enter destination account number: ";
              cin >> toAcc;
            cout << "Enter transfer amount: R";
              cin >> amount;

            if (toAcc == customer->getAccNumber()) {
                cout << "Cannot transfer money to your own account!" << endl;
                break;
            }

            {
                Account* dest = findAccount(toAcc);
                if (dest != nullptr && customer->withdraw(amount)) {
                    dest->deposit(amount);
                    cout << "Transfer successful! R" << amount << " sent to account " << toAcc << endl;
                    addTransaction(customer->getAccNumber(), "TRANSFER_OUT", amount);
                    addTransaction(toAcc, "TRANSFER_IN", amount);
                } else {
                    cout << "Transfer failed! Destination account not found or insufficient funds." << endl;
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
            if (newPin.length() == 5) {
                customer->changePin(oldPin, newPin);
            } else {
                cout << "PIN must be exactly 5 digits!" << endl;
            }
            break;

        case 7:
            cout << "Logging out... Thank you for banking with us!" << endl;
            break;

        default:
            cout << "Invalid option! Please select 1-7." << endl;
        }
    } while (choice != 7);
}
