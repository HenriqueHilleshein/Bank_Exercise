#pragma once
#include <IDBConnection.h>
#include <ITransaction.h>
#include <map>
#include <memory>
#include <chrono>
#include <string>

class FakeDBConnection : public IDBConnection
{
public:
    FakeDBConnection();

    DBOperationResult getAccount(const AccountId::Identifier uniqueIdentifier, std::unique_ptr<IAccount>& outAccount) const override;
    DBOperationResult createEnterpriseAccount(const EnterpriseInformation& enterpriseInformation, AccountId& outAccountId) override;
    DBOperationResult createCustomerAccount(const CustomerInformation& customerInformation, AccountId& outAccountId) override;
    DBOperationResult getAccountIdWithEnterpriseInformation(const EnterpriseInformation& enterpriseInformation, AccountId& outAccountId) const override;
    DBOperationResult getAccountIdWithCustomerInformation(const CustomerInformation& customerInformation, AccountId& outAccountId) const override;
    DBOperationResult beginTransaction(std::unique_ptr<ITransaction>& outTransaction) override;

private:
    class FakeTransaction;
    friend class FakeTransaction;

    DBOperationResult updateAccountBalance(const IAccount& account);
    DBOperationResult commitTransaction();
    DBOperationResult rollbackTransaction();
    void takeTransactionSnapshot();
    void restoreTransactionSnapshot();

    enum class AccountType
    {
        Enterprise,
        Customer
    };

    struct AccountData
    {
        AccountId::Identifier accountId;
        double balance;
        AccountId accountIdStruct;
        AccountType type;
    };

    std::map<AccountId::Identifier, AccountData> _accounts;
    std::map<AccountId::Identifier, EnterpriseInformation> _enterpriseInfoByAccountId;
    std::map<AccountId::Identifier, CustomerInformation> _customerInfoByAccountId;
    std::map<std::string, AccountId::Identifier> _enterpriseYTunnusToId;
    std::map<std::string, AccountId::Identifier> _customerNameToId;
    AccountId::Identifier _nextAccountId;

    bool _transactionActive;
    std::map<AccountId::Identifier, AccountData> _transactionSnapshotAccounts;
    std::map<AccountId::Identifier, EnterpriseInformation> _transactionSnapshotEnterpriseInfo;
    std::map<AccountId::Identifier, CustomerInformation> _transactionSnapshotCustomerInfo;
    std::map<std::string, AccountId::Identifier> _transactionSnapshotEnterpriseYTunnusToId;
    std::map<std::string, AccountId::Identifier> _transactionSnapshotCustomerNameToId;
    AccountId::Identifier _transactionSnapshotNextAccountId;

    AccountId::Identifier _generateNextAccountId();
};
