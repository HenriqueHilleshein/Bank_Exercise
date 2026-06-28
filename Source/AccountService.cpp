#include <AccountService.h>
#include <ITransaction.h>
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
    std::unique_ptr<ITransaction> transaction;
    DBOperationResult result = _dbConnection->beginTransaction(transaction);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to begin transaction for deposit");
    }

    std::unique_ptr<IAccount> account;
    result = transaction->getForUpdate(uniqueIdentifier, account);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to lock account for deposit");
    }

    account->deposit(amount);
    result = transaction->updateAccountBalance(*account);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to update account balance after deposit");
    }

    result = transaction->commit();
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to commit deposit transaction");
    }
}

bool AccountService::withdrawFromAccount(const int uniqueIdentifier, const double amount) const
{
    std::unique_ptr<ITransaction> transaction;
    DBOperationResult result = _dbConnection->beginTransaction(transaction);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to begin transaction for withdrawal");
    }

    std::unique_ptr<IAccount> account;
    result = transaction->getForUpdate(uniqueIdentifier, account);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to lock account for withdrawal");
    }

    bool success = account->withdraw(amount);
    if (!success) {
        return false;
    }

    result = transaction->updateAccountBalance(*account);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to update account balance after withdrawal");
    }

    result = transaction->commit();
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to commit withdrawal transaction");
    }

    return true;
}

void AccountService::transferBetweenAccounts(const int fromUniqueIdentifier, const int toUniqueIdentifier, const double amount) const
{
    std::unique_ptr<ITransaction> transaction;
    DBOperationResult result = _dbConnection->beginTransaction(transaction);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to begin transaction for transfer");
    }

    std::unique_ptr<IAccount> fromAccount;
    result = transaction->getForUpdate(fromUniqueIdentifier, fromAccount);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to lock source account for transfer");
    }

    std::unique_ptr<IAccount> toAccount;
    result = transaction->getForUpdate(toUniqueIdentifier, toAccount);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to lock destination account for transfer");
    }

    if (!fromAccount->withdraw(amount)) {
        throw std::runtime_error("Insufficient funds for transfer");
    }

    toAccount->deposit(amount);
    result = transaction->updateAccountBalance(*fromAccount);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to update source account balance after transfer");
    }

    result = transaction->updateAccountBalance(*toAccount);
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to update destination account balance after transfer");
    }

    result = transaction->commit();
    if (result != DBOperationResult::Success) {
        throw std::runtime_error("Failed to commit transfer transaction");
    }
}
