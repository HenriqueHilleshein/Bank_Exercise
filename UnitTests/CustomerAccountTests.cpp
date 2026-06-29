#include <gtest/gtest.h>
#include <CustomerAccount.h>
#include <CustomerInformation.h>
#include <AccountId.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class CustomerAccountTests : public ::testing::Test {
protected:
    AccountId testAccountId{2000, 1609459200000000000};
    CustomerInformation customerInfo{"John", "Doe"};
    double initialBalance = 500.0;
};

// Test constructor
TEST_F(CustomerAccountTests, ConstructorInitializesCorrectly) {
    CustomerAccount account(initialBalance, testAccountId, customerInfo);
    EXPECT_DOUBLE_EQ(account.getBalance(), initialBalance);
    EXPECT_EQ(account.getAccountId().UniqueIdentifier, testAccountId.UniqueIdentifier);
}

// Test getAccountDetails returns JSON with customer info
TEST_F(CustomerAccountTests, GetAccountDetailsIncludesCustomerInformation) {
    CustomerAccount account(initialBalance, testAccountId, customerInfo);
    json details = account.getAccountDetails();
    
    EXPECT_TRUE(details.contains("balance"));
    EXPECT_TRUE(details.contains("accountId"));
    EXPECT_TRUE(details.contains("customerInformation"));
    
    EXPECT_DOUBLE_EQ(details["balance"].get<double>(), initialBalance);
    EXPECT_EQ(details["customerInformation"]["firstName"].get<std::string>(), "John");
    EXPECT_EQ(details["customerInformation"]["lastName"].get<std::string>(), "Doe");
}

TEST_F(CustomerAccountTests, GetAccountDetailsDoesNotIncludeEnterpriseInformation) {
    CustomerAccount account(initialBalance, testAccountId, customerInfo);
    json details = account.getAccountDetails();
    
    EXPECT_FALSE(details.contains("enterpriseInformation"));
}

// Test that CustomerAccount inherits deposit behavior
TEST_F(CustomerAccountTests, InheritedDepositBehavior) {
    CustomerAccount account(initialBalance, testAccountId, customerInfo);
    account.deposit(100.0);
    EXPECT_DOUBLE_EQ(account.getBalance(), 600.0);
    
    json details = account.getAccountDetails();
    EXPECT_DOUBLE_EQ(details["balance"].get<double>(), 600.0);
}

// Test that CustomerAccount inherits withdraw behavior
TEST_F(CustomerAccountTests, InheritedWithdrawBehavior) {
    CustomerAccount account(initialBalance, testAccountId, customerInfo);
    bool success = account.withdraw(200.0);
    EXPECT_TRUE(success);
    EXPECT_DOUBLE_EQ(account.getBalance(), 300.0);
    
    json details = account.getAccountDetails();
    EXPECT_DOUBLE_EQ(details["balance"].get<double>(), 300.0);
}

// Test withdraw with insufficient funds
TEST_F(CustomerAccountTests, WithdrawInsufficientFundsDoesNotChangeBalance) {
    CustomerAccount account(initialBalance, testAccountId, customerInfo);
    bool success = account.withdraw(600.0);
    EXPECT_FALSE(success);
    EXPECT_DOUBLE_EQ(account.getBalance(), 500.0);
}

// Test accountId structure in JSON
TEST_F(CustomerAccountTests, AccountIdStructureInJSON) {
    CustomerAccount account(initialBalance, testAccountId, customerInfo);
    json details = account.getAccountDetails();
    
    EXPECT_TRUE(details["accountId"].contains("UniqueIdentifier"));
    EXPECT_TRUE(details["accountId"].contains("CreationTime"));
    EXPECT_EQ(details["accountId"]["UniqueIdentifier"].get<int>(), 2000);
    EXPECT_EQ(details["accountId"]["CreationTime"].get<long>(), 1609459200000000000);
}

// Test with different customer names
TEST_F(CustomerAccountTests, DifferentCustomerNames) {
    CustomerInformation info1{"Alice", "Smith"};
    CustomerInformation info2{"Bob", "Johnson"};
    
    CustomerAccount account1(1000.0, testAccountId, info1);
    CustomerAccount account2(2000.0, testAccountId, info2);
    
    json details1 = account1.getAccountDetails();
    json details2 = account2.getAccountDetails();
    
    EXPECT_EQ(details1["customerInformation"]["firstName"].get<std::string>(), "Alice");
    EXPECT_EQ(details2["customerInformation"]["firstName"].get<std::string>(), "Bob");
}

// Test with empty names
TEST_F(CustomerAccountTests, EmptyCustomerNames) {
    CustomerInformation info{"", ""};
    CustomerAccount account(initialBalance, testAccountId, info);
    json details = account.getAccountDetails();
    
    EXPECT_EQ(details["customerInformation"]["firstName"].get<std::string>(), "");
    EXPECT_EQ(details["customerInformation"]["lastName"].get<std::string>(), "");
}

// Test with special characters in names
TEST_F(CustomerAccountTests, SpecialCharactersInNames) {
    CustomerInformation info{"José", "Müller-O'Brien"};
    CustomerAccount account(initialBalance, testAccountId, info);
    json details = account.getAccountDetails();
    
    EXPECT_EQ(details["customerInformation"]["firstName"].get<std::string>(), "José");
    EXPECT_EQ(details["customerInformation"]["lastName"].get<std::string>(), "Müller-O'Brien");
}

// Test zero balance account
TEST_F(CustomerAccountTests, ZeroBalanceCustomerAccount) {
    CustomerAccount account(0.0, testAccountId, customerInfo);
    json details = account.getAccountDetails();
    EXPECT_DOUBLE_EQ(details["balance"].get<double>(), 0.0);
}
