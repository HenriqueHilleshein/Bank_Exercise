#pragma once
#include <IAccount.h>

class Account : public IAccount
{
public:
    Account(double balance, const AccountId accountId);
    double getBalance() const override;
    void deposit(const double amount) override;
    bool withdraw(const double amount) override;
    AccountId getAccountId() const override;
    virtual nlohmann::json getAccountDetails() const override;
private:
    double _balance;
    const AccountId _accountId;
};
