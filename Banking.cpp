#include <iostream>
#include <fstream>
#include <string>
#include <limits>
using namespace std;

class Teller {
    string TellerID;
    string Full;
    string password;
    string branchcode;

public:
    string encrypt(const string &pass){
        string encrypted = "";
        for (char c : pass){
            encrypted += c + 2;
        }
        return encrypted;
    } 

    void createTeller() {
        cout << "Enter Teller ID: ";
        cin >> TellerID;

        cout << "Enter Name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, Full);

        cout << "Enter Password: ";
        cin >> password;
        password = encrypt(password);

        cout << "Enter Branch Code: ";
        cin >> branchcode;
    }

    void saveToFile(ofstream &outFile){
        outFile << Teller_ID << endl;
        outFile << Full << endl;
        outFile << password << endl;
        outFile << branchcode << endl;
        outFile << "-----" << endl;
    }
    bool login(){
        string id, pass;
        string storedID, storedName, storedPass, storedBranch;

        cout << "Enter teller id: ";
        cin >> id;

        pass = encrypt(pass);

        ifstream inFile("data.dat");

        if(!inFile) {
            cout << "Error" << endl;
            return false;
        }

        while (getline(inFile, storedID)) {
            getline(inFile, storedName);
            getline(inFile, storedPass);
            getline(inFile, storedBranch);
            inFile.ignore(5);

            if (storedID == id && storedPass == pass) {
                cout << "Login successful!\n";

                // store logged-in teller
                Teller_ID = storedID;
                Full = storedName;
                password = storedPass;
                branchcode = storedBranch;

                return true;
        }
    }
};

int main(){
    Teller t1;

    int choice;
    cout << "1. Register Teller\n2. Login\nChoice: ";
    cin >> choice;

    if (choice == 1) {
        t1.createTeller();

        ofstream outFile("data.dat", ios::app);
        if (outFile.is_open()){
            t1.saveToFile(outFile);
            outFile.close();
            cout << "Teller is saved\n";
        }
        else {
            cout << "Error with file\n";
        }
    }

    else if (choice == 2) {
        int attempts = 0;

        while (attempts < 3) {
            if (t1.login()) {
                cout << "Access granted.\n";
                break;
            }
            attempts++;
        }

        if (attempts == 3) {
            cout << "Too many failed attempts!\n";
        }
    }

    return 0;
}
