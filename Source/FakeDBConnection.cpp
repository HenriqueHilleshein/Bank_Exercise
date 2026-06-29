#include <FakeDBConnection.h>
#include <EnterpriseAccount.h>
#include <CustomerAccount.h>
#include <unordered_set>

class FakeDBConnection::FakeTransaction : public ITransaction
{
public:
    FakeTransaction(FakeDBConnection& connection)
        : _connection(connection), _active(true)
    {
    }

    ~FakeTransaction() override
    {
        if (_active) {
            rollback();
        }
    }

    DBOperationResult getForUpdate(const AccountId::Identifier uniqueIdentifier, std::unique_ptr<IAccount>& outAccount) override
    {
        if (!_active) {
            return DBOperationResult::OperationFailed;
        }

        if (_lockedAccounts.find(uniqueIdentifier) == _lockedAccounts.end()) {
            _lockedAccounts.insert(uniqueIdentifier);
        }

        return _connection.getAccount(uniqueIdentifier, outAccount);
    }

    DBOperationResult updateAccountBalance(const IAccount& account) override
    {
        if (!_active) {
            return DBOperationResult::OperationFailed;
        }

        if (_lockedAccounts.find(account.getAccountId().UniqueIdentifier) == _lockedAccounts.end()) {
            return DBOperationResult::OperationFailed;
        }

        return _connection.updateAccountBalance(account);
    }

    DBOperationResult commit() override
    {
        if (!_active) {
            return DBOperationResult::NoActiveTransaction;
        }

        DBOperationResult result = _connection.commitTransaction();
        if (result == DBOperationResult::Success) {
            _active = false;
        }
        return result;
    }

    DBOperationResult rollback() override
    {
        if (!_active) {
            return DBOperationResult::NoActiveTransaction;
        }

        DBOperationResult result = _connection.rollbackTransaction();
        if (result == DBOperationResult::Success) {
            _active = false;
        }
        return result;
    }

private:
    FakeDBConnection& _connection;
    bool _active;
    std::unordered_set<AccountId::Identifier> _lockedAccounts;
};

FakeDBConnection::FakeDBConnection()
    : _nextAccountId(1000), _transactionActive(false), _transactionSnapshotNextAccountId(0)
{
}

AccountId::Identifier FakeDBConnection::_generateNextAccountId()
{
    return _nextAccountId++;
}

DBOperationResult FakeDBConnection::getAccount(const AccountId::Identifier uniqueIdentifier, std::unique_ptr<IAccount>& outAccount) const
{
    auto it = _accounts.find(uniqueIdentifier);
    if (it == _accounts.end()) {
        return DBOperationResult::AccountNotFound;
    }

    const AccountData& data = it->second;
    if (data.type == AccountType::Enterprise) {
        auto infoIt = _enterpriseInfoByAccountId.find(uniqueIdentifier);
        if (infoIt == _enterpriseInfoByAccountId.end()) {
            return DBOperationResult::OperationFailed;
        }
        outAccount = std::make_unique<EnterpriseAccount>(data.balance, data.accountIdStruct, infoIt->second);
    } else {
        auto infoIt = _customerInfoByAccountId.find(uniqueIdentifier);
        if (infoIt == _customerInfoByAccountId.end()) {
            return DBOperationResult::OperationFailed;
        }
        outAccount = std::make_unique<CustomerAccount>(data.balance, data.accountIdStruct, infoIt->second);
    }
    return DBOperationResult::Success;
}

DBOperationResult FakeDBConnection::updateAccountBalance(const IAccount& account)
{
    AccountId::Identifier uniqueIdentifier = account.getAccountId().UniqueIdentifier;
    auto it = _accounts.find(uniqueIdentifier);
    if (it == _accounts.end()) {
        return DBOperationResult::AccountNotFound;
    }

    it->second.balance = account.getBalance();
    return DBOperationResult::Success;
}

DBOperationResult FakeDBConnection::createEnterpriseAccount(const EnterpriseInformation& enterpriseInformation, AccountId& outAccountId)
{
    if (enterpriseInformation.YTunnus.empty() || enterpriseInformation.CompanyName.empty()) {
        return DBOperationResult::InvalidInput;
    }

    AccountId::Identifier accountId = _generateNextAccountId();
    AccountId::CreationTimestamp creationTime = std::chrono::system_clock::now().time_since_epoch().count();
    AccountId newAccountId{accountId, creationTime};

    AccountData data;
    data.accountId = accountId;
    data.balance = 0.0;
    data.accountIdStruct = newAccountId;
    data.type = AccountType::Enterprise;

    _accounts[accountId] = data;
    _enterpriseInfoByAccountId[accountId] = enterpriseInformation;
    _enterpriseYTunnusToId[enterpriseInformation.YTunnus] = accountId;

    outAccountId = newAccountId;
    return DBOperationResult::Success;
}

