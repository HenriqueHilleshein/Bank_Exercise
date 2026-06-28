#pragma once
#include <IDBConnection.h>
#include <map>
#include <memory>
#include <chrono>

class FakeDBConnection : public IDBConnection
{
public:
    FakeDBConnection();
    
    DBOperationResult getAccount(const int uniqueIdentifier, std::unique_ptr<IAccount>& outAccount) const override;
    DBOperationResult updateAccountBalance(const IAccount& account) const override;
    DBOperationResult updateAccountsBalance(const IAccount& accountA, const IAccount& accountB) const override;
    DBOperationResult createEnterpriseAccount(const EnterpriseInformation enterpriseInformation, AccountId& outAccountId) const override;
    DBOperationResult createCustomerAccount(const CustomerInformation customerInformation, AccountId& outAccountId) const override;
    DBOperationResult getAccountIdWithEnterpriseInformation(const EnterpriseInformation enterpriseInformation, AccountId& outAccountId) const override;
    DBOperationResult getAccountIdWithCustomerInformation(const CustomerInformation customerInformation, AccountId& outAccountId) const override;
    DBOperationResult beginTransaction() const override;
    DBOperationResult commit() const override;
    DBOperationResult rollback() const override;

private:
    enum class AccountType
    {
        Enterprise,
        Customer
    };

    struct AccountData
    {
        int accountId;
        double balance;
        AccountId accountIdStruct;
        AccountType type;
    };

    mutable std::map<int, AccountData> _accounts;
    mutable std::map<int, EnterpriseInformation> _enterpriseInfoByAccountId;
    mutable std::map<int, CustomerInformation> _customerInfoByAccountId;
    mutable std::map<std::string, int> _enterpriseYTunnusToId;
    mutable std::map<std::string, int> _customerNameToId;
    mutable int _nextAccountId;
    
    mutable bool _transactionActive;
    mutable std::map<int, AccountData> _transactionSnapshot;
    
    int _generateNextAccountId() const;
};
