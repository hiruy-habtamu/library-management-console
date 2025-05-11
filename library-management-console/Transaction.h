// Get Transaction table globally
mysqlx::Table transactionTable = db.getTable("Transaction");

// Print a transaction's details
inline void printTransactionDetails(
    int transactionID,
    int userID,
    int bookID,
    int copyID,
    const string& borrowDate,
    const string& dueDate,
    const string& returnDate,
    double fine
) {
    cout << "-------------------------------------\n";
    cout << "Transaction ID: " << transactionID << endl;
    cout << "User ID: " << userID << endl;
    cout << "Book ID: " << bookID << endl;
    cout << "Copy ID: " << copyID << endl;
    cout << "Borrow Date: " << borrowDate << endl;
    cout << "Due Date: " << dueDate << endl;
    cout << "Return Date: "
        << (returnDate.empty() || returnDate == "NULL" ? "Not Returned" : returnDate)
        << endl;
    cout << "Fine: $" << fine << endl;
    cout << "-------------------------------------\n";
}

// Create a new transaction record

inline bool createTransaction(
    int userID,
    int bookID,
    int copyID,
    const std::string& borrowDateSQL,
    const std::string& dueDateSQL
) {
    // Determine new transaction ID
    auto rr_max_id = transactionTable.select("MAX(TransactionID)").execute();
    auto row_max_id = rr_max_id.fetchOne();
    int newTransactionID = 1;
    if (row_max_id && !row_max_id[0].isNull()) {
        newTransactionID = row_max_id[0].get<int>() + 1;
    }

    try {
        transactionTable.insert(
            "TransactionID", "UserID", "BookID", "CopyID", "BorrowDate", "DueDate", "Fine"
        )
            .values(newTransactionID, userID, bookID, copyID, borrowDateSQL, dueDateSQL, 0.0)
            .execute();

        std::cout << "Transaction created successfully. ID: " << newTransactionID << std::endl;
        return true;
    }
    catch (const mysqlx::Error& err) {
        std::cerr << "ERROR creating transaction: " << err.what() << std::endl;
        return false;
    }
}


bool borrowBook(int userID, int bookID) {
    // 1. Check if the book exists and is available (status is 'Available')
    mysqlx::RowResult rrAvailableCopy = bookTable.select("CopyID")
        .where("BookID = :bid AND Status = 'Available'")
        .bind("bid", bookID)
        .execute();
    mysqlx::Row rowAvailableCopy = rrAvailableCopy.fetchOne();

    if (!rowAvailableCopy) {
        std::cout << "Sorry, book with ID " << bookID << " is currently not available." << std::endl;
        return false;
    }

    int copyIDToBorrow = rowAvailableCopy[0].get<int>();

    // 2. Get borrow date (current date from the database)
    mysqlx::SqlResult rrBorrowDate = sess.sql("SELECT CURRENT_DATE()").execute();
    mysqlx::Row rowBorrowDate = rrBorrowDate.fetchOne();
    if (!rowBorrowDate || rowBorrowDate[0].isNull()) {
        std::cerr << "Error getting current date from database." << std::endl;
        return false;
    }
    std::string borrowDateSQL = rowBorrowDate[0].get<std::string>();

    // 3. Calculate due date (e.g., 14 days from borrow date using SQL)
    mysqlx::SqlResult rrDueDate = sess.sql("SELECT DATE_ADD(CURRENT_DATE(), INTERVAL 14 DAY)").execute();
    mysqlx::Row rowDueDate = rrDueDate.fetchOne();
    if (!rowDueDate || rowDueDate[0].isNull()) {
        std::cerr << "Error calculating due date from database." << std::endl;
        return false;
    }
    std::string dueDateSQL = rowDueDate[0].get<std::string>();

    // 4. Create the transaction
    if (createTransaction(userID, bookID, copyIDToBorrow, borrowDateSQL, dueDateSQL)) {
        // 5. Update the book copy status to 'Borrowed' in the bookTable
        auto updateResult = bookTable.update()
            .set("Status", "Borrowed")
            .where("BookID = :bid AND CopyID = :cid")
            .bind("bid", bookID)
            .bind("cid", copyIDToBorrow)
            .execute();

        if (updateResult.getAffectedItemsCount() > 0) {
            std::cout << "Book borrowed successfully!" << std::endl;
            return true;
        }
        else {
            std::cout << "Warning: Could not update book copy status." << std::endl;
            // Consider rolling back the transaction creation if status update fails
            return false;
        }
    }
    else {
        return false;
    }
}

// Mark a transaction as returned


