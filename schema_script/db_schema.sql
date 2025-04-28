CREATE DATABASE IF NOT EXISTS library_db;
USE library_db;

-- Users Table: Split name into first_name and last_name
CREATE TABLE IF NOT EXISTS User (
    UserID INT AUTO_INCREMENT PRIMARY KEY,
    FirstName VARCHAR(255) NOT NULL,
    LastName VARCHAR(255) NOT NULL,
    Email VARCHAR(255) NOT NULL UNIQUE,
    Role ENUM('Librarian', 'Student', 'Faculty') NOT NULL,
    Password VARCHAR(255) NOT NULL,
    Status ENUM('Active', 'Inactive') DEFAULT 'Active',
    UniversityID VARCHAR(50) NOT NULL
);

-- Books Table
CREATE TABLE IF NOT EXISTS Book (
    BookID INT NOT NULL,
    CopyID INT NOT NULL,
    Title VARCHAR(255) NOT NULL,
    Author VARCHAR(255) NOT NULL,
    ISBN VARCHAR(20) NOT NULL,
    PublicationYear YEAR NOT NULL,
    Category ENUM('fiction', 'non-fiction', 'academic', 'other') NOT NULL,
    Status ENUM('available', 'borrowed', 'reserved') NOT NULL DEFAULT 'available',
    PRIMARY KEY (BookID, CopyID)  -- Primary key on BookID and CopyID together
);

-- Transactions Table
CREATE TABLE IF NOT EXISTS Transaction (
    TransactionID INT AUTO_INCREMENT PRIMARY KEY,
    UserID INT NOT NULL,
    BookID INT NOT NULL,
    CopyID INT NOT NULL,
    BorrowDate DATE NOT NULL,
    DueDate DATE NOT NULL,
    ReturnDate DATE,
    Fine DECIMAL(10, 2) DEFAULT 0.00,
    FOREIGN KEY (UserID) REFERENCES User(UserID),
    FOREIGN KEY (BookID, CopyID) REFERENCES Book(BookID, CopyID)
);

-- Reservations Table (References both BookID and CopyID)
CREATE TABLE IF NOT EXISTS Reservation (
    ReservationID INT AUTO_INCREMENT PRIMARY KEY,
    UserID INT NOT NULL,
    BookID INT NOT NULL,
    CopyID INT NOT NULL,
    ReservationDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (UserID) REFERENCES User(UserID),
    FOREIGN KEY (BookID, CopyID) REFERENCES Book(BookID, CopyID)
);