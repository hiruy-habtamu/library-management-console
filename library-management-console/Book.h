struct Book {
    int bookid;
    int copyid;
    string title; 	
    string author; 	
    string ISBN;	
    int year; 	// Make sure the year is in format like '2020'
    string Category;

};

// TODO: MAKE CONFIGURATIONS FOR ADD BOOK AND REMOVE BOOK MEANINGFULLY
// 
// 
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
