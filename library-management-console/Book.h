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
        cout << "Book not found in the system.\n \tPlease Enter book Details." << endl;

        cout << "Enter Book Title: "<<endl;
        getline(cin >> ws, title);
        cout << "Enter Author: "<<endl;
        getline(cin >> ws, author);
        cout << "Enter Publication Year (YYYY): "<<endl;
        cin >> year;
        cout << "Enter Category (fiction/non-fiction/academic/other): "<<endl;
        getline(cin >> ws, category);

        rr = bookTable.select("MAX(BookID)").execute();

        row = rr.fetchOne();
        int new_book_id = (row && !row.isNull()) ? row[0].get<int>() + 1 : 1;
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

    try {
        mysqlx::RowResult rr = bookTable.select("status")
            .where("BookID = :bid AND CopyID = :cid")
            .bind("bid", bookID)
            .bind("cid", copyID)
            .execute();

        mysqlx::Row row = rr.fetchOne();
        if (row && !row.isNull()) {
            string status = row[0].get<string>();
            cout << "Current Status: " << status << endl; 

            if (status != "removed") {
                auto result = bookTable.update()
                    .set("Status", "removed")
                    .where("BookID = :bid AND CopyID = :cid")
                    .bind("bid", bookID)
                    .bind("cid", copyID)
                    .execute();

                cout << "Rows affected: " << result.getAffectedItemsCount() << endl; 
                cout << "Book removed successfully." << endl;
            } else {
                cout << "Book is already removed." << endl;
            }
        } else {
            cout << "Book not found." << endl;
        }
    } catch (const mysqlx::Error &err) {
        cerr << "Error: " << err.what() << endl;
    }


}

void viewAllBooks() {
    mysqlx::RowResult rr = bookTable
        .select("BookID", "CopyID", "Title", "Author", "ISBN", "PublicationYear", "Category", "Status")
        .where("Status != 'removed'")
        .execute();

    if (rr.count() == 0) {
        cout << "\nNo books found (excluding removed).\n" << endl;
        return;
    }

    cout << "\n------------------------ All Books ------------------------\n";
    for (mysqlx::Row row : rr) {
        cout << "BookID: " << row[0].get<int>() << ", CopyID: " << row[1].get<int>() << endl;
        cout << "Title: " << row[2].get<string>() << endl;
        cout << "Author: " << row[3].get<string>() << endl;
        cout << "ISBN: " << row[4].get<string>() << endl;
        cout << "Year: " << row[5].get<int>() << endl;
        cout << "Category: " << row[6].get<string>() << endl;
        cout << "Status: " << row[7].get<string>() << endl;
        cout << "------------------------------------------------------------\n";
    }
}














