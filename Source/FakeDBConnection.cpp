#include <FakeDBConnection.h>
#include <EnterpriseAccount.h>
#include <CustomerAccount.h>

FakeDBConnection::FakeDBConnection() : _nextAccountId(1000), _transactionActive(false)
{
}

int FakeDBConnection::_generateNextAccountId() const
{
    return _nextAccountId++;
}

DBOperationResult FakeDBConnection::getAccount(const int uniqueIdentifier, std::unique_ptr<IAccount>& outAccount) const
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

DBOperationResult FakeDBConnection::updateAccountBalance(const IAccount& account) const
{
    // Find the account by balance and update it
    double targetBalance = account.getBalance();
    for (auto& pair : _accounts) {
        if (pair.second.balance == targetBalance) {
            pair.second.balance = account.getBalance();
            return DBOperationResult::Success;
        }
    }
    return DBOperationResult::AccountNotFound;
}

DBOperationResult FakeDBConnection::updateAccountsBalance(const IAccount& accountA, const IAccount& accountB) const
{
    // Update both accounts in the database
    DBOperationResult resultA = updateAccountBalance(accountA);
    if (resultA != DBOperationResult::Success) {
        return resultA;
    }
    
    DBOperationResult resultB = updateAccountBalance(accountB);
    return resultB;
}

DBOperationResult FakeDBConnection::createEnterpriseAccount(const EnterpriseInformation enterpriseInformation, AccountId& outAccountId) const
{
    if (enterpriseInformation.YTunnus.empty() || enterpriseInformation.CompanyName.empty()) {
        return DBOperationResult::InvalidInput;
    }
    
    int accountId = _generateNextAccountId();
    long creationTime = std::chrono::system_clock::now().time_since_epoch().count();
    
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

DBOperationResult FakeDBConnection::createCustomerAccount(const CustomerInformation customerInformation, AccountId& outAccountId) const
{
    if (customerInformation.FirstName.empty() || customerInformation.LastName.empty()) {
        return DBOperationResult::InvalidInput;
    }
    
    int accountId = _generateNextAccountId();
    long creationTime = std::chrono::system_clock::now().time_since_epoch().count();
    
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

DBOperationResult FakeDBConnection::getAccountIdWithEnterpriseInformation(const EnterpriseInformation enterpriseInformation, AccountId& outAccountId) const
{
    auto it = _enterpriseYTunnusToId.find(enterpriseInformation.YTunnus);
    if (it == _enterpriseYTunnusToId.end()) {
        return DBOperationResult::AccountNotFound;
    }
    
    int accountId = it->second;
    auto accountIt = _accounts.find(accountId);
    if (accountIt != _accounts.end()) {
        outAccountId = accountIt->second.accountIdStruct;
        return DBOperationResult::Success;
    }
    
    return DBOperationResult::OperationFailed;
}

DBOperationResult FakeDBConnection::getAccountIdWithCustomerInformation(const CustomerInformation customerInformation, AccountId& outAccountId) const
{
    std::string nameKey = customerInformation.FirstName + " " + customerInformation.LastName;
    auto it = _customerNameToId.find(nameKey);
    if (it == _customerNameToId.end()) {
        return DBOperationResult::AccountNotFound;
    }
    
    int accountId = it->second;
    auto accountIt = _accounts.find(accountId);
    if (accountIt != _accounts.end()) {
        outAccountId = accountIt->second.accountIdStruct;
        return DBOperationResult::Success;
    }
    
    return DBOperationResult::OperationFailed;
}

DBOperationResult FakeDBConnection::beginTransaction() const
{
    if (_transactionActive) {
        return DBOperationResult::TransactionAlreadyActive;
    }
    
    // Take a snapshot of the current state for potential rollback
    _transactionSnapshot = _accounts;
    _transactionActive = true;
    
    return DBOperationResult::Success;
}

DBOperationResult FakeDBConnection::commit() const
{
    if (!_transactionActive) {
        return DBOperationResult::NoActiveTransaction;
    }
    
    // Transaction is committed - clear the snapshot
    _transactionSnapshot.clear();
    _transactionActive = false;
    
    return DBOperationResult::Success;
}
