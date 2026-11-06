# Library Management System (C++)

A console-based library system built with object-oriented design and file persistence.

## Features
- **Roles:** register and log in as Admin or User/Student
- **Admin:** add books, remove books, view all books
- **Student:** view books, borrow a book (one at a time), return a book
- Input validation for email (must contain `@` and end with `.com`) and phone (11 digits)
- Data saved to `books.txt` and `users.txt`, so it persists between runs

## OOP concepts used
Abstract base class `User` with virtual methods, `Admin` and `Student` subclasses (inheritance and polymorphism), `Book`, `Library` and `System` classes, encapsulation.

## Build & run
```bash
g++ -std=c++17 main.cpp -o library
./library
```
Sample data is in `books.txt` and `users.txt` (demo accounts only, e.g. `admin2` / `adminpass`).

## Tech
C++, STL (`vector`, `fstream`, `algorithm`), OOP

## Authors
Ayan Aqeel, Abdul Rehman, Hamza Sheikh — Bahria University Islamabad.