inline bool completeTransaction(int transactionID, const string& actualReturnDateSQL) {
    try {
        auto result = transactionTable.update()
            .set("ReturnDate", actualReturnDateSQL)
            .where("TransactionID = :tid")
            .bind("tid", transactionID)
            .execute();

        if (result.getAffectedItemsCount() > 0) {
            cout << "Transaction ID " << transactionID << " marked as returned on " << actualReturnDateSQL << endl;
            return true;
        }
        else {
            cerr << "Error: Could not update transaction ID " << transactionID << " for return." << endl;
            return false;
        }
    }
    catch (const mysqlx::Error& err) {
        cerr << "Error updating transaction: " << err.what() << endl;
        return false;
    }
}
// View transactions for a specific user
inline void viewUserTransactions(int userID) {
    auto rr = transactionTable.select("*")
        .where("UserID = :uid").orderBy("BorrowDate DESC").bind("uid", userID)
        .execute();

    bool found = false;
    decltype(rr.fetchOne()) row;
    while ((row = rr.fetchOne())) {
        if (!found) {
            cout << "\n--- Transactions for User ID: " << userID << " ---\n";
            found = true;
        }
        printTransactionDetails(
            row[0].get<int>(), row[1].get<int>(), row[2].get<int>(), row[3].get<int>(),
            row[4].get<string>(), row[5].get<string>(),
            row[6].isNull() ? string() : row[6].get<string>(),
            row[7].get<double>()
        );
    }

    if (!found) {
        cout << "No transactions found for User ID: " << userID << endl;
    }
}

//Return Book
bool returnBook(int userID, int transactionID) {
    // 1. Retrieve transaction details
    auto rrTransaction = transactionTable.select("BookID", "CopyID", "ReturnDate", "DueDate")
        .where("TransactionID = :tid AND UserID = :uid")
        .bind("tid", transactionID)
        .bind("uid", userID)
        .execute();
    auto rowTransaction = rrTransaction.fetchOne();

    if (!rowTransaction) {
        std::cout << "Error: Transaction not found or does not belong to you." << std::endl;
        return false;
    }

    if (!rowTransaction[2].isNull()) {
        std::cout << "Error: Transaction " << transactionID << " already marked as returned on " << rowTransaction[2].get<std::string>() << std::endl;
        return false;
    }

    int bookID = rowTransaction[0].get<int>();
    int copyID = rowTransaction[1].get<int>();
    std::string dueDate = rowTransaction[3].get<std::string>();

    // 2. Get current return date from the database
    mysqlx::SqlResult rrReturnDate = sess.sql("SELECT CURRENT_DATE()").execute();
    mysqlx::Row rowReturnDate = rrReturnDate.fetchOne();
    if (!rowReturnDate || rowReturnDate[0].isNull()) {
        std::cerr << "Error getting current date from database." << std::endl;
        return false;
    }
    std::string actualReturnDateSQL = rowReturnDate[0].get<std::string>();

    // 3. Complete the transaction (update ReturnDate and Fine in one query)
    std::stringstream ssUpdateQuery;
    ssUpdateQuery << "UPDATE transaction "
        "SET ReturnDate = '" << actualReturnDateSQL << "', "
        "Fine = IF(DATEDIFF('" << actualReturnDateSQL << "', '" << dueDate << "') > 0, "
        "DATEDIFF('" << actualReturnDateSQL << "', '" << dueDate << "') * 0.5, 0) "
        "WHERE TransactionID = " << transactionID;

    mysqlx::SqlResult updateResult = sess.sql(ssUpdateQuery.str()).execute();

    if (updateResult.getAffectedItemsCount() > 0) {
        std::cout << "Book returned successfully." << std::endl;

        // 4. Update book copy status to "Available"
        auto updateBookStatusResult = bookTable.update()
            .set("Status", "Available")
            .where("BookID = :bid AND CopyID = :cid")
            .bind("bid", bookID)
            .bind("cid", copyID)
            .execute();

        if (updateBookStatusResult.getAffectedItemsCount() > 0) {
            // 5. Retrieve the calculated fine to display to the user
            mysqlx::RowResult rrFine = transactionTable.select("Fine")
                .where("TransactionID = :tid")
                .bind("tid", transactionID)
                .execute();
            mysqlx::Row rowFine = rrFine.fetchOne();
            if (rowFine && !rowFine[0].isNull()) {
                double fineAmount = rowFine[0].get<double>();
                if (fineAmount > 0) {
                    std::cout << "Fine for this return: $" << fineAmount << std::endl;
                }
            }
            return true;
        }
        else {
            std::cout << "Warning: Could not update book copy status." << std::endl;
            return false;
        }
    }
    else {
        std::cout << "Error: Failed to mark transaction as returned or update fine." << std::endl;
        return false;
    }
}
// View all transactions (for librarian)
inline void viewAllTransactions() {
    auto rr = transactionTable.select("*")
        .orderBy("TransactionID DESC").execute();


    bool found = false;
    decltype(rr.fetchOne()) row;
    while ((row = rr.fetchOne())) {
        if (!found) {
            cout << "\n--- All Library Transactions ---\n";
            found = true;
        }
        printTransactionDetails(
            row[0].get<int>(), row[1].get<int>(), row[2].get<int>(), row[3].get<int>(),
            row[4].get<string>(), row[5].get<string>(),
            row[6].isNull() ? string() : row[6].get<string>(),
            row[7].get<double>()
        );
    }

    if (!found) {
        cout << "No transactions found in the system." << endl;
    }
}
