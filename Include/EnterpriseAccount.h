#pragma once
#include <Account.h>
#include <EnterpriseInformation.h>

class EnterpriseAccount : public Account
{
public:
    EnterpriseAccount(double balance, AccountId accountId, const EnterpriseInformation enterpriseInformation);
    nlohmann::json getAccountDetails() const override;

private:
    EnterpriseInformation _enterpriseInformation;
};
