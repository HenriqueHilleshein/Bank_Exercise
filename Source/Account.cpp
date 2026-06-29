#include <Account.h>
#include <stdexcept>

Account::Account(double balance, AccountId accountId)
    : _balance(balance), _accountId(accountId)
{
    if (balance < 0.0) {
        throw std::invalid_argument("Initial balance cannot be negative");
    }
}

double Account::getBalance() const
{
    return _balance;
}

void Account::deposit(double amount)
{
    if (amount <= 0.0) {
        throw std::invalid_argument("Amount must be greater than zero");
    }

    _balance += amount;
}

bool Account::withdraw(double amount)
{
    if (amount <= 0.0) {
        throw std::invalid_argument("Amount must be greater than zero");
    }

    if (_balance < amount) {
        return false;
    }
    _balance -= amount;
    return true;
}

AccountId Account::getAccountId() const
{
    return _accountId;
}

nlohmann::json Account::getAccountDetails() const
{
    nlohmann::json accountDetails = {
        {"balance", _balance},
        {"accountId", accountId_to_json(_accountId)}
    };
    return accountDetails;
};
