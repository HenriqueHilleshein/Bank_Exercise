#include <Account.h>

Account::Account(double balance, AccountId accountId)
    : _balance(balance), _accountId(accountId)
{
}

double Account::getBalance() const
{
    return _balance;
}

void Account::deposit(double amount)
{
    _balance += amount;
}

bool Account::withdraw(double amount)
{
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
