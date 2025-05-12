
void exportBorrowedBooks() {
    try {

        std::ofstream file("logs/borrowed_books.csv");
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing.\n";
            return;
        }

        file << "BookID,UserID,CopyID,BorrowedDate\n";

        // Use aliasing to make sure column has name
        auto result = transactionTable
            .select("BookID", "UserID", "CopyID","CAST(BorrowDate AS CHAR)")
            .execute();

        for (auto row : result) {
            if (row.colCount() >= 4 && !row[0].isNull() && !row[1].isNull() &&
                !row[2].isNull() && !row[3].isNull()) {

                file << row[0].get<int>() << ","
                    << row[1].get<int>() << ","
                    << row[2].get<int>() << ","
                    << row[3].get<std::string>() << "\n";
            }
        }

        file.close();
        std::cout << "Borrowed books exported to logs/borrowed_books.csv\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Exception while exporting borrowed books: " << e.what() << "\n";
    }
}


void exportOverdueBooks() {
    try {
        std::ofstream file("logs/overdue_books.csv");

        file << "BookID,UserID,CopyID,BorrowDate,DueDate\n";

        auto result = transactionTable
            .select("BookID", "UserID", "CopyID",
                "CAST(BorrowDate AS CHAR)", "CAST(DueDate AS CHAR)")
            .where("DueDate > CURDATE()")
            .execute();

        for (auto row : result) {
            if (row.colCount() >= 5 && !row[0].isNull() && !row[1].isNull() &&
                !row[2].isNull() && !row[3].isNull() && !row[4].isNull()) {

                file << row[0].get<int>() << ","
                    << row[1].get<int>() << ","
                    << row[2].get<int>() << ","
                    << row[3].get<std::string>() << ","
                    << row[4].get<std::string>() << "\n";
            }
        }

        file.close();
        std::cout << "Overdue books exported to logs/overdue_books.csv\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Exception while exporting overdue books: " << e.what() << "\n";
    }
}

void exportMonthlyBorrowing() {
    try {
        std::ofstream file("logs/monthly_borrowing.csv");

        file << "Month,TotalBorrowed\n";

        std::string query = "SELECT DATE_FORMAT(BorrowDate, '%Y-%m') AS Month, COUNT(*) AS TotalBorrowed "
            "FROM transaction  GROUP BY Month ORDER BY Month";

        auto result = sess.sql(query).execute();

        for (auto row : result) {
            file << row[0].get<std::string>() << "," << row[1].get<int>() << "\n";
        }

        file.close();
        std::cout << "Monthly summary exported to logs/monthly_borrow_summary.csv\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Exception while exporting monthly summary: " << e.what() << "\n";
    }
}


void exportPopularBooks() {
    try {
        std::ofstream file("logs/popular_books.csv");

        file << "BookID,TimesBorrowed\n";

        std::string query = "SELECT BookID, COUNT(*) AS TimesBorrowed "
            "FROM transaction GROUP BY BookID ORDER BY TimesBorrowed DESC";

        auto result = sess.sql(query).execute();

        for (auto row : result) {
            file << row[0].get<int>() << "," << row[1].get<int>() << "\n";
        }

        file.close();
        std::cout << "Popular books exported to logs/popular_books.csv\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Exception while exporting popular books: " << e.what() << "\n";
    }
}


void exportFinesSummary() {
    try {
        std::ofstream file("logs/fine_report.csv");

        file << "UserID,BookID,Fine\n";

        auto result = sess.sql(
            "SELECT UserID, BookID, Fine FROM transaction WHERE ReturnDate IS NOT NULL"
        ).execute();

        for (auto row : result) {
            file << row[0].get<int>() << ","
                << row[1].get<int>() << ","
                << row[2].get<double>() << "\n";
        }

        file.close();
        std::cout << "Fine report exported to logs/fine_report.csv\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Exception while exporting fine report: " << e.what() << "\n";
    }
}
