// Get Transaction table globally
mysqlx::Table transactionTable = db.getTable("transaction");

// THis is the default fine rate
double FINE_RATE = 0.5;


void changeFineRate() {
    cout << "Current fine rate is $" << FINE_RATE << " per day." << endl;
    cout << "Enter new fine rate: ";
    cin >> FINE_RATE;

    if (FINE_RATE < 0) {
        cout << "Invalid fine rate. Reverting to default ($0.5)." << endl;
        FINE_RATE = 0.5;
    }
    else {
        cout << "Fine rate updated to $" << FINE_RATE << " per day." << endl;
    }
}
inline void MostPopularBooks() {
    cout << "\n--- Most Popular Books ---\n";

    try {

        auto result = sess.sql(R"(
            SELECT b.BookID, b.Title, COUNT(t.TransactionID) AS BorrowCount
            FROM Transaction t
            JOIN Book b ON t.BookID = b.BookID
            GROUP BY b.BookID, b.Title
            ORDER BY BorrowCount DESC
        )").execute();

        int rank = 1;
        mysqlx::Row row;
        while ((row = result.fetchOne())) {
            int bookID = row[0].get<int>();
            string title = row[1].get<string>();
            int borrowCount = row[2].get<int>();

            cout << rank++ << ". " << title << " (ID: " << bookID << ") - Borrowed " << borrowCount << " times\n";
        }
    }
    catch (const mysqlx::Error& err) {
        cerr << "SQL Error while listing popular books: " << err.what() << endl;
    }
}

// Helps change mysql date to string
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>
using namespace chrono;

// used to convert the chrono time to 'YYYY-MM-DD' format
string formatDateToString(const system_clock::time_point& tp) {
    time_t tt = system_clock::to_time_t(tp);
    struct tm localTime;

    localtime_s(&localTime, &tt);

    stringstream ss;
    ss << put_time(&localTime, "%Y-%m-%d");
    return ss.str();
}
// Print a transaction's details
inline void printTransactionDetails(
    int transactionID,
    int userID,
    int bookID,
    int copyID,
    string borrowDate,
    string dueDate,
    string returnDate,
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
    int copyID
) {
    // Get the current date (borrow date) using C++ chrono
    auto now = std::chrono::system_clock::now();
    string borrowDateSQL = formatDateToString(now);

    // Calculate the due date (15 days later)
    auto dueDate = now + hours(24 * 15); 
    string dueDateSQL = formatDateToString(dueDate);

    // Determine new transaction ID
    auto rr_max_id = transactionTable.select("MAX(TransactionID)").execute();
    auto row_max_id = rr_max_id.fetchOne();
    int newTransactionID = 1;
    if (row_max_id && !row_max_id[0].isNull()) {
        newTransactionID = row_max_id[0].get<int>() + 1;
    }

    // Log the values for debugging purposes
    cout << "Transaction Date (Borrow Date): " << borrowDateSQL << std::endl;
    cout << "Transaction Date (Due Date): " << dueDateSQL << std::endl;

    try {
        // Insert into the transaction table
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
        .where("BookID = :bid AND Status = 'available'")
        .bind("bid", bookID)
        .execute();
    mysqlx::Row rowAvailableCopy = rrAvailableCopy.fetchOne();

    if (!rowAvailableCopy) {
        std::cout << "Sorry, book with ID " << bookID << " is currently not available." << std::endl;
        char userInput;
        std::cout << "Would you like to reserve it? (y/n): ";
        std::cin >> userInput;

        if (userInput == 'y' || userInput == 'Y') {

            reserveBook(userID, bookID);
        }
        else {
            // User doesn't want to reserve
            std::cout << "You chose not to reserve the book." << std::endl;
            return false;
        }
        return false;
    }

    int copyIDToBorrow = rowAvailableCopy[0].get<int>();

    // 2. Create the transaction (dates handled inside)
    if (createTransaction(userID, bookID, copyIDToBorrow)) {
        // 3. Update the book copy status to 'Borrowed'
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
            return false;
        }
    }
    else {
        std::cerr << "Borrow operation failed while creating transaction." << std::endl;
        return false;
    }
}


// View transactions for a specific user
inline void viewUserTransactions(int userID) {
    auto rr = transactionTable.select("TransactionID", "UserID", "BookID", "CopyID",
        "CAST(BorrowDate AS CHAR)",
        "CAST(DueDate AS CHAR)",
        "CAST(ReturnDate AS CHAR)",
        "Fine")
        .where("UserID = :uid")
        .orderBy("BorrowDate DESC")
        .bind("uid", userID)
        .execute();

    bool found = false;
    mysqlx::Row row;
    while ((row = rr.fetchOne())) {
        if (!found) {
            cout << "\n--- Transactions for User ID: " << userID << " ---\n";
            found = true;
        }

        // carefully handle date time
        string borrowDate = row[4].isNull() ? "Not Available" : row[4].get<string>();
        string dueDate = row[5].isNull() ? "Not Available" : row[5].get<string>();
        string returnDate = row[6].isNull() ? "Not Returned" : row[6].get<string>();

        printTransactionDetails(
            row[0].get<int>(), row[1].get<int>(), row[2].get<int>(), row[3].get<int>(),
            borrowDate, dueDate, returnDate, row[7].get<double>()
        );
    }

    if (!found) {
        cout << "No transactions found for User ID: " << userID << endl;
    }
}

