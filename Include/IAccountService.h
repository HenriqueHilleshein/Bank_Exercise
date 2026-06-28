#pragma once
#include <IAccount.h>
#include <EnterpriseInformation.h>
#include <CustomerInformation.h>

class IAccountService
{
public:
    virtual std::unique_ptr<IAccount> getAccount(const int uniqueIdentifier) const = 0;
    virtual AccountId createEnterpriseAccount(const EnterpriseInformation enterpriseInformation) const = 0;
    virtual AccountId createCustomerAccount(const CustomerInformation customerInformation) const = 0;
    virtual AccountId getAccountIdWithEnterpriseInformation(const EnterpriseInformation enterpriseInformation) const = 0;
    virtual AccountId getAccountIdWithCustomerInformation(const CustomerInformation customerInformation) const = 0;
    virtual void depositToAccount(const int uniqueIdentifier, const double amount) const = 0;
    virtual bool withdrawFromAccount(const int uniqueIdentifier, const double amount) const = 0;
    virtual void transferBetweenAccounts(const int fromUniqueIdentifier, const int toUniqueIdentifier, const double amount) const = 0;
};