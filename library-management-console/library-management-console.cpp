#include <iostream>
#include <string>
#include <mysqlx/xdevapi.h>
#include "User.h"        
#include "Book.h"        
#include "Transaction.h" 
#include "Reservation.h" 

using namespace std;

// SELECT SESSION AND SCHEMA
mysqlx::Session sess("localhost", 33060, "root", "root", "library_db");
mysqlx::Schema db = sess.getSchema("library_db");

// LIBRARIAN ONLY MENUU//
void displayLibrarianMenu() {
    cout << "==================== Library Management System ====================" << endl;
    cout << "1. Register User" << endl;
    cout << "2. Login User" << endl;
    cout << "3. Borrow Book" << endl;
    cout << "4. Return Book" << endl;
    cout << "5. View Transactions" << endl;
    cout << "6. View Reserved Books" << endl;
    cout << "7. Add Book " << endl;
    cout << "8. Remove Book " << endl;
    cout << "9. Logout" << endl;
    cout << "====================================================================" << endl;
    cout << "Please choose an option: ";
}

User signup() {
    mysqlx::Table u = db.getTable("User");
    //Finds the max user_id
    mysqlx::RowResult rr = u.select("MAX(userID)").execute();
    
    mysqlx::Row row = rr.fetchOne();

    if (row) {
        int last_user_id = row[0];
        int new_user_id = last_user_id + 1;
    }
    else {
        int new_user_id = 1;
    }

    string name, email, role, password, universityID;
    cout << "Enter Name: ";
    cin >> name;
    cout << "Enter Email: ";
    cin >> email;
    cout << "Enter Role (Student/Faculty/Librarian): ";
    cin >> role;
    cout << "Enter Password: ";
    cin >> password;
    cout << "Enter University ID: ";
    cin >> universityID;

    
    //User newUser = {new_user_id,name, email, role, password, universityID };
    
    /*
    TODO Make sql query to calculate next user id
    */
    // SOME SQL STUFF
    cout << "User registered successfully!" << endl;
}

// Function to handle user login
User login() {
    string email, password;
    cout << "Enter Email: ";
    cin >> email;
    cout << "Enter Password: ";
    cin >> password;

/// SOME SQL STUFF TO CHECK IF PASSWORD AND USERNAME MATCH
}

// LIBRARIAN ONLY///
void addBook() {
    string title, author, isbn, category;
    int year;
    cout << "Enter Book Title: ";
    cin >> title;
    cout << "Enter Author: ";
    cin >> author;
    cout << "Enter ISBN: ";
    cin >> isbn;
    cout << "Enter Publication Year: ";
    cin >> year;
    cout << "Enter Category (fiction/non-fiction/academic/other): ";
    cin >> category;

    // CREATE A STRUCT OF BOOK 
    // Book newBook(3, 1, title, author, isbn, year, category);

   //SOME SQL STUFFFF
    cout << "Book added successfully!" << endl;
}

// LIBRARIAN ONLY///
void removeBook() {
    int bookID, copyID;
    cout << "Enter Book ID to remove: ";
    cin >> bookID;
    cout << "Enter Copy ID to remove: ";
    cin >> copyID;


    // Some sql stuffff
    cout << "Book removed successfully!" << endl;
}


//void displayUserMenu() {
//
//}
int main() {
    // TEST TO SHOW HOW THE API WORKS


    //mysqlx::Table bookTable = db.getTable("book");
    //mysqlx::RowResult result = bookTable.select("*").execute();

    //for (mysqlx::Row row : result) {
    //    for (int i = 0; i < row.colCount(); ++i) {
    //        cout << row[i] << "\t";
    //    }
    //    cout << endl;
    //}
    return 0;
}