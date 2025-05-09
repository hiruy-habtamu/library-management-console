struct Book {
    int bookid;
    int copyid;
    string title; 	
    string author; 	
    string ISBN;	
    int year; 	// Make sure the year is in format like '2020'
    string Category;

};


 
// LIBRARIAN ONLY///
mysqlx::Table bookTable = db.getTable("book");

void addBook() {
   


    string title, author, isbn, category;
    char choice;
    int year;
    cout << "Enter ISBN " << endl;
    cin >> isbn;
    cout << endl;


    //since isbn is unique for all books checking the book if it exist in the database with isbn
    mysqlx::RowResult rr = bookTable.select("BookID").where("ISBN = :isbn").bind("isbn", isbn).execute();
    mysqlx::Row row = rr.fetchOne();
    if (row && !row[0].isNull()) {
        // Book exists
        int book_id = row[0].get<int>();
        cout << "Book already exists in the system.\nDo you want to add a new copy of it? (y/n): " << endl;
        cin >> choice;

        if (choice == 'y' || choice == 'Y') {
            // Get highest current CopyID for that BookID
            rr = bookTable.select("MAX(CopyID)").where("BookID = :id").bind("id", book_id).execute();
            row = rr.fetchOne();

            int new_copy_id = (row && !row[0].isNull()) ? row[0].get<int>() + 1 : 2;


            //here using other data(meta data) of the copy to be added ,from the book that already exist
            rr = bookTable.select("Title", "Author", "PublicationYear", "Category").where("BookID = :id").bind("id", book_id).execute();
            row = rr.fetchOne();
            title = row[0].get<string>();
            author = row[1].get<string>();
            year = row[2].get<int>();
            category = row[3].get<string>();

            // Insert new copy
            bookTable.insert("BookID", "CopyID", "Title", "Author", "ISBN", "PublicationYear", "Category", "Status")
                .values(book_id, new_copy_id, title, author, isbn, year, category, "Available").execute();

            cout << "New copy added successfully! (BookID: " << book_id << ", CopyID: " << new_copy_id << ")" << endl;

        }
        else {
            cout << "Operation canceled." << endl;
            return;
        }
    }
    else {
        cout << "Book not found in the system.\nEnter book details:" << endl;

        cout << "Enter Book Title: "<<endl;
        cin >> title;
        cout << "Enter Author: "<<endl;
        cin >> author;
        cout << "Enter Publication Year: "<<endl;
        cin >> year;
        cout << "Enter Category (fiction/non-fiction/academic/other): "<<endl;
        cin >> category;

        rr = bookTable.select("MAX(BookID)").execute();

        row = rr.fetchOne();
        int new_book_id = (row && !row[0].isNull()) ? row[0].get<int>() + 1 : 1;
        int copy_id = 1;

        bookTable.insert("BookID", "CopyID", "Title", "Author", "ISBN", "PublicationYear", "Category", "Status")
            .values(new_book_id, copy_id, title, author, isbn, year, category, "Available").execute();

        cout << "New book added successfully! (BookID: " << new_book_id << ", CopyID: " << copy_id << ")" ;

    }
    
}

// LIBRARIAN ONLY///
void removeBook() {
    int bookID, copyID;
    cout << "Enter Book ID: ";
    cin >> bookID;
    cout << "Enter Copy ID: ";
    cin >> copyID;

    mysqlx::RowResult rr = bookTable.select("BookID")
        .where("BookID = :bid AND CopyID = :cid")
        .bind("bid", bookID)
        .bind("cid", copyID)
        .execute();

    mysqlx::Row row = rr.fetchOne();

    if (!row.isNull()) {
        // Book exists, delete it
        bookTable.remove()
            .where("BookID = :bid AND CopyID = :cid")
            .bind("bid", bookID)
            .bind("cid", copyID)
            .execute();
        cout << "Book deleted successfully." << endl;
    }
    else {
        cout << "Book not found." << endl;
    }


}




