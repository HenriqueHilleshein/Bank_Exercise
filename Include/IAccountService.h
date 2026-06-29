#pragma once
#include <IAccount.h>
#include <EnterpriseInformation.h>
#include <CustomerInformation.h>

class IAccountService
{
public:
    virtual ~IAccountService() = default;

    virtual std::unique_ptr<IAccount> getAccount(const AccountId::Identifier uniqueIdentifier) const = 0;
    virtual AccountId createEnterpriseAccount(const EnterpriseInformation& enterpriseInformation) const = 0;
    virtual AccountId createCustomerAccount(const CustomerInformation& customerInformation) const = 0;
    virtual AccountId getAccountIdWithEnterpriseInformation(const EnterpriseInformation& enterpriseInformation) const = 0;
    virtual AccountId getAccountIdWithCustomerInformation(const CustomerInformation& customerInformation) const = 0;
    virtual void depositToAccount(const AccountId::Identifier uniqueIdentifier, const double amount) const = 0;
    virtual bool withdrawFromAccount(const AccountId::Identifier uniqueIdentifier, const double amount) const = 0;
    virtual void transferBetweenAccounts(const AccountId::Identifier fromUniqueIdentifier, const AccountId::Identifier toUniqueIdentifier, const double amount) const = 0;
};