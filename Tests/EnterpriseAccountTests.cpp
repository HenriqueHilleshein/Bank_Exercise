#include <gtest/gtest.h>
#include <EnterpriseAccount.h>
#include <EnterpriseInformation.h>
#include <AccountId.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class EnterpriseAccountTests : public ::testing::Test {
protected:
    AccountId testAccountId{3000, 1609459200000000000};
    EnterpriseInformation enterpriseInfo{"1234567-8", "TechCorp Ltd"};
    double initialBalance = 10000.0;
};

// Test constructor
TEST_F(EnterpriseAccountTests, ConstructorInitializesCorrectly) {
    EnterpriseAccount account(initialBalance, testAccountId, enterpriseInfo);
    EXPECT_DOUBLE_EQ(account.getBalance(), initialBalance);
    EXPECT_EQ(account.getAccountId().UniqueIdentifier, testAccountId.UniqueIdentifier);
}

// Test getAccountDetails returns JSON with enterprise info
TEST_F(EnterpriseAccountTests, GetAccountDetailsIncludesEnterpriseInformation) {
    EnterpriseAccount account(initialBalance, testAccountId, enterpriseInfo);
    json details = account.getAccountDetails();
    
    EXPECT_TRUE(details.contains("balance"));
    EXPECT_TRUE(details.contains("accountId"));
    EXPECT_TRUE(details.contains("enterpriseInformation"));
    
    EXPECT_DOUBLE_EQ(details["balance"].get<double>(), initialBalance);
    EXPECT_EQ(details["enterpriseInformation"]["yTunnus"].get<std::string>(), "1234567-8");
    EXPECT_EQ(details["enterpriseInformation"]["companyName"].get<std::string>(), "TechCorp Ltd");
}

TEST_F(EnterpriseAccountTests, GetAccountDetailsDoesNotIncludeCustomerInformation) {
    EnterpriseAccount account(initialBalance, testAccountId, enterpriseInfo);
    json details = account.getAccountDetails();
    
    EXPECT_FALSE(details.contains("customerInformation"));
}

// Test that EnterpriseAccount inherits deposit behavior
TEST_F(EnterpriseAccountTests, InheritedDepositBehavior) {
    EnterpriseAccount account(initialBalance, testAccountId, enterpriseInfo);
    account.deposit(5000.0);
    EXPECT_DOUBLE_EQ(account.getBalance(), 15000.0);
    
    json details = account.getAccountDetails();
    EXPECT_DOUBLE_EQ(details["balance"].get<double>(), 15000.0);
}

// Test that EnterpriseAccount inherits withdraw behavior
TEST_F(EnterpriseAccountTests, InheritedWithdrawBehavior) {
    EnterpriseAccount account(initialBalance, testAccountId, enterpriseInfo);
    bool success = account.withdraw(3000.0);
    EXPECT_TRUE(success);
    EXPECT_DOUBLE_EQ(account.getBalance(), 7000.0);
    
    json details = account.getAccountDetails();
    EXPECT_DOUBLE_EQ(details["balance"].get<double>(), 7000.0);
}

// Test withdraw with insufficient funds
TEST_F(EnterpriseAccountTests, WithdrawInsufficientFundsDoesNotChangeBalance) {
    EnterpriseAccount account(initialBalance, testAccountId, enterpriseInfo);
    bool success = account.withdraw(15000.0);
    EXPECT_FALSE(success);
    EXPECT_DOUBLE_EQ(account.getBalance(), 10000.0);
}

// Test accountId structure in JSON
TEST_F(EnterpriseAccountTests, AccountIdStructureInJSON) {
    EnterpriseAccount account(initialBalance, testAccountId, enterpriseInfo);
    json details = account.getAccountDetails();
    
    EXPECT_TRUE(details["accountId"].contains("UniqueIdentifier"));
    EXPECT_TRUE(details["accountId"].contains("CreationTime"));
    EXPECT_EQ(details["accountId"]["UniqueIdentifier"].get<int>(), 3000);
    EXPECT_EQ(details["accountId"]["CreationTime"].get<long>(), 1609459200000000000);
}

// Test with different enterprise information
TEST_F(EnterpriseAccountTests, DifferentEnterpriseInformation) {
    EnterpriseInformation info1{"9876543-1", "FinanceInc"};
    EnterpriseInformation info2{"5555555-5", "RetailCorp"};
    
    EnterpriseAccount account1(1000.0, testAccountId, info1);
    EnterpriseAccount account2(2000.0, testAccountId, info2);
    
    json details1 = account1.getAccountDetails();
    json details2 = account2.getAccountDetails();
    
    EXPECT_EQ(details1["enterpriseInformation"]["companyName"].get<std::string>(), "FinanceInc");
    EXPECT_EQ(details2["enterpriseInformation"]["companyName"].get<std::string>(), "RetailCorp");
}

// Test with empty YTunnus and company name
TEST_F(EnterpriseAccountTests, EmptyEnterpriseInformation) {
    EnterpriseInformation info{"", ""};
    EnterpriseAccount account(initialBalance, testAccountId, info);
    json details = account.getAccountDetails();
    
    EXPECT_EQ(details["enterpriseInformation"]["yTunnus"].get<std::string>(), "");
    EXPECT_EQ(details["enterpriseInformation"]["companyName"].get<std::string>(), "");
}

// Test with special characters in company name
TEST_F(EnterpriseAccountTests, SpecialCharactersInCompanyName) {
    EnterpriseInformation info{"1111111-1", "Tech & Finance GmbH"};
    EnterpriseAccount account(initialBalance, testAccountId, info);
    json details = account.getAccountDetails();
    
    EXPECT_EQ(details["enterpriseInformation"]["companyName"].get<std::string>(), "Tech & Finance GmbH");
}

// Test zero balance enterprise account
TEST_F(EnterpriseAccountTests, ZeroBalanceEnterpriseAccount) {
    EnterpriseAccount account(0.0, testAccountId, enterpriseInfo);
    json details = account.getAccountDetails();
    EXPECT_DOUBLE_EQ(details["balance"].get<double>(), 0.0);
}

// Test large balance
TEST_F(EnterpriseAccountTests, LargeBalanceEnterpriseAccount) {
    double largeBalance = 999999999.99;
    EnterpriseAccount account(largeBalance, testAccountId, enterpriseInfo);
    EXPECT_DOUBLE_EQ(account.getBalance(), largeBalance);
}

// Test multiple deposits and withdrawals
TEST_F(EnterpriseAccountTests, MultipleTransactions) {
    EnterpriseAccount account(initialBalance, testAccountId, enterpriseInfo);
    
    account.deposit(2000.0);
    EXPECT_DOUBLE_EQ(account.getBalance(), 12000.0);
    
    account.withdraw(1000.0);
    EXPECT_DOUBLE_EQ(account.getBalance(), 11000.0);
    
    account.deposit(5000.0);
    EXPECT_DOUBLE_EQ(account.getBalance(), 16000.0);
    
    bool success = account.withdraw(16000.0);
    EXPECT_TRUE(success);
    EXPECT_DOUBLE_EQ(account.getBalance(), 0.0);
}
