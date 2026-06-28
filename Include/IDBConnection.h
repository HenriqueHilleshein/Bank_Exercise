#pragma once
#include <IAccount.h>
#include <EnterpriseAccount.h>
#include <CustomerAccount.h>

enum class DBOperationResult
{
    Success,
    AccountNotFound,
    InvalidInput,
    OperationFailed,
    TransactionAlreadyActive,
    NoActiveTransaction
};

class IDBConnection
{
public:
    virtual ~IDBConnection() = default;
    
    virtual DBOperationResult getAccount(const int uniqueIdentifier, std::unique_ptr<IAccount>& outAccount) const = 0;
    virtual DBOperationResult updateAccountBalance(const IAccount& account) const = 0;
    virtual DBOperationResult updateAccountsBalance(const IAccount& accountA, const IAccount& accountB) const = 0;
    virtual DBOperationResult createEnterpriseAccount(const EnterpriseInformation enterpriseInformation, AccountId& outAccountId) const = 0;
    virtual DBOperationResult createCustomerAccount(const CustomerInformation customerInformation, AccountId& outAccountId) const = 0;
    virtual DBOperationResult getAccountIdWithEnterpriseInformation(const EnterpriseInformation enterpriseInformation, AccountId& outAccountId) const = 0;
    virtual DBOperationResult getAccountIdWithCustomerInformation(const CustomerInformation customerInformation, AccountId& outAccountId) const = 0;
    virtual DBOperationResult beginTransaction() const = 0;
    virtual DBOperationResult commit() const = 0;
    virtual DBOperationResult rollback() const = 0;
};