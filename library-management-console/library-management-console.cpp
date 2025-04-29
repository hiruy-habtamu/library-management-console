#include <iostream>
#include <string>
#include <mysqlx/xdevapi.h>
using namespace std;

mysqlx::Session sess("localhost", 33060, "root", "root", "library_db");
mysqlx::Schema db = sess.getSchema("library_db");

#include "User.h"        
#include "Book.h"        
#include "Transaction.h" 
#include "Reservation.h" 

// Initiate current user
User currentUser;

// LIBRARIAN ONLY MENUU//
void displayLibrarianMenu() {
    cout << "******************** Library Management System **********************" << endl;
    cout << "_____________________________ LIBRARIAN MENU ________________________" << endl;
    cout << "1. View Transactions" << endl;
    cout << "2. View Reserved Books" << endl;
    cout << "3. Add Book " << endl;
    cout << "4. Remove Book " << endl;
    cout << "5. Logout" << endl;
    cout << "6. Deactivate Librarian Account" << endl;
    cout << "*********************************************************************" << endl;
    cout << "Please choose an option: ";
}



void displayUserMenu() {
    cout << "************************Library Management System *******************" << endl;
    cout << "_____________________________USER MENU_______________________________" << endl;
    cout << "1. Borrow Book" << endl; // Should Automatically ask to reserve book if there is no copy that exists.
    cout << "2. Return Book" << endl;
    cout << "3. View Available Books" << endl;
    cout << "4. View My Transactions" << endl;
    cout << "5. View My Reservations" << endl;
    cout << "6. Logout" << endl;
    cout << "7. Deactivate User account " << endl;
    cout << "*********************************************************************" << endl;
    cout << "Please choose an option: ";
}

int main() {

    sess.close();
    return 0;
}