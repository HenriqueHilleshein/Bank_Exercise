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

    DBOperationResult getAccount(const int uniqueIdentifier, std::unique_ptr<IAccount>& outAccount) const override;
    DBOperationResult createEnterpriseAccount(const EnterpriseInformation enterpriseInformation, AccountId& outAccountId) override;
    DBOperationResult createCustomerAccount(const CustomerInformation customerInformation, AccountId& outAccountId) override;
    DBOperationResult getAccountIdWithEnterpriseInformation(const EnterpriseInformation enterpriseInformation, AccountId& outAccountId) const override;
    DBOperationResult getAccountIdWithCustomerInformation(const CustomerInformation customerInformation, AccountId& outAccountId) const override;
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
        int accountId;
        double balance;
        AccountId accountIdStruct;
        AccountType type;
    };

    std::map<int, AccountData> _accounts;
    std::map<int, EnterpriseInformation> _enterpriseInfoByAccountId;
    std::map<int, CustomerInformation> _customerInfoByAccountId;
    std::map<std::string, int> _enterpriseYTunnusToId;
    std::map<std::string, int> _customerNameToId;
    int _nextAccountId;

    bool _transactionActive;
    std::map<int, AccountData> _transactionSnapshotAccounts;
    std::map<int, EnterpriseInformation> _transactionSnapshotEnterpriseInfo;
    std::map<int, CustomerInformation> _transactionSnapshotCustomerInfo;
    std::map<std::string, int> _transactionSnapshotEnterpriseYTunnusToId;
    std::map<std::string, int> _transactionSnapshotCustomerNameToId;
    int _transactionSnapshotNextAccountId;

    int _generateNextAccountId();
};
