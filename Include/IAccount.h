#pragma once
#include <AccountId.h>
#include <nlohmann/json.hpp>

class IAccount
{
public:
    virtual double getBalance() const = 0;
    virtual void deposit(const double amount) = 0;
    virtual bool withdraw(const double amount) = 0;
    virtual AccountId getAccountId() const = 0;
    virtual nlohmann::json getAccountDetails() const = 0;
};
