# Bank Exercise

A C++ banking application with REST API server, transaction support, and comprehensive unit tests. Features customer and enterprise account management with deposit, withdraw, and transfer operations.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Project Structure](#project-structure)
- [Building the Project](#building-the-project)
- [Running the Application](#running-the-application)
- [Running Unit Tests](#running-unit-tests)
- [API Documentation](#api-documentation)

## Prerequisites

- **CMake** 3.10 or higher
- **C++ Compiler** with C++17 support (g++, clang, or MSVC)
- **git** (for fetching dependencies)
- **curl** or Postman (optional, for testing API endpoints)

### On Linux/macOS:
```bash
# Ubuntu/Debian
sudo apt-get install cmake build-essential

# macOS
brew install cmake
```

### On Windows:
Download and install from [cmake.org](https://cmake.org/download/)

## Project Structure

```
Bank_Exercise/
├── Include/                    # Header files
│   ├── Account.h
│   ├── CustomerAccount.h
│   ├── EnterpriseAccount.h
│   ├── AccountService.h
│   ├── IAccountService.h
│   ├── IDBConnection.h
│   ├── ITransaction.h
│   ├── FakeDBConnection.h
│   ├── Server.h
│   └── ...
├── Source/                     # Implementation files
│   ├── main.cpp
│   ├── Account.cpp
│   ├── CustomerAccount.cpp
│   ├── EnterpriseAccount.cpp
│   ├── AccountService.cpp
│   ├── FakeDBConnection.cpp
│   ├── Server.cpp
│   └── ...
├── UI/                         # Simple browser-based UI
│   ├── index.html
│   └── README.md
├── UnitTests/                  # Unit tests
│   ├── AccountTests.cpp
│   ├── CustomerAccountTests.cpp
│   ├── EnterpriseAccountTests.cpp
│   ├── FakeDBConnectionTests.cpp
│   └── AccountServiceTests.cpp
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
└── docs/
    └── API.md                  # API documentation
```

## Building the Project

### Step 1: Clone and Navigate to Project

```bash
git clone <repository-url>
cd Bank_Exercise
```

### Step 2: Create Build Directory

```bash
mkdir -p build
cd build
```

### Step 3: Configure and Build

```bash
# Configure the project
cmake ..

# Build both the application and tests
cmake --build .
```

**Build output:**
- `build/Bank_Exercise` - Main application executable
- `build/Bank_Exercise_Tests` - Unit tests executable

### Alternative: One-liner Build

```bash
mkdir -p build && cd build && cmake .. && cmake --build .
```

### Clean Build

```bash
cd build
rm -rf *
cmake ..
cmake --build .
```

## Running the Application

### Start the Server

```bash
cd build
./Bank_Exercise
```

**Expected output:**
```
Server listening on 0.0.0.0:8080
```

The server will now listen on `http://localhost:8080`

## Running the Simple UI

The repository includes a simple browser UI in [UI/index.html](UI/index.html) that connects to the REST API server.

### Start the UI

```bash
cd /path/to/Bank_Exercise
python3 -m http.server 8000
```

Then open `http://localhost:8000` in your browser.

The root `index.html` forwards to the UI automatically, so the server root shows the app instead of a file listing.

### UI Workflow

1. Create a customer or enterprise account, or retrieve an existing account by ID.
2. Use the opened account to deposit, withdraw, or transfer money.
3. If you need an account number from customer or enterprise details, use the lookup forms in the UI.

### UI Requirements

- The Bank Exercise server must already be running on `http://localhost:8080`
- The UI is a static page and does not require a build step
- The UI uses the same REST API endpoints documented below

### Testing API Endpoints

#### 1. Health Check
```bash
curl http://localhost:8080/health
```

#### 2. Create a Customer Account
```bash
curl -X POST http://localhost:8080/accounts/customers \
  -H "Content-Type: application/json" \
  -d '{
    "firstName": "John",
    "lastName": "Doe"
  }'
```

#### 3. Create an Enterprise Account
```bash
curl -X POST http://localhost:8080/accounts/enterprises \
  -H "Content-Type: application/json" \
  -d '{
    "yTunnus": "1234567-8",
    "companyName": "TechCorp Ltd"
  }'
```

#### 4. Get Account Details
```bash
curl http://localhost:8080/accounts/1000
```

#### 5. Find a Customer Account by Name
```bash
curl -X POST http://localhost:8080/accounts/customers/lookup \
  -H "Content-Type: application/json" \
  -d '{
    "firstName": "John",
    "lastName": "Doe"
  }'
```

#### 6. Find an Enterprise Account by Details
```bash
curl -X POST http://localhost:8080/accounts/enterprises/lookup \
  -H "Content-Type: application/json" \
  -d '{
    "yTunnus": "1234567-8",
    "companyName": "TechCorp Ltd"
  }'
```

#### 7. Deposit Money
```bash
curl -X POST http://localhost:8080/accounts/1000/deposit \
  -H "Content-Type: application/json" \
  -d '{
    "amount": 500.0
  }'
```

#### 8. Withdraw Money
```bash
curl -X POST http://localhost:8080/accounts/1000/withdraw \
  -H "Content-Type: application/json" \
  -d '{
    "amount": 100.0
  }'
```

#### 9. Transfer Between Accounts
```bash
curl -X POST http://localhost:8080/accounts/1000/transfer \
  -H "Content-Type: application/json" \
  -d '{
    "toUniqueIdentifier": 1001,
    "amount": 250.0
  }'
```

See [API.md](docs/API.md) for complete API documentation.

## Running Unit Tests

### Run All Tests

```bash
cd build
./Bank_Exercise_Tests
```

**Expected output:**
```
[==========] Running 112 tests from 5 test suites.
[----------] Global test environment set-up.
[----------] 15 tests from AccountTests
[ OK ] AccountTests.DepositIncreasesBalance
[ OK ] AccountTests.WithdrawDecreasesBalance
...
[==========] 112 tests from 5 test suites ran. (XXX ms total)
[  PASSED  ] 112 tests.
```

### Run Specific Test Suite

```bash
# Run only Account tests
./Bank_Exercise_Tests --gtest_filter=AccountTests.*

# Run only AccountService tests
./Bank_Exercise_Tests --gtest_filter=AccountServiceTests.*

# Run only FakeDBConnection tests
./Bank_Exercise_Tests --gtest_filter=FakeDBConnectionTests.*
```

### Run Specific Test

```bash
# Run a single test
./Bank_Exercise_Tests --gtest_filter=AccountTests.DepositIncreasesBalance
```

### Generate Test Report

```bash
# XML output for CI/CD integration
./Bank_Exercise_Tests --gtest_output=xml:test_results.xml

# Display test names without running them
./Bank_Exercise_Tests --gtest_list_tests
```

### Run Tests with CTest

```bash
ctest --output-on-failure
```

## Test Coverage

The project includes **112 comprehensive unit tests** covering:

- **Account Operations** (15 tests)
  - Deposit operations
  - Withdrawal operations
  - Balance queries
  - Edge cases (zero amounts, insufficient funds)

- **Customer Account** (11 tests)
  - Account creation
  - Customer information
  - JSON serialization
  - Inherited operations

- **Enterprise Account** (12 tests)
  - Account creation
  - Enterprise information
  - JSON serialization
  - Large transactions

- **Database Operations** (32 tests)
  - Account CRUD operations
  - Transaction management
  - Rollback and commit operations
  - Account ID querying

- **Account Service** (42 tests)
  - Create accounts
  - Deposit operations
  - Withdrawal operations
  - Money transfers
  - Error handling
  - Complex scenarios

## Architecture

### Layered Design

```
┌─────────────────────┐
│   REST API Server   │
└──────────┬──────────┘
           │
┌──────────▼──────────┐
│ AccountService      │
│ (Business Logic)    │
└──────────┬──────────┘
           │
┌──────────▼──────────┐
│ IDBConnection       │
│ (Database Abstract) │
└──────────┬──────────┘
           │
┌──────────▼──────────┐
│FakeDBConnection     │
│(In-Memory Database) │
└─────────────────────┘
```

### Key Components

- **Account** - Base account class with balance operations
- **CustomerAccount** - Customer-specific account with personal information
- **EnterpriseAccount** - Enterprise-specific account with company information
- **AccountService** - Business logic layer for account operations
- **IDBConnection** - Abstract database interface
- **FakeDBConnection** - In-memory database implementation with transaction support
- **Server** - HTTP REST API server

## Dependencies

Dependencies are automatically downloaded via CMake FetchContent:

- **nlohmann/json** (v3.11.3) - JSON library for data serialization
- **cpp-httplib** (v0.18.3) - HTTP server library
- **Google Test** (v1.14.0) - Unit testing framework

## Build Options

### Enable/Disable Tests

Edit `CMakeLists.txt` to disable tests:

```cmake
# Comment out to disable tests
# add_executable(Bank_Exercise_Tests ...)
```

### Compiler Optimization

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## Troubleshooting

### Build Fails on CMake Configuration

```bash
# Clear CMake cache
rm -rf build/*
cmake ..
```

### Tests Won't Compile

Ensure C++17 support:
```bash
cmake -DCMAKE_CXX_STANDARD=17 ..
```

### Port 8080 Already in Use

Edit `Source/main.cpp` and change the port number:
```cpp
server.run("0.0.0.0", 8081);  // Change 8080 to 8081
```

### Permission Denied Running Executable

```bash
chmod +x ./Bank_Exercise
chmod +x ./Bank_Exercise_Tests
```

## Development Workflow

```bash
# 1. Build
mkdir -p build && cd build && cmake .. && cmake --build .

# 2. Run tests
./Bank_Exercise_Tests

# 3. Run application
./Bank_Exercise

# 4. Test API (in another terminal)
curl http://localhost:8080/health
```

## Adding New Tests

1. Create a new test file in `UnitTests/` directory
2. Include the necessary headers and gtest
3. Add test cases following the existing pattern
4. Update `CMakeLists.txt` to include the new test file

Example:
```cpp
#include <gtest/gtest.h>
#include <YourClass.h>

class YourClassTests : public ::testing::Test {
protected:
    YourClass instance;
};

TEST_F(YourClassTests, TestSomething) {
    EXPECT_EQ(expected, actual);
}
```

## Adding New Features

1. Define interfaces in `Include/`
2. Implement in `Source/`
3. Add unit tests in `UnitTests/`
4. Rebuild and run tests

## License

This is an educational project.

## Contact

For questions or issues, refer to the project documentation or API reference in `docs/API.md`.