//Return Book
bool returnBook(int userID, int transactionID) {
    // 1. Retrieve transaction details with CAST to CHAR to avoid parsing issues
    auto rrTransaction = transactionTable
        .select("BookID", "CopyID", "CAST(ReturnDate AS CHAR)", "CAST(DueDate AS CHAR)")
        .where("TransactionID = :tid AND UserID = :uid")
        .bind("tid", transactionID)
        .bind("uid", userID)
        .execute();

    auto rowTransaction = rrTransaction.fetchOne();

    if (!rowTransaction) {
        cout << "Error: Transaction not found or does not belong to you." << endl;
        return false;
    }

    // Check if book is already returned
    string returnDate = rowTransaction[2].isNull() ? "" : rowTransaction[2].get<string>();
    if (!returnDate.empty()) {
        cout << "Error: Transaction " << transactionID << " already marked as returned on "
            << returnDate << endl;
        return false;
    }

    int bookID = rowTransaction[0].get<int>();
    int copyID = rowTransaction[1].get<int>();
    string dueDate = rowTransaction[3].isNull() ? "" : rowTransaction[3].get<string>();

    if (dueDate.empty()) {
        cerr << "Error: Due date is missing or invalid." << endl;
        return false;
    }

    // 2. Get current return date
    auto now = chrono::system_clock::now();
    string actualReturnDateSQL = formatDateToString(now);

    // 3. Calculate fine
    tm tmDue = {};
    istringstream ssDue(dueDate);
    if (!(ssDue >> get_time(&tmDue, "%Y-%m-%d"))) {
        cerr << "Error: Failed to parse due date: " << dueDate << endl;
        return false;
    }

    time_t due_time = mktime(&tmDue);
    time_t return_time = chrono::system_clock::to_time_t(now);

    if (due_time == -1) {
        cerr << "Error: Invalid due date." << endl;
        return false;
    }
    // returns diff in seconds so needs to be converted to days
    int daysLate = difftime(return_time, due_time) / (60 * 60 * 24);
    double fine = (daysLate > 0) ? daysLate * FINE_RATE : 0.0;

    // 4. Complete the transaction
    try {
        auto result = transactionTable.update()
            .set("ReturnDate", actualReturnDateSQL)
            .set("Fine", fine)
            .where("TransactionID = :tid")
            .bind("tid", transactionID)
            .execute();

        // No need to check affected count here — assume update success if no exception

        // 5. Update book copy status
        bookTable.update()
            .set("Status", "available")
            .where("BookID = :bid AND CopyID = :cid")
            .bind("bid", bookID)
            .bind("cid", copyID)
            .execute();

        cout << "Book returned successfully on " << actualReturnDateSQL << "." << endl;
        if (fine > 0.0) {
            cout << "Late return fine: $" << fine << endl;
        }

        return true;
    }
    catch (const mysqlx::Error& err) {
        cerr << "SQL Error while completing return: " << err.what() << endl;
        return false;
    }
}



// View all transactions (for librarian)
inline void viewAllTransactions() {
    auto rr = transactionTable.select(
        "TransactionID", "UserID", "BookID", "CopyID",
        "CAST(BorrowDate AS CHAR)",
        "CAST(DueDate AS CHAR)",
        "CAST(ReturnDate AS CHAR)",
        "Fine"
    )
        .orderBy("TransactionID DESC")
        .execute();

    bool found = false;
    decltype(rr.fetchOne()) row;
    while ((row = rr.fetchOne())) {
        if (!found) {
            cout << "\n--- All Library Transactions ---\n";
            found = true;
        }

        string borrowDate = row[4].isNull() ? "N/a" : row[4].get<string>();
        string dueDate = row[5].isNull() ? "N/A" : row[5].get<string>();
        string returnDate = row[6].isNull() ? "Not Returned" : row[6].get<string>();

        printTransactionDetails(
            row[0].get<int>(),  // TransactionID
            row[1].get<int>(),  // UserID
            row[2].get<int>(),  // BookID
            row[3].get<int>(),  // CopyID
            borrowDate,
            dueDate,
            returnDate,
            row[7].get<double>() // Fine
        );
    }

    if (!found) {
        cout << "No transactions found in the system." << endl;
    }
}

