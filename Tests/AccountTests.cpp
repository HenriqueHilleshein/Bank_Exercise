#include <gtest/gtest.h>
#include <Account.h>
#include <AccountId.h>

class AccountTests : public ::testing::Test {
protected:
    AccountId testAccountId{1000, 1609459200000000000};
    double initialBalance = 100.0;
};

// Test deposit operation
TEST_F(AccountTests, DepositIncreasesBalance) {
    Account account(initialBalance, testAccountId);
    account.deposit(50.0);
    EXPECT_DOUBLE_EQ(account.getBalance(), 150.0);
}

TEST_F(AccountTests, DepositZeroAmount) {
    Account account(initialBalance, testAccountId);
    account.deposit(0.0);
    EXPECT_DOUBLE_EQ(account.getBalance(), 100.0);
}

TEST_F(AccountTests, DepositMultipleTimes) {
    Account account(initialBalance, testAccountId);
    account.deposit(25.0);
    account.deposit(25.0);
    account.deposit(50.0);
    EXPECT_DOUBLE_EQ(account.getBalance(), 200.0);
}

TEST_F(AccountTests, DepositLargeAmount) {
    Account account(initialBalance, testAccountId);
    account.deposit(1000000.0);
    EXPECT_DOUBLE_EQ(account.getBalance(), 1000100.0);
}

// Test withdraw operation
TEST_F(AccountTests, WithdrawDecreasesBalance) {
    Account account(initialBalance, testAccountId);
    bool success = account.withdraw(30.0);
    EXPECT_TRUE(success);
    EXPECT_DOUBLE_EQ(account.getBalance(), 70.0);
}

TEST_F(AccountTests, WithdrawZeroAmount) {
    Account account(initialBalance, testAccountId);
    bool success = account.withdraw(0.0);
    EXPECT_TRUE(success);
    EXPECT_DOUBLE_EQ(account.getBalance(), 100.0);
}

TEST_F(AccountTests, WithdrawInsufficientFunds) {
    Account account(initialBalance, testAccountId);
    bool success = account.withdraw(150.0);
    EXPECT_FALSE(success);
    EXPECT_DOUBLE_EQ(account.getBalance(), 100.0);
}

TEST_F(AccountTests, WithdrawEntireBalance) {
    Account account(initialBalance, testAccountId);
    bool success = account.withdraw(100.0);
    EXPECT_TRUE(success);
    EXPECT_DOUBLE_EQ(account.getBalance(), 0.0);
}

TEST_F(AccountTests, WithdrawMoreThanBalance) {
    Account account(initialBalance, testAccountId);
    bool success = account.withdraw(100.1);
    EXPECT_FALSE(success);
    EXPECT_DOUBLE_EQ(account.getBalance(), 100.0);
}

TEST_F(AccountTests, WithdrawMultipleTimes) {
    Account account(initialBalance, testAccountId);
    EXPECT_TRUE(account.withdraw(25.0));
    EXPECT_TRUE(account.withdraw(25.0));
    EXPECT_TRUE(account.withdraw(25.0));
    EXPECT_DOUBLE_EQ(account.getBalance(), 25.0);
}

// Test getBalance
TEST_F(AccountTests, GetBalanceReturnsCorrectValue) {
    Account account(initialBalance, testAccountId);
    EXPECT_DOUBLE_EQ(account.getBalance(), 100.0);
}

TEST_F(AccountTests, GetBalanceAfterOperations) {
    Account account(initialBalance, testAccountId);
    account.deposit(50.0);
    account.withdraw(30.0);
    EXPECT_DOUBLE_EQ(account.getBalance(), 120.0);
}

// Test getAccountId
TEST_F(AccountTests, GetAccountIdReturnsCorrectId) {
    Account account(initialBalance, testAccountId);
    AccountId retrievedId = account.getAccountId();
    EXPECT_EQ(retrievedId.UniqueIdentifier, testAccountId.UniqueIdentifier);
    EXPECT_EQ(retrievedId.CreationTime, testAccountId.CreationTime);
}

// Test constructor with zero balance
TEST_F(AccountTests, ConstructorWithZeroBalance) {
    Account account(0.0, testAccountId);
    EXPECT_DOUBLE_EQ(account.getBalance(), 0.0);
}

// Test constructor with negative balance (edge case)
TEST_F(AccountTests, ConstructorWithNegativeBalance) {
    Account account(-50.0, testAccountId);
    EXPECT_DOUBLE_EQ(account.getBalance(), -50.0);
}

// Test precision with decimal amounts
TEST_F(AccountTests, DepositWithDecimalPrecision) {
    Account account(100.50, testAccountId);
    account.deposit(25.75);
    EXPECT_DOUBLE_EQ(account.getBalance(), 126.25);
}

TEST_F(AccountTests, WithdrawWithDecimalPrecision) {
    Account account(100.75, testAccountId);
    bool success = account.withdraw(50.25);
    EXPECT_TRUE(success);
    EXPECT_DOUBLE_EQ(account.getBalance(), 50.50);
}
