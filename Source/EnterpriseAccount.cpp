#include <EnterpriseAccount.h>

EnterpriseAccount::EnterpriseAccount(double balance, AccountId accountId, EnterpriseInformation enterpriseInformation)
    : Account(balance, accountId), _enterpriseInformation(enterpriseInformation)
{
}

nlohmann::json EnterpriseAccount::getAccountDetails() const
{
    nlohmann::json accountDetails = Account::getAccountDetails();
    accountDetails["enterpriseInformation"] = enterpriseInformation_to_json(_enterpriseInformation);
    return accountDetails;
};