#pragma once
#include <memory>
#include <IAccount.h>
#include <EnterpriseInformation.h>
#include <CustomerInformation.h>
#include <AccountId.h>
#include <DBOperationResult.h>

class ITransaction;

class IDBConnection
{
public:
    virtual ~IDBConnection() = default;

    virtual DBOperationResult getAccount(const AccountId::Identifier uniqueIdentifier, std::unique_ptr<IAccount>& outAccount) const = 0;
    virtual DBOperationResult createEnterpriseAccount(const EnterpriseInformation& enterpriseInformation, AccountId& outAccountId) = 0;
    virtual DBOperationResult createCustomerAccount(const CustomerInformation& customerInformation, AccountId& outAccountId) = 0;
    virtual DBOperationResult getAccountIdWithEnterpriseInformation(const EnterpriseInformation& enterpriseInformation, AccountId& outAccountId) const = 0;
    virtual DBOperationResult getAccountIdWithCustomerInformation(const CustomerInformation& customerInformation, AccountId& outAccountId) const = 0;
    virtual DBOperationResult beginTransaction(std::unique_ptr<ITransaction>& outTransaction) = 0;
};
