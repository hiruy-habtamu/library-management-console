#include <iostream>
#include <string>
#include <mysqlx/xdevapi.h>
using namespace std;

mysqlx::Session sess("localhost", 33060, "root", "root", "library_db");
mysqlx::Schema db = sess.getSchema("library_db");

#include "picosha2.h"
//picosha2 is a third-party single header library used for sha256 hash generation

#include "User.h"        
// Default password for all dummy users is abcd@1234

#include "Book.h"     
#include "Reservation.h" 
#include "Transaction.h"
#include "Report.h"

// Initiate current user
User currentUser;

// LIBRARIAN ONLY MENUU//
void displayLibrarianMenu() {
    currentUser.displayUserDetails();
    cout << "******************** Library Management System **********************" << endl;
    cout << "_____________________________ LIBRARIAN MENU ________________________" << endl;
    cout << "1. View Transactions" << endl;
    cout << "2. View Reserved Books" << endl;
    cout << "3. Add Book " << endl;
    cout << "4. Remove Book " << endl;
    cout << "5. Logout " << endl;
    cout << "6. Deactivate Librarian Account" << endl;
	cout << "7. Change Fine rate" << endl;
	cout << "8. View All Books" << endl;
	cout << "9. Export Reports to CSV" << endl;
    cout << "*********************************************************************" << endl;
    cout << "Please choose an option: ";
}



void displayUserMenu() {
    currentUser.displayUserDetails();
    cout << "************************Library Management System *******************" << endl;
    cout << "_____________________________USER MENU_______________________________" << endl;
    cout << "1. Borrow Book" << endl; // Should Automatically ask to reserve book if there is no copy that exists.
    cout << "2. Return Book" << endl;
    cout << "3. View All Books" << endl;
    cout << "4. View My Transactions" << endl;
    cout << "5. View My Reservations" << endl;
    cout << "6. Logout " << endl;
    cout << "7. Deactivate User account " << endl;
	cout << "8. Most Popular Books" << endl;
    cout << "9. Remove reservations" << endl;
    cout << "*********************************************************************" << endl;
    cout << "Please choose an option: ";
}

