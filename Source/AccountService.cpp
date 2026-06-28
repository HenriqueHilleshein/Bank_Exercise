#include <AccountService.h>
#include <stdexcept>

AccountService::AccountService(IDBConnection* dbConnection)
    : _dbConnection(dbConnection)
{
}

std::unique_ptr<IAccount> AccountService::getAccount(const int uniqueIdentifier) const
{
    std::unique_ptr<IAccount> account;
    DBOperationResult result = _dbConnection->getAccount(uniqueIdentifier, account);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to get account");
    }
    return account;
}

AccountId AccountService::createEnterpriseAccount(const EnterpriseInformation enterpriseInformation) const
{
    AccountId accountId{};
    DBOperationResult result = _dbConnection->createEnterpriseAccount(enterpriseInformation, accountId);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to create enterprise account");
    }
    return accountId;
}

AccountId AccountService::createCustomerAccount(const CustomerInformation customerInformation) const
{
    AccountId accountId{};
    DBOperationResult result = _dbConnection->createCustomerAccount(customerInformation, accountId);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to create customer account");
    }
    return accountId;
}

AccountId AccountService::getAccountIdWithEnterpriseInformation(const EnterpriseInformation enterpriseInformation) const
{
    AccountId accountId{};
    DBOperationResult result = _dbConnection->getAccountIdWithEnterpriseInformation(enterpriseInformation, accountId);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to get enterprise account ID");
    }
    return accountId;
}

AccountId AccountService::getAccountIdWithCustomerInformation(const CustomerInformation customerInformation) const
{
    AccountId accountId{};
    DBOperationResult result = _dbConnection->getAccountIdWithCustomerInformation(customerInformation, accountId);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to get customer account ID");
    }
    return accountId;
}

void AccountService::depositToAccount(const int uniqueIdentifier, const double amount) const
{
    std::unique_ptr<IAccount> account;
    DBOperationResult result = _dbConnection->getAccount(uniqueIdentifier, account);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to get account for deposit");
    }
    
    account->deposit(amount);
    result = _dbConnection->updateAccountBalance(*account);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to update account balance after deposit");
    }
}

bool AccountService::withdrawFromAccount(const int uniqueIdentifier, const double amount) const
{
    std::unique_ptr<IAccount> account;
    DBOperationResult result = _dbConnection->getAccount(uniqueIdentifier, account);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to get account for withdrawal");
    }
    
    bool success = account->withdraw(amount);
    if (success) {
        result = _dbConnection->updateAccountBalance(*account);
        if (result != DBOperationResult::Success) {
            throw std::runtime_error("Failed to update account balance after withdrawal");
        }
    }
    return success;
}

void AccountService::transferBetweenAccounts(const int fromUniqueIdentifier, const int toUniqueIdentifier, const double amount) const
{
    std::unique_ptr<IAccount> fromAccount;
    std::unique_ptr<IAccount> toAccount;
    
    DBOperationResult result = _dbConnection->getAccount(fromUniqueIdentifier, fromAccount);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to get source account for transfer");
    }
    
    result = _dbConnection->getAccount(toUniqueIdentifier, toAccount);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to get destination account for transfer");
    }

    if (fromAccount->withdraw(amount)) {
        toAccount->deposit(amount);
        result = _dbConnection->updateAccountsBalance(*fromAccount, *toAccount);
        if (result != DBOperationResult::Success) {
            throw std::runtime_error("Failed to update account balances after transfer");
        }
    }
}

