mysqlx::Table reservationTable = db.getTable("reservation");
inline void viewReservedBooks() {
    cout << "\n--- Reserved Books List ---\n";

    try {
        auto result = sess.sql(R"(
            SELECT r.ReservationID, r.UserID, u.FirstName, r.BookID, b.Title, CAST(r.ReservationDate AS CHAR)
            FROM reservation r
            JOIN Book b ON r.BookID = b.BookID
            JOIN user u ON r.UserID = u.UserID
            ORDER BY r.ReservationDate DESC
        )").execute();

        mysqlx::Row row;
        bool found = false;
        while ((row = result.fetchOne())) {
            found = true;
            int reservationID = row[0].get<int>();
            int userID = row[1].get<int>();
            string userName = row[2].get<string>();
            int bookID = row[3].get<int>();
            string title = row[4].get<string>();
            string reservationDate = row[5].get<string>();

            cout << "Reservation ID: " << reservationID << "\n"
                << "User ID: " << userID << " | Name: " << userName << "\n"
                << "Book ID: " << bookID << " | Title: " << title << "\n"
                << "Date Reserved: " << reservationDate << "\n"
                << "-----------------------------\n";
        }

        if (!found) {
            cout << "No reserved books found.\n";
        }
    }
    catch (const mysqlx::Error& err) {
        cerr << "SQL Error while retrieving reserved books: " << err.what() << endl;
    }
}

void reserveBook(int userID, int bookID) {
    try {
        // Check how many copies are borrowed 
        mysqlx::RowResult rrBorrowed = bookTable.select("CopyID")
            .where("BookID = :bid AND Status = 'borrowed'").bind("bid", bookID).execute();

        // Check the total number of copies of the book
        mysqlx::RowResult rrTotal = bookTable.select("CopyID")
            .where("BookID = :bid").bind("bid", bookID).execute();

        // Check how many copies are already reserved
        mysqlx::RowResult rrReserved = reservationTable.select("CopyID")
            .where("BookID = :bid").bind("bid", bookID).execute();

        // If all copies are borrowed and the number of reserved copies is equal to total copies cannot reserve
        if (rrBorrowed.count() == 0 && rrReserved.count() == rrTotal.count()) {
            cout << "Sorry, all copies of this book are already reserved. No further reservations can be made." << endl;
            return;
        }

        // if the borrowed count is greater than 0 that means we can reserve
        if (rrBorrowed.count()>0) {
                // Find a copy that is borrowed and available for reservation
                mysqlx::Row row = rrBorrowed.fetchOne();
                int copyID = row[0].get<int>();

                // Insert reservation for the user
                reservationTable.insert("UserID", "CopyID", "BookID")
                    .values(userID, copyID, bookID).execute();

                // Update the book status to 'Reserved' for the selected copy
                bookTable.update()
                    .set("Status", "reserved")
                    .where("BookID = :bid AND CopyID = :cid")
                    .bind("bid", bookID)
                    .bind("cid", copyID)
                    .execute();

                cout << "The book has been successfully reserved!" << endl;
        }
        else {
            // Book is not fully borrowed (some copies are still available)
            cout << "Sorry, some copies of this book are still available, and cannot be reserved right now." << endl;
        }
    }
    catch (const mysqlx::Error& err) {
        cerr << "Error while reserving the book: " << err.what() << endl;
    }
}

inline void viewUserReservations(int userID) {
    try {

        mysqlx::SqlResult reservations = sess.sql(R"(
            SELECT r.BookID, b.Title, r.CopyID, r.ReservationID
            FROM reservation r
            JOIN book b ON r.BookID = b.BookID
            WHERE r.UserID = ? AND Status = 'reserved'
        )").bind(userID).execute();

        if (reservations.count() == 0) {
            std::cout << "You have no current reservations." << std::endl;
            return;
        }

        std::cout << "Your current reservations:\n";
        std::cout << "------------------------------------------\n";
        for (mysqlx::Row row : reservations) {
            int bookID = row[0].get<int>();
            std::string title = row[1].get<std::string>();
            int copyID = row[2].get<int>();
			int reservationID = row[3].get<int>();

            std::cout << "Resv. ID:"<< reservationID << ", BookID: " << bookID
                << ", Title: \"" << title << "\""
                << ", CopyID: " << copyID << std::endl;
        }
    }
    catch (const mysqlx::Error& err) {
        std::cerr << "Error while viewing reservations: " << err.what() << std::endl;
    }
}

void removeReservation(int reservationID, int userID) {
    try {
        // 1. Check if the reservation exists and belongs to the user
        mysqlx::RowResult rrReservation = reservationTable.select("CopyID", "UserID","BookID")
            .where("ReservationID = :resID")
            .bind("resID", reservationID)
            .execute();

        if (rrReservation.count() == 0) {
            std::cout << "Reservation not found." << std::endl;
            return;
        }

        mysqlx::Row rowReservation = rrReservation.fetchOne();
        int reservedCopyID = rowReservation[0].get<int>();
        int reservedUserID = rowReservation[1].get<int>();
		int reservedBookID = rowReservation[2].get<int>();

        // Verify if the reservation belongs to the user
        if (reservedUserID != userID) {
            std::cout << "This reservation does not belong to you." << std::endl;
            return;
        }

        // 2. Check the status of the book copy
        mysqlx::RowResult rrStatus = bookTable.select("Status")
            .where("CopyID = :copyID AND BookID = :bookID")
			.bind("copyID", reservedCopyID).bind("bookID", reservedBookID)
            .execute();

        if (rrStatus.count() == 0) {
            std::cout << "Copy not found." << std::endl;
            return;
        }

        mysqlx::Row rowStatus = rrStatus.fetchOne();
        std::string bookStatus = rowStatus[0].get<std::string>();

        // 3. If the book copy is reserved, change it back to 'Borrowed'
        if (bookStatus == "reserved") {
            auto updateStatus = bookTable.update()
                .set("Status", "borrowed")
                .where("CopyID = :copyID AND BookID = :bookID")
				.bind("copyID", reservedCopyID).bind("bookID", reservedBookID)
                .execute();
			cout << "Book status updated to 'borrowed'." << endl;

        }

        auto deleteReservation = reservationTable.remove()
            .where("ReservationID = :resID")
            .bind("resID", reservationID)
            .execute();

        if (deleteReservation.getAffectedItemsCount() > 0) {
            std::cout << "Reservation removed successfully." << std::endl;
        }
        else {
            std::cout << "Failed to remove reservation." << std::endl;
        }
    }
    catch (const mysqlx::Error& err) {
        std::cerr << "Error while removing reservation: " << err.what() << std::endl;
    }
}

