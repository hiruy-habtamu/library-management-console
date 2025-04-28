-- Users Table: Add some dummy users with first and last names
INSERT INTO User (FirstName, LastName, Email, Role, Password, Status, UniversityID)
VALUES 
    ('Alice', 'Johnson', 'alice@example.com', 'Student', 'password123', 'Active', 'U12345'),
    ('Bob', 'Smith', 'bob@example.com', 'Faculty', 'password456', 'Active', 'U67890'),
    ('Charlie', 'Brown', 'charlie@example.com', 'Librarian', 'password789', 'Active', 'U11223'),
    ('David', 'White', 'david@example.com', 'Student', 'password101', 'Inactive', 'U44556');

-- Books Table: Add some books with multiple copies
INSERT INTO Book (BookID, CopyID, Title, Author, ISBN, PublicationYear, Category, Status)
VALUES 
    (1, 1, 'Introduction to C++', 'John Doe', '978-3-16-148410-0', 2020, 'academic', 'available'),
    (1, 2, 'Introduction to C++', 'John Doe', '978-3-16-148410-0', 2020, 'academic', 'available'),
    (2, 1, 'Data Structures and Algorithms', 'Jane Smith', '978-0-123-45678-9', 2018, 'academic', 'borrowed'),
    (2, 2, 'Data Structures and Algorithms', 'Jane Smith', '978-0-123-45678-9', 2018, 'academic', 'reserved'),
    (3, 1, 'The Great Gatsby', 'F. Scott Fitzgerald', '978-1-234-56789-0', 1925, 'fiction', 'available'),
    (4, 1, '1984', 'George Orwell', '978-1-234-56780-7', 1949, 'fiction', 'borrowed'),
    (5, 1, 'Python Programming', 'Jake Williams', '978-1-111-22222-3', 2021, 'non-fiction', 'available'),
    (6, 1, 'The History of Time', 'Stephen Hawking', '978-0-345-39180-3', 1988, 'non-fiction', 'available');

-- Transactions Table: Add some dummy transactions (book borrowings)
INSERT INTO Transaction (UserID, BookID, CopyID, BorrowDate, DueDate, Fine)
VALUES 
    (1, 2, 1, '2025-04-01', '2025-04-15', 0.00),
    (2, 3, 1, '2025-03-20', '2025-04-03', 5.00),
    (4, 1, 2, '2025-04-05', '2025-04-19', 0.00),
    (3, 4, 1, '2025-04-02', '2025-04-16', 2.00);

-- Reservations Table: Add some dummy reservations (for reserved books)
INSERT INTO Reservation (UserID, BookID, CopyID, ReservationDate)
VALUES 
    (1, 2, 2, '2025-04-07 14:00:00'),
    (2, 1, 1, '2025-04-08 15:30:00'),
    (4, 3, 1, '2025-04-09 10:15:00'),
    (3, 6, 1, '2025-04-10 09:45:00');