int main() {
    bool running = true;
    while (running) {
        // both bool running and deactivate_choice are used for deactivation purposes
        char deactivate_choice;
        string plain_password,password;

        char choice;
        if (currentUser.is_empty()||currentUser.status == "Inactive") {

            cout << "*************** MAIN MENU **************************" << endl;
            cout << "1. Sign Up " << endl;
            cout << "2. Login " << endl;
            cout << "3. Exit " << endl;
            cout << "\nEnter choice:";
            cin >> choice;
            switch (choice) {
            case '1':
                system("cls");
                currentUser = signup();
                break;
            case '2':
                system("cls");
                currentUser = login();
                break;
            case '3':
                running = false;
                break;
            default:
                cout << "Invalid Input " << endl;
                break;
            }
        }
        else if (currentUser.role == "Student" || currentUser.role == "Faculty") {
            system("cls");
            displayUserMenu();
            cin >> choice;
            switch (choice) {
            case '1': {
                // BorrowBook stuff
                system("cls");
                int bookIDToBorrow;
                cout << "--- Borrow Book ---" << endl;
                cout << "Enter the Book ID you want to borrow: ";
                cin >> bookIDToBorrow;

                if (borrowBook(currentUser.userID, bookIDToBorrow)) {
                    cout << "\nBorrow operation completed." << endl;
                }
                else {
                    cout << "\nBorrow operation failed." << endl;
                }

                cout << "\nPress Enter to continue...";
                cin.ignore();
                cin.get();
                break;
             }
            case '2': {
                // Return Book stuff
                system("cls");
                int transactionIDToReturn;
                cout << "--- Return Book ---" << endl;
                cout << "Enter the Transaction ID to return: ";
                cin >> transactionIDToReturn;

                if (returnBook(currentUser.userID, transactionIDToReturn)) {
                    cout << "\nReturn operation completed." << endl;
                }
                else {
                    cout << "\nReturn operation failed." << endl;
                }

                cout << "\nPress Enter to continue...";
                cin.ignore();
                cin.get();
            }
                break;
            case '3':
                // Show all book
                viewAllBooks();
                cout << "\nPress Enter to continue...";
                cin.ignore();
                cin.get();
                break;
            case '4':
                //View Transaction
                system("cls");
                cout << "Viewing your transaction history..." << endl;
                viewUserTransactions(currentUser.userID);
                cout << "\nPress any key to continue...";
                cin.ignore();
                cin.get();
                system("cls");
                break;
            case '5':
                // View Reservation
				viewUserReservations(currentUser.userID);
                cout << "\nPress any key to continue...";
                cin.ignore();
                cin.get();
                system("cls");
                break;
            case '6':
                //logout
                system("cls");
                currentUser.clear();
                break;
            case '7':
                cout << "Enter password: " << endl;
                cin >> plain_password;
                // Hash the plain password into a hashed password
                password = picosha2::hash256_hex_string(plain_password);
                if (password == currentUser.password) {

                    cout << "Are you sure you want to deactivate your account? Y/N " << endl;
                    cin >> deactivate_choice;
                    if (deactivate_choice == 'N' || deactivate_choice == 'n') {
                        system("cls");
                        break;
                    }
                }
                else {
                    cout << "Incorrect Password " << endl;
                    system("cls");
                    break;
                }
                // Deactivate User in database
                userTable.update().set("Status", "Inactive").where("UserID = :id").bind("id", currentUser.userID).execute();
                // Deactivate in Struct
                currentUser.status = "Inactive";
                system("cls");
                break;
            case '8':
				// View most popular books
				system("cls");
				cout << "Viewing most popular books..." << endl;
				MostPopularBooks();
				cout << "\nPress any key to continue...";
				cin.ignore();
				cin.get();
				break;
            case '9':
                int reservationID;
                std::cout << "Please enter the Reservation ID you want to remove: ";
                std::cin >> reservationID;
                removeReservation(reservationID, currentUser.userID);
                cout << "\nPress any key to continue...";
                cin.ignore();
                cin.get();
                break;
            default:
                cout << "Invalid Input " << endl;
                break;
            }
        }
        else if (currentUser.role == "Librarian") {
            system("cls");
            displayLibrarianMenu();
            cin >> choice;
            switch (choice) {
            case '1':
                // View transaction
                system("cls");
                cout << "Viewing all library transactions..." << endl;
                viewAllTransactions();
                cout << "\nPress any key to continue...";
                cin.ignore();
                cin.get();
                system("cls");
                break;
            case '2':
                // View all reserved books
				system("cls");
                viewReservedBooks();
                cout << "\nPress any key to continue...";
                cin.ignore();
                cin.get();
                break;
            case '3':
                system("cls");
                addBook();
                break;
            case '4':
                system("cls");
                removeBook();
                break;
            case '5':
                // Logout
                currentUser.clear();
                system("cls");
                break;
            case '6':
                cout << "Enter password: " << endl;
                cin >> password;
                if (password == currentUser.password) {
                 
                    cout << "Are you sure you want to deactivate your account? Y/N " << endl;
                    cin >> deactivate_choice;
                    if (deactivate_choice == 'N' || deactivate_choice == 'n') {
                        system("cls");
                        break;
                    }
                }
                else {
                    system("cls");
                    cout << "Incorrect Password " << endl;
                    break;
                }
                // Deactivate User in database
                userTable.update().set("Status", "Inactive").where("UserID = :id").bind("id", currentUser.userID).execute();
                // Deactivate in Struct
                currentUser.status = "Inactive";
                system("cls");
                break;
            case '7':
                changeFineRate();
                break;
            case '8':
				// View all books
				system("cls");
				cout << "...Viewing all books..." << endl;
				viewAllBooks();
				cout << "\nPress any key to continue...";
				cin.ignore();
				cin.get();
				break;
            case '9':
				// Export reports to CSV
				system("cls");
				cout << "Exporting reports..." << endl;
                exportBorrowedBooks();
                exportOverdueBooks();
                exportMonthlyBorrowing();
                exportPopularBooks();
                exportFinesSummary();
				cout << "\nReports exported successfully." << endl;
				cout << "\nPress any key to continue...";
				cin.ignore();
				cin.get();
				break;
            default:
                cout << "Invalid Input " << endl;
                break;
            }

        }
    }
    sess.close();
    return 0;
}