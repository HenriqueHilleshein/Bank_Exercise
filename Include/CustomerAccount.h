#pragma once
#include <Account.h>
#include <CustomerInformation.h>

class CustomerAccount : public Account
{
public:
    CustomerAccount(double balance, AccountId accountId, const CustomerInformation customerInformation);
    nlohmann::json getAccountDetails() const override;

private:
    CustomerInformation _customerInformation;
};
