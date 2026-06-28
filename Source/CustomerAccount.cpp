#include <CustomerAccount.h>

CustomerAccount::CustomerAccount(double balance, AccountId accountId, CustomerInformation customerInformation)
    : Account(balance, accountId), _customerInformation(customerInformation)
{
}

nlohmann::json CustomerAccount::getAccountDetails() const
{
    nlohmann::json accountDetails = Account::getAccountDetails();
    accountDetails["customerInformation"] = customerInformation_to_json(_customerInformation);
    return accountDetails;
};