# Bank Exercise — Software Architecture

## Purpose

This document describes the architecture of the bank account management software,
with emphasis on how interface-based design makes every major component replaceable —
especially the database backend.

The diagrams live alongside this file:

| File | Contents |
|---|---|
| `architecture-component.puml` | Layer and component overview |
| `architecture-class.puml`     | Interfaces, classes, and inheritance |
| `architecture-sequence.puml`  | Runtime flow for a deposit operation |

Render them with PlantUML: `plantuml docs/*.puml`

---

## Layering

```
┌──────────────────────────────────────────────┐
│  API Layer         Server                    │
│                    ApiRequestValidation       │
├──────────────────────────────────────────────┤
│  Service Layer     IAccountService  ◄── seam │
│                    AccountService             │
├──────────────────────────────────────────────┤
│  Domain Layer      IAccount  ◄──────── seam  │
│                    Account                    │
│                    CustomerAccount            │
│                    EnterpriseAccount          │
├──────────────────────────────────────────────┤
│  Persistence       IDBConnection  ◄── seam   │
│  Layer             ITransaction   ◄── seam   │
│                    FakeDBConnection  (today)  │
│                    SqlDBConnection   (future) │
└──────────────────────────────────────────────┘
```

Each `◄── seam` marks an interface boundary that a consuming layer never crosses.
The concrete class behind the seam can be replaced without touching any of the
layers above it.

---

## Interface Descriptions

### `IAccount` — polymorphic account

Defines what every account type must support: balance operations (`deposit`,
`withdraw`), identity (`getAccountId`), and details (`getAccountDetails`).

`Server` and `AccountService` only ever hold `std::unique_ptr<IAccount>` and
call through this interface. They have no knowledge of whether the account is a
`CustomerAccount` or `EnterpriseAccount`.

```cpp
// Server route — type of account is irrelevant here
std::unique_ptr<IAccount> account = _accountService->getAccount(id);
res.set_content(account->getAccountDetails().dump(), "application/json");
```

Because `getAccountDetails()` is a virtual method, each concrete type controls
what it returns. This means you can freely change or extend the details of any
account type without touching `Server`:

- Adding `email` to `CustomerInformation` and including it in
  `CustomerAccount::getAccountDetails()` requires changes only in
  `CustomerInformation.h`, `CustomerAccount.cpp`, and the DB creation logic.
  `Server` calls `getAccountDetails()` the same way and the new field appears
  automatically in the JSON response.

- Changing `EnterpriseAccount` to return a formatted company registration string
  instead of the raw `yTunnus` only requires editing
  `EnterpriseAccount::getAccountDetails()`. Nothing outside the enterprise
  account module is aware of the change.

In both cases `Server`, `AccountService`, and all unrelated tests remain
**completely unchanged**.

---

### `IAccountService` — business operations seam

`Server` holds a pointer to `IAccountService`, not to `AccountService`. This
means the entire service layer can be swapped or mocked without touching the HTTP
routing code.

```cpp
// Server.h  — depends on the interface, not the concrete class
class Server {
    IAccountService* _accountService;
};
```

This makes it straightforward to:
- Unit-test `Server` with a mock service
- Replace `AccountService` with a caching, auditing, or remote-call variant

---

### `IDBConnection` — storage seam

`AccountService` holds a pointer to `IDBConnection`. It has no knowledge of
in-memory maps, SQL, or any other storage mechanism.

To replace `FakeDBConnection` with a real SQL backend:

1. Create `SqlDBConnection : public IDBConnection`
2. Implement all pure-virtual methods using your SQL driver
3. Change one line in `main.cpp`:

```cpp
// Before
FakeDBConnection db;

// After
SqlDBConnection db("host=localhost dbname=bank user=app password=...");
```

`AccountService`, `Server`, and every test that uses `IAccountService` remain
**completely unchanged**.

---

### `ITransaction` — unit-of-work seam

`AccountService` acquires a transaction via `IDBConnection::beginTransaction` and
routes all reads and writes through the returned `ITransaction`. It never calls
begin/commit/rollback logic directly against the concrete connection object.

This means:
- A SQL backend maps `getForUpdate` to `SELECT ... FOR UPDATE`
- `commit` and `rollback` map to `COMMIT` and `ROLLBACK`
- `FakeDBConnection` implements them with an in-memory snapshot
- `AccountService` code is identical regardless of which backend is active

```
ITransaction
  getForUpdate(id)      → SQL: SELECT ... FOR UPDATE  |  Fake: map lookup + snapshot
  updateAccountBalance  → SQL: UPDATE accounts SET …  |  Fake: map update
  commit                → SQL: COMMIT                 |  Fake: discard snapshot
  rollback              → SQL: ROLLBACK               |  Fake: restore snapshot
```

---

### `AccountId::Identifier` — type alias for future identifier evolution

The account identifier is currently `int`. An alias is used everywhere instead
of the raw type, so switching to IBAN strings requires changing one line:

```cpp
// AccountId.h — one change propagates to all interfaces and implementations
struct AccountId {
    using Identifier = int;   // change to std::string for IBAN
    ...
};
```

Route parsing is centralised in one helper function in `Server.cpp` so the
string-to-identifier conversion is also a single change point.

---

## Replacing FakeDBConnection with a SQL Backend — Step by Step

1. **Create** `Include/SqlDBConnection.h` and `Source/SqlDBConnection.cpp`

```cpp
class SqlDBConnection : public IDBConnection {
public:
    explicit SqlDBConnection(const std::string& connectionString);

    DBOperationResult getAccount(AccountId::Identifier id,
                                  std::unique_ptr<IAccount>& out) const override;
    DBOperationResult createCustomerAccount(const CustomerInformation& info,
                                             AccountId& outId) override;
    // ... all other IDBConnection methods ...
    DBOperationResult beginTransaction(std::unique_ptr<ITransaction>& out) override;

private:
    // SQL connection handle (libpq, ODBC, etc.)
};
```

2. **Create** an inner `SqlTransaction : public ITransaction` that wraps the
   connection-level `BEGIN`/`COMMIT`/`ROLLBACK` and uses `SELECT FOR UPDATE`
   inside `getForUpdate`.

3. **Wire up** in `main.cpp`:

```cpp
// main.cpp — the only file that mentions a concrete DB class
SqlDBConnection db("host=localhost dbname=bank user=app");
AccountService service(&db);
Server server(&service);
server.run("0.0.0.0", 8080);
```

Everything else stays the same. The unit tests written against `IAccountService`
continue to use `FakeDBConnection` and need no changes.