DBOperationResult FakeDBConnection::createCustomerAccount(const CustomerInformation& customerInformation, AccountId& outAccountId)
{
    if (customerInformation.FirstName.empty() || customerInformation.LastName.empty()) {
        return DBOperationResult::InvalidInput;
    }

    AccountId::Identifier accountId = _generateNextAccountId();
    AccountId::CreationTimestamp creationTime = std::chrono::system_clock::now().time_since_epoch().count();
    AccountId newAccountId{accountId, creationTime};

    AccountData data;
    data.accountId = accountId;
    data.balance = 0.0;
    data.accountIdStruct = newAccountId;
    data.type = AccountType::Customer;

    _accounts[accountId] = data;
    _customerInfoByAccountId[accountId] = customerInformation;

    std::string nameKey = customerInformation.FirstName + " " + customerInformation.LastName;
    _customerNameToId[nameKey] = accountId;

    outAccountId = newAccountId;
    return DBOperationResult::Success;
}

DBOperationResult FakeDBConnection::getAccountIdWithEnterpriseInformation(const EnterpriseInformation& enterpriseInformation, AccountId& outAccountId) const
{
    auto it = _enterpriseYTunnusToId.find(enterpriseInformation.YTunnus);
    if (it == _enterpriseYTunnusToId.end()) {
        return DBOperationResult::AccountNotFound;
    }

    AccountId::Identifier accountId = it->second;
    auto accountIt = _accounts.find(accountId);
    if (accountIt != _accounts.end()) {
        outAccountId = accountIt->second.accountIdStruct;
        return DBOperationResult::Success;
    }

    return DBOperationResult::OperationFailed;
}

DBOperationResult FakeDBConnection::getAccountIdWithCustomerInformation(const CustomerInformation& customerInformation, AccountId& outAccountId) const
{
    std::string nameKey = customerInformation.FirstName + " " + customerInformation.LastName;
    auto it = _customerNameToId.find(nameKey);
    if (it == _customerNameToId.end()) {
        return DBOperationResult::AccountNotFound;
    }

    AccountId::Identifier accountId = it->second;
    auto accountIt = _accounts.find(accountId);
    if (accountIt != _accounts.end()) {
        outAccountId = accountIt->second.accountIdStruct;
        return DBOperationResult::Success;
    }

    return DBOperationResult::OperationFailed;
}

void FakeDBConnection::takeTransactionSnapshot()
{
    _transactionSnapshotAccounts = _accounts;
    _transactionSnapshotEnterpriseInfo = _enterpriseInfoByAccountId;
    _transactionSnapshotCustomerInfo = _customerInfoByAccountId;
    _transactionSnapshotEnterpriseYTunnusToId = _enterpriseYTunnusToId;
    _transactionSnapshotCustomerNameToId = _customerNameToId;
    _transactionSnapshotNextAccountId = _nextAccountId;
}

void FakeDBConnection::restoreTransactionSnapshot()
{
    _accounts = _transactionSnapshotAccounts;
    _enterpriseInfoByAccountId = _transactionSnapshotEnterpriseInfo;
    _customerInfoByAccountId = _transactionSnapshotCustomerInfo;
    _enterpriseYTunnusToId = _transactionSnapshotEnterpriseYTunnusToId;
    _customerNameToId = _transactionSnapshotCustomerNameToId;
    _nextAccountId = _transactionSnapshotNextAccountId;
}

DBOperationResult FakeDBConnection::commitTransaction()
{
    if (!_transactionActive) {
        return DBOperationResult::NoActiveTransaction;
    }

    _transactionActive = false;
    return DBOperationResult::Success;
}

DBOperationResult FakeDBConnection::rollbackTransaction()
{
    if (!_transactionActive) {
        return DBOperationResult::NoActiveTransaction;
    }

    restoreTransactionSnapshot();
    _transactionActive = false;
    return DBOperationResult::Success;
}

DBOperationResult FakeDBConnection::beginTransaction(std::unique_ptr<ITransaction>& outTransaction)
{
    if (_transactionActive) {
        return DBOperationResult::TransactionAlreadyActive;
    }

    takeTransactionSnapshot();
    _transactionActive = true;
    outTransaction = std::make_unique<FakeTransaction>(*this);
    return DBOperationResult::Success;
}
