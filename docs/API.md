# Bank Exercise API Documentation

## Overview
This server exposes a simple bank account API backed by a fake in-memory database and transaction support.

- Base URL: `http://localhost:8080`
- Content type: `application/json`
- No authentication is implemented

## Endpoints

### GET /health
Check whether the server is running.

Response:
- `200 OK`
- body: `OK`

### GET /accounts/{id}
Retrieve account details by unique account identifier.

Request:
- Path parameter: `id` (integer)

Response:
- `200 OK` with account JSON
- `404` if account is not found

Example response:
```json
{
  "balance": 100.0,
  "accountId": {
    "UniqueIdentifier": 1000,
    "CreationTime": 1710000000000000000
  },
  "customerInformation": {
    "firstName": "John",
    "lastName": "Doe"
  }
}
```

For enterprise accounts, the response includes `enterpriseInformation` instead of `customerInformation`.

### POST /accounts/enterprises
Create a new enterprise account.

Request body:
```json
{
  "yTunnus": "1234567-8",
  "companyName": "Acme Corp"
}
```

Response:
- `200 OK`
- body:
```json
{
  "accountId": {
    "UniqueIdentifier": 1001,
    "CreationTime": 1710000000000000000
  }
}
```

### POST /accounts/customers
Create a new customer account.

Request body:
```json
{
  "firstName": "Jane",
  "lastName": "Smith"
}
```

Response:
- `200 OK`
- body:
```json
{
  "accountId": {
    "UniqueIdentifier": 1002,
    "CreationTime": 1710000000000000000
  }
}
```

### POST /accounts/enterprises/lookup
Find an enterprise account ID by enterprise information.

Request body:
```json
{
  "yTunnus": "1234567-8"
}
```

Response:
- `200 OK`
- body:
```json
{
  "accountId": {
    "UniqueIdentifier": 1001,
    "CreationTime": 1710000000000000000
  }
}
```

### POST /accounts/customers/lookup
Find a customer account ID by customer information.

Request body:
```json
{
  "firstName": "Jane",
  "lastName": "Smith"
}
```

Response:
- `200 OK`
- body:
```json
{
  "accountId": {
    "UniqueIdentifier": 1002,
    "CreationTime": 1710000000000000000
  }
}
```

### POST /accounts/{id}/deposit
Deposit money into an account.

Request body:
```json
{
  "amount": 50.0
}
```

Response:
- `200 OK`
- body:
```json
{
  "success": true
}
```

### POST /accounts/{id}/withdraw
Withdraw money from an account.

Request body:
```json
{
  "amount": 25.0
}
```

Response:
- `200 OK` if withdrawal succeeds
- body:
```json
{
  "success": true
}
```
- `400` if withdrawal fails due to insufficient funds
- body:
```json
{
  "success": false,
  "error": "Insufficient funds"
}
```

### POST /accounts/{id}/transfer
Transfer money between two accounts.

Request body:
```json
{
  "toUniqueIdentifier": 1001,
  "amount": 20.0
}
```

Response:
- `200 OK`
- body:
```json
{
  "success": true
}
```

## Error handling
If a request is invalid or the requested operation fails, the API returns a `400` or `404` status and a JSON error body:

```json
{
  "error": "Description of the failure"
}
```

## Notes
- Account creation uses a fake in-memory database.
- Deposit, withdraw, and transfer operations are executed using transaction semantics.
- `getAccount` returns full account details for either customer or enterprise accounts.
