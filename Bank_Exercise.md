# C++ Exercise: Inheritance, Aggregation, and Implementation Hiding

## Objective
Design a bank/account management software in C++ that demonstrates:
- Inheritance
- Aggregation
- Implementation hiding (abstraction and minimal coupling)

---

## System Overview

The system should include an **Account module** capable of handling two different account types:

- Customer account
- Enterprise account

The module must expose a **common interface** to the main program.

---

## Core Requirements

### 1. Account Module (Base Design)

The account system must support:

#### Balance Management
- Store account balance
- Support:
  - Adding funds
  - Decreasing funds
- Must include **sanity checks**, such as:
  - Prevent negative balance (unless explicitly allowed)
  - Validate operations before applying changes

---

### 2. AccountId Object (Mandatory Aggregation)

Each account must use a **custom AccountId object**.

#### AccountId must include:
- A unique integer ID
- A date representing when the ID was created

#### Design Requirement (Important)
- The main program must be minimally affected if the internal representation changes  
  - Example: switching from `int` ID to IBAN-style string
- This implies:
  - Strong encapsulation
  - Use of abstraction / interfaces where appropriate

---

## Account Types

### 3. Customer Account

Additional attributes:
- Customer first name
- Customer last name

---

### 4. Enterprise Account

Additional attributes:
- Business ID (e.g., “Y-tunnus”)
- Company name

---

## Design Constraints

- You may represent account-specific data using:
  - Two `string` fields for simplicity, OR
  - A more flexible schema-based design (preferred for extensibility)
- The design should allow future account types with minimal changes

---

## Main Program Requirements

Create a simple UI for testing it.

The user should be able to:
- Create an account (choose type: Customer or Enterprise)
- Add balance to an account
- Decrease balance from an account
- Query account details


