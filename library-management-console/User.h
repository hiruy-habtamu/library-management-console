#include <iostream>
#include <string>

using namespace std;
struct User {
    int userID;
    string name;
    string email;
    string role;
    string password;
    string universityID;

    void displayUserDetails() const {
        cout << "User ID: " << userID << endl;
        cout << "Name: " << name << endl;
        cout << "Email: " << email << endl;
        cout << "Role: " << role << endl;
        cout << "University ID: " << universityID << endl;
    }
};
