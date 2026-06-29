#pragma once
#include <IAccountService.h>
#include <IDBConnection.h>

class AccountService : public IAccountService
{
public:
    AccountService(IDBConnection* dbConnection);
    
    std::unique_ptr<IAccount> getAccount(const AccountId::Identifier uniqueIdentifier) const override;
    AccountId createEnterpriseAccount(const EnterpriseInformation& enterpriseInformation) const override;
    AccountId createCustomerAccount(const CustomerInformation& customerInformation) const override;
    AccountId getAccountIdWithEnterpriseInformation(const EnterpriseInformation& enterpriseInformation) const override;
    AccountId getAccountIdWithCustomerInformation(const CustomerInformation& customerInformation) const override;
    void depositToAccount(const AccountId::Identifier uniqueIdentifier, const double amount) const override;
    bool withdrawFromAccount(const AccountId::Identifier uniqueIdentifier, const double amount) const override;
    void transferBetweenAccounts(const AccountId::Identifier fromUniqueIdentifier, const AccountId::Identifier toUniqueIdentifier, const double amount) const override;

private:
    IDBConnection* _dbConnection;
};