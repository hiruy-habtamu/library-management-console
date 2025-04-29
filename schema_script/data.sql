-- MySQL dump 10.13  Distrib 8.0.41, for Win64 (x86_64)
--
-- Host: localhost    Database: library_db
-- ------------------------------------------------------
-- Server version	8.0.41

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `book`
--

DROP TABLE IF EXISTS `book`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `book` (
  `BookID` int NOT NULL,
  `CopyID` int NOT NULL,
  `Title` varchar(255) NOT NULL,
  `Author` varchar(255) NOT NULL,
  `ISBN` varchar(20) NOT NULL,
  `PublicationYear` year NOT NULL,
  `Category` enum('fiction','non-fiction','academic','other') NOT NULL,
  `Status` enum('available','borrowed','reserved') NOT NULL DEFAULT 'available',
  PRIMARY KEY (`BookID`,`CopyID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `book`
--

LOCK TABLES `book` WRITE;
/*!40000 ALTER TABLE `book` DISABLE KEYS */;
INSERT INTO `book` VALUES (1,1,'Introduction to C++','John Doe','978-3-16-148410-0',2020,'academic','available'),(1,2,'Introduction to C++','John Doe','978-3-16-148410-0',2020,'academic','available'),(2,1,'Data Structures and Algorithms','Jane Smith','978-0-123-45678-9',2018,'academic','borrowed'),(2,2,'Data Structures and Algorithms','Jane Smith','978-0-123-45678-9',2018,'academic','reserved'),(3,1,'The Great Gatsby','F. Scott Fitzgerald','978-1-234-56789-0',1925,'fiction','available'),(4,1,'1984','George Orwell','978-1-234-56780-7',1949,'fiction','borrowed'),(5,1,'Python Programming','Jake Williams','978-1-111-22222-3',2021,'non-fiction','available'),(6,1,'The History of Time','Stephen Hawking','978-0-345-39180-3',1988,'non-fiction','available');
/*!40000 ALTER TABLE `book` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `reservation`
--

DROP TABLE IF EXISTS `reservation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `reservation` (
  `ReservationID` int NOT NULL AUTO_INCREMENT,
  `UserID` int NOT NULL,
  `BookID` int NOT NULL,
  `CopyID` int NOT NULL,
  `ReservationDate` datetime DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`ReservationID`),
  KEY `UserID` (`UserID`),
  KEY `BookID` (`BookID`,`CopyID`),
  CONSTRAINT `reservation_ibfk_1` FOREIGN KEY (`UserID`) REFERENCES `user` (`UserID`),
  CONSTRAINT `reservation_ibfk_2` FOREIGN KEY (`BookID`, `CopyID`) REFERENCES `book` (`BookID`, `CopyID`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `reservation`
--

LOCK TABLES `reservation` WRITE;
/*!40000 ALTER TABLE `reservation` DISABLE KEYS */;
INSERT INTO `reservation` VALUES (1,1,2,2,'2025-04-07 14:00:00'),(2,2,1,1,'2025-04-08 15:30:00'),(3,4,3,1,'2025-04-09 10:15:00'),(4,3,6,1,'2025-04-10 09:45:00');
/*!40000 ALTER TABLE `reservation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `transaction`
--

DROP TABLE IF EXISTS `transaction`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `transaction` (
  `TransactionID` int NOT NULL AUTO_INCREMENT,
  `UserID` int NOT NULL,
  `BookID` int NOT NULL,
  `CopyID` int NOT NULL,
  `BorrowDate` date NOT NULL,
  `DueDate` date NOT NULL,
  `ReturnDate` date DEFAULT NULL,
  `Fine` decimal(10,2) DEFAULT '0.00',
  PRIMARY KEY (`TransactionID`),
  KEY `UserID` (`UserID`),
  KEY `BookID` (`BookID`,`CopyID`),
  CONSTRAINT `transaction_ibfk_1` FOREIGN KEY (`UserID`) REFERENCES `user` (`UserID`),
  CONSTRAINT `transaction_ibfk_2` FOREIGN KEY (`BookID`, `CopyID`) REFERENCES `book` (`BookID`, `CopyID`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `transaction`
--

LOCK TABLES `transaction` WRITE;
/*!40000 ALTER TABLE `transaction` DISABLE KEYS */;
INSERT INTO `transaction` VALUES (1,1,2,1,'2025-04-01','2025-04-15',NULL,0.00),(2,2,3,1,'2025-03-20','2025-04-03',NULL,5.00),(3,4,1,2,'2025-04-05','2025-04-19',NULL,0.00),(4,3,4,1,'2025-04-02','2025-04-16',NULL,2.00);
/*!40000 ALTER TABLE `transaction` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `user` (
  `UserID` int NOT NULL AUTO_INCREMENT,
  `FirstName` varchar(255) NOT NULL,
  `LastName` varchar(255) NOT NULL,
  `Email` varchar(255) NOT NULL,
  `Role` enum('Librarian','Student','Faculty') NOT NULL,
  `Password` varchar(255) NOT NULL,
  `Status` enum('Active','Inactive') DEFAULT 'Active',
  `UniversityID` varchar(50) NOT NULL,
  PRIMARY KEY (`UserID`),
  UNIQUE KEY `Email` (`Email`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user`
--

LOCK TABLES `user` WRITE;
/*!40000 ALTER TABLE `user` DISABLE KEYS */;
INSERT INTO `user` VALUES (1,'Alice','Johnson','alice@example.com','Student','password123','Active','U12345'),(2,'Bob','Smith','bob@example.com','Faculty','password456','Active','U67890'),(3,'Charlie','Brown','charlie@example.com','Librarian','password789','Active','U11223'),(4,'David','White','david@example.com','Student','password101','Inactive','U44556');
/*!40000 ALTER TABLE `user` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2025-04-29 21:33:08
