
# PaymentProcessor Project

## Overview

The **PaymentProcessor** project is a C++ application that simulates a payment processing system. It interacts with a PostgreSQL database to handle payment transactions, retrieve transaction history, and manage accounts and cards.

### Features:
- Process payment transactions, including applying merchant and issuer fees.
- Retrieve transaction history for a specific card, with optional date filtering, the date format is 'YYYY/MM/DD'.
- Database interaction using PostgreSQL (via the `libpqxx` library).
- Unit tests using Google Test framework to ensure correctness.

## Project Structure

```
PaymentProcessor/
├── CMakeLists.txt             # CMake configuration for building the project
├── README.md                  # Project documentation
├── src/
│   ├── PaymentProcessor.cpp   # Implementation of the PaymentProcessor class
│   ├── PaymentProcessor.h     # Header file defining the PaymentProcessor class
│   ├── main.cpp               # Main program that interacts with the user
├── tests/
│   └── test_payment.cpp       # Unit tests for the PaymentProcessor class
├── sql/
│   ├── setup_data_example.sql # Example SQL for setting up test data
│   └── setup_database.sql     # SQL to create the database and schema
```
## Requirements

To run the PaymentProcessor project, you will need the following:

- **C++ Compiler** supporting C++11 or later (e.g., GCC, Clang, or MSVC).
- **CMake** (version 3.10 or later).
- **PostgreSQL** (version 9.5 or later).
- **libpqxx** library (C++ client for PostgreSQL).
- **Google Test** library (for running unit tests).

## Setup Instructions

### 1. Install PostgreSQL

Ensure PostgreSQL is installed and running on your system. You can follow the installation instructions on the [official PostgreSQL website](https://www.postgresql.org/download/).

### 2. Install Required Libraries

You will need the following libraries:

- **libpqxx**: This is a C++ client library for PostgreSQL.
- **Google Test**: This is used for unit testing.


### 3. Database Setup

1. **Create the Database**:

   You can use the provided `setup_database.sql` script to create the database and necessary tables:

   ```bash
   psql -U postgres -f sql/setup_database.sql
   ```

2. **Populate the Database**:

   If you want to populate the database with example data, use the `setup_data_example.sql` script:

   ```bash
   psql -U postgres -d paymentdb -f sql/setup_data_example.sql
   ```

   This will populate the database with example accounts, cards, and transactions.

### 4. Building the Project

1. **Clone the repository**:

   Clone the project repository to your local machine:

   ```bash
   git clone <repository_url>
   cd PaymentProcessor
   ```

2. **Run CMake**:

   Create a build from Makefile (In this case with MinGW):

   ```bash
   cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_C_COMPILER=C:/msys64/mingw64/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/msys64/mingw64/bin/g++.exe
   ```

3. **Build the project**:

   Run CMake to configure the build system:

   ```bash
   cmake --build build
   ```

### 5. Running the Application

Once the project is built, you can run the `main` program as follows:

```bash
./build/PaymentProcessor
```

It will prompt you to enter commands like `payment`, `history`, or `exit` to interact with the payment system.

### 6. Running Unit Tests

Unit tests are located in `tests/test_payment.cpp`. You can run the tests using Google Test:

1. **Build the tests**:

   In the previus steps were created the tests

2. **Run the tests**:

   Once the tests are built, you can run them with:

   ```bash
   ./build/TestPayment.exe
   ```

   This will execute all the test cases and show the results.
