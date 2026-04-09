#include <iostream>
#include <fstream>
#include <string>
#include <limits>
using namespace std;

class Teller {
    int Teller_ID;
    string Full;
    string password;
    string branchcode;

public:
    string encrypt(const string &pass){
        string encrypted = "";
        for (int i = pass.length() - 1; i >= 0; i--){
            encrypted += pass[i];
        }
        return encrypted;
    } 

    void createTeller() {
        cout << "Enter Teller ID: ";
        cin >> Teller_ID;

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
};

int main(){
    Teller t1;
    t1.createTeller();
    
    ofstream outFile("data.dat", ios::app);
    if (outFile.is_open()){
        t1.saveToFile(outFile);
        outFile.close();
        cout << "Teller is saved" << endl;
    }
    else {
        cout << "Error with file" << endl;
    }

    return 0;
}