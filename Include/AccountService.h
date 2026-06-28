#pragma once
#include <IAccountService.h>
#include <IDBConnection.h>

class AccountService : public IAccountService
{
public:
    AccountService(IDBConnection* dbConnection);
    
    std::unique_ptr<IAccount> getAccount(const int uniqueIdentifier) const override;
    AccountId createEnterpriseAccount(const EnterpriseInformation enterpriseInformation) const override;
    AccountId createCustomerAccount(const CustomerInformation customerInformation) const override;
    AccountId getAccountIdWithEnterpriseInformation(const EnterpriseInformation enterpriseInformation) const override;
    AccountId getAccountIdWithCustomerInformation(const CustomerInformation customerInformation) const override;
    void depositToAccount(const int uniqueIdentifier, const double amount) const override;
    bool withdrawFromAccount(const int uniqueIdentifier, const double amount) const override;
    void transferBetweenAccounts(const int fromUniqueIdentifier, const int toUniqueIdentifier, const double amount) const override;

private:
    IDBConnection* _dbConnection;
};