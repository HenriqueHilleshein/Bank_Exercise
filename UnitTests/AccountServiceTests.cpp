#include <gtest/gtest.h>
#include <AccountService.h>
#include <FakeDBConnection.h>
#include <CustomerInformation.h>
#include <EnterpriseInformation.h>
#include <functional>

class AccountServiceTests : public ::testing::Test {
protected:
    FakeDBConnection db;
    AccountService service{&db};
};

static void expectRuntimeErrorMessage(const std::function<void()>& action, const std::string& expectedMessage)
{
    try {
        action();
        FAIL() << "Expected runtime_error";
    } catch (const std::runtime_error& error) {
        EXPECT_EQ(error.what(), expectedMessage);
    }
}

// Test customer account creation
TEST_F(AccountServiceTests, CreateCustomerAccountReturnsValidAccountId) {
    CustomerInformation info{"John", "Doe"};
    
    AccountId accountId = service.createCustomerAccount(info);
    
    EXPECT_NE(accountId.UniqueIdentifier, 0);
    EXPECT_NE(accountId.CreationTime, 0);
}

TEST_F(AccountServiceTests, CreateCustomerAccountThrowsOnMissingFirstName) {
    CustomerInformation info{"", "Doe"};
    
    EXPECT_THROW(service.createCustomerAccount(info), std::runtime_error);
}

TEST_F(AccountServiceTests, CreateCustomerAccountThrowsOnMissingLastName) {
    CustomerInformation info{"John", ""};
    
    EXPECT_THROW(service.createCustomerAccount(info), std::runtime_error);
}

// Test enterprise account creation
TEST_F(AccountServiceTests, CreateEnterpriseAccountReturnsValidAccountId) {
    EnterpriseInformation info{"1234567-8", "TechCorp"};
    
    AccountId accountId = service.createEnterpriseAccount(info);
    
    EXPECT_NE(accountId.UniqueIdentifier, 0);
    EXPECT_NE(accountId.CreationTime, 0);
}

TEST_F(AccountServiceTests, CreateEnterpriseAccountThrowsOnMissingYTunnus) {
    EnterpriseInformation info{"", "TechCorp"};
    
    EXPECT_THROW(service.createEnterpriseAccount(info), std::runtime_error);
}

TEST_F(AccountServiceTests, CreateEnterpriseAccountThrowsOnMissingCompanyName) {
    EnterpriseInformation info{"1234567-8", ""};
    
    EXPECT_THROW(service.createEnterpriseAccount(info), std::runtime_error);
}

// Test getAccount
TEST_F(AccountServiceTests, GetAccountReturnsValidAccount) {
    CustomerInformation info{"John", "Doe"};
    AccountId createdId = service.createCustomerAccount(info);
    
    std::unique_ptr<IAccount> account = service.getAccount(createdId.UniqueIdentifier);
    
    EXPECT_NE(account, nullptr);
}

TEST_F(AccountServiceTests, GetAccountThrowsOnNonExistentAccount) {
    expectRuntimeErrorMessage([this]() { service.getAccount(9999); }, "Account not found");
}

// Test getAccountIdWithCustomerInformation
TEST_F(AccountServiceTests, GetAccountIdWithCustomerInformationReturnsCorrectId) {
    CustomerInformation info{"John", "Doe"};
    AccountId createdId = service.createCustomerAccount(info);
    
    AccountId retrievedId = service.getAccountIdWithCustomerInformation(info);
    
    EXPECT_EQ(retrievedId.UniqueIdentifier, createdId.UniqueIdentifier);
}

TEST_F(AccountServiceTests, GetAccountIdWithCustomerInformationThrowsOnNotFound) {
    CustomerInformation info{"NonExistent", "User"};
    
    expectRuntimeErrorMessage([this, &info]() { service.getAccountIdWithCustomerInformation(info); }, "Account not found");
}

// Test getAccountIdWithEnterpriseInformation
TEST_F(AccountServiceTests, GetAccountIdWithEnterpriseInformationReturnsCorrectId) {
    EnterpriseInformation info{"1234567-8", "TechCorp"};
    AccountId createdId = service.createEnterpriseAccount(info);
    
    AccountId retrievedId = service.getAccountIdWithEnterpriseInformation(info);
    
    EXPECT_EQ(retrievedId.UniqueIdentifier, createdId.UniqueIdentifier);
}

TEST_F(AccountServiceTests, GetAccountIdWithEnterpriseInformationThrowsOnNotFound) {
    EnterpriseInformation info{"9999999-9", "NonExistent"};
    
    expectRuntimeErrorMessage([this, &info]() { service.getAccountIdWithEnterpriseInformation(info); }, "Account not found");
}

// Test deposit
TEST_F(AccountServiceTests, DepositIncreasesAccountBalance) {
    CustomerInformation info{"John", "Doe"};
    AccountId accountId = service.createCustomerAccount(info);
    
    service.depositToAccount(accountId.UniqueIdentifier, 500.0);
    
    std::unique_ptr<IAccount> account = service.getAccount(accountId.UniqueIdentifier);
    EXPECT_DOUBLE_EQ(account->getBalance(), 500.0);
}

TEST_F(AccountServiceTests, DepositMultipleTimes) {
    CustomerInformation info{"John", "Doe"};
    AccountId accountId = service.createCustomerAccount(info);
    
    service.depositToAccount(accountId.UniqueIdentifier, 100.0);
    service.depositToAccount(accountId.UniqueIdentifier, 200.0);
    service.depositToAccount(accountId.UniqueIdentifier, 300.0);
    
    std::unique_ptr<IAccount> account = service.getAccount(accountId.UniqueIdentifier);
    EXPECT_DOUBLE_EQ(account->getBalance(), 600.0);
}

TEST_F(AccountServiceTests, DepositZeroAmount) {
    CustomerInformation info{"John", "Doe"};
    AccountId accountId = service.createCustomerAccount(info);
    
    service.depositToAccount(accountId.UniqueIdentifier, 0.0);
    
    std::unique_ptr<IAccount> account = service.getAccount(accountId.UniqueIdentifier);
    EXPECT_DOUBLE_EQ(account->getBalance(), 0.0);
}

TEST_F(AccountServiceTests, DepositThrowsOnNonExistentAccount) {
    expectRuntimeErrorMessage([this]() { service.depositToAccount(9999, 100.0); }, "Account not found");
}

// Test withdraw
TEST_F(AccountServiceTests, WithdrawDecreasesAccountBalance) {
    CustomerInformation info{"John", "Doe"};
    AccountId accountId = service.createCustomerAccount(info);
    service.depositToAccount(accountId.UniqueIdentifier, 500.0);
    
    bool success = service.withdrawFromAccount(accountId.UniqueIdentifier, 200.0);
    
    EXPECT_TRUE(success);
    std::unique_ptr<IAccount> account = service.getAccount(accountId.UniqueIdentifier);
    EXPECT_DOUBLE_EQ(account->getBalance(), 300.0);
}

TEST_F(AccountServiceTests, WithdrawMoreThanBalanceReturnsFalse) {
    CustomerInformation info{"John", "Doe"};
    AccountId accountId = service.createCustomerAccount(info);
    service.depositToAccount(accountId.UniqueIdentifier, 100.0);
    
    bool success = service.withdrawFromAccount(accountId.UniqueIdentifier, 200.0);
    
    EXPECT_FALSE(success);
    std::unique_ptr<IAccount> account = service.getAccount(accountId.UniqueIdentifier);
    EXPECT_DOUBLE_EQ(account->getBalance(), 100.0);
}

TEST_F(AccountServiceTests, WithdrawEntireBalance) {
    CustomerInformation info{"John", "Doe"};
    AccountId accountId = service.createCustomerAccount(info);
    service.depositToAccount(accountId.UniqueIdentifier, 500.0);
    
    bool success = service.withdrawFromAccount(accountId.UniqueIdentifier, 500.0);
    
    EXPECT_TRUE(success);
    std::unique_ptr<IAccount> account = service.getAccount(accountId.UniqueIdentifier);
    EXPECT_DOUBLE_EQ(account->getBalance(), 0.0);
}

TEST_F(AccountServiceTests, WithdrawFromEmptyAccountReturnsFalse) {
    CustomerInformation info{"John", "Doe"};
    AccountId accountId = service.createCustomerAccount(info);
    
    bool success = service.withdrawFromAccount(accountId.UniqueIdentifier, 1.0);
    
    EXPECT_FALSE(success);
}

TEST_F(AccountServiceTests, WithdrawZeroAmount) {
    CustomerInformation info{"John", "Doe"};
    AccountId accountId = service.createCustomerAccount(info);
    service.depositToAccount(accountId.UniqueIdentifier, 100.0);
    
    bool success = service.withdrawFromAccount(accountId.UniqueIdentifier, 0.0);
    
    EXPECT_TRUE(success);
    std::unique_ptr<IAccount> account = service.getAccount(accountId.UniqueIdentifier);
    EXPECT_DOUBLE_EQ(account->getBalance(), 100.0);
}

TEST_F(AccountServiceTests, WithdrawThrowsOnNonExistentAccount) {
    expectRuntimeErrorMessage([this]() { service.withdrawFromAccount(9999, 100.0); }, "Account not found");
}

// Test transfer
TEST_F(AccountServiceTests, TransferMovesMoneyBetweenAccounts) {
    CustomerInformation info1{"John", "Doe"};
    CustomerInformation info2{"Jane", "Smith"};
    
    AccountId id1 = service.createCustomerAccount(info1);
    AccountId id2 = service.createCustomerAccount(info2);
    
    service.depositToAccount(id1.UniqueIdentifier, 1000.0);
    service.depositToAccount(id2.UniqueIdentifier, 500.0);
    
    service.transferBetweenAccounts(id1.UniqueIdentifier, id2.UniqueIdentifier, 300.0);
    
    std::unique_ptr<IAccount> account1 = service.getAccount(id1.UniqueIdentifier);
    std::unique_ptr<IAccount> account2 = service.getAccount(id2.UniqueIdentifier);
    
    EXPECT_DOUBLE_EQ(account1->getBalance(), 700.0);
    EXPECT_DOUBLE_EQ(account2->getBalance(), 800.0);
}

TEST_F(AccountServiceTests, TransferThrowsOnInsufficientFunds) {
    CustomerInformation info1{"John", "Doe"};
    CustomerInformation info2{"Jane", "Smith"};
    
    AccountId id1 = service.createCustomerAccount(info1);
    AccountId id2 = service.createCustomerAccount(info2);
    
    service.depositToAccount(id1.UniqueIdentifier, 100.0);
    service.depositToAccount(id2.UniqueIdentifier, 500.0);
    
    EXPECT_THROW(service.transferBetweenAccounts(id1.UniqueIdentifier, id2.UniqueIdentifier, 200.0), std::runtime_error);
}

TEST_F(AccountServiceTests, TransferEntireBalance) {
    CustomerInformation info1{"John", "Doe"};
    CustomerInformation info2{"Jane", "Smith"};
    
    AccountId id1 = service.createCustomerAccount(info1);
    AccountId id2 = service.createCustomerAccount(info2);
    
    service.depositToAccount(id1.UniqueIdentifier, 500.0);
    
    service.transferBetweenAccounts(id1.UniqueIdentifier, id2.UniqueIdentifier, 500.0);
    
    std::unique_ptr<IAccount> account1 = service.getAccount(id1.UniqueIdentifier);
    std::unique_ptr<IAccount> account2 = service.getAccount(id2.UniqueIdentifier);
    
    EXPECT_DOUBLE_EQ(account1->getBalance(), 0.0);
    EXPECT_DOUBLE_EQ(account2->getBalance(), 500.0);
}

TEST_F(AccountServiceTests, TransferZeroAmount) {
    CustomerInformation info1{"John", "Doe"};
    CustomerInformation info2{"Jane", "Smith"};
    
    AccountId id1 = service.createCustomerAccount(info1);
    AccountId id2 = service.createCustomerAccount(info2);
    
    service.depositToAccount(id1.UniqueIdentifier, 1000.0);
    service.depositToAccount(id2.UniqueIdentifier, 500.0);
    
    service.transferBetweenAccounts(id1.UniqueIdentifier, id2.UniqueIdentifier, 0.0);
    
    std::unique_ptr<IAccount> account1 = service.getAccount(id1.UniqueIdentifier);
    std::unique_ptr<IAccount> account2 = service.getAccount(id2.UniqueIdentifier);
    
    EXPECT_DOUBLE_EQ(account1->getBalance(), 1000.0);
    EXPECT_DOUBLE_EQ(account2->getBalance(), 500.0);
}

TEST_F(AccountServiceTests, TransferToSameAccountWorks) {
    CustomerInformation info{"John", "Doe"};
    AccountId id = service.createCustomerAccount(info);
    
    service.depositToAccount(id.UniqueIdentifier, 1000.0);
    
    EXPECT_THROW(service.transferBetweenAccounts(id.UniqueIdentifier, id.UniqueIdentifier, 100.0), std::runtime_error);
    
    std::unique_ptr<IAccount> account = service.getAccount(id.UniqueIdentifier);
    EXPECT_DOUBLE_EQ(account->getBalance(), 1000.0);
}

TEST_F(AccountServiceTests, TransferThrowsOnNonExistentFromAccount) {
    CustomerInformation info{"Jane", "Smith"};
    AccountId id2 = service.createCustomerAccount(info);
    
    expectRuntimeErrorMessage([this, &id2]() { service.transferBetweenAccounts(9999, id2.UniqueIdentifier, 100.0); }, "Account not found");
}

TEST_F(AccountServiceTests, TransferThrowsOnNonExistentToAccount) {
    CustomerInformation info{"John", "Doe"};
    AccountId id1 = service.createCustomerAccount(info);
    service.depositToAccount(id1.UniqueIdentifier, 1000.0);
    
    expectRuntimeErrorMessage([this, &id1]() { service.transferBetweenAccounts(id1.UniqueIdentifier, 9999, 100.0); }, "Account not found");
}

// Test complex scenarios
TEST_F(AccountServiceTests, MultipleOperationsOnSingleAccount) {
    CustomerInformation info{"John", "Doe"};
    AccountId id = service.createCustomerAccount(info);
    
    service.depositToAccount(id.UniqueIdentifier, 1000.0);
    EXPECT_TRUE(service.withdrawFromAccount(id.UniqueIdentifier, 200.0));
    service.depositToAccount(id.UniqueIdentifier, 500.0);
    EXPECT_TRUE(service.withdrawFromAccount(id.UniqueIdentifier, 300.0));
    
    std::unique_ptr<IAccount> account = service.getAccount(id.UniqueIdentifier);
    EXPECT_DOUBLE_EQ(account->getBalance(), 1000.0);
}

TEST_F(AccountServiceTests, EnterpriseAccountOperations) {
    EnterpriseInformation info{"1234567-8", "TechCorp"};
    AccountId id = service.createEnterpriseAccount(info);
    
    service.depositToAccount(id.UniqueIdentifier, 50000.0);
    EXPECT_TRUE(service.withdrawFromAccount(id.UniqueIdentifier, 10000.0));
    
    std::unique_ptr<IAccount> account = service.getAccount(id.UniqueIdentifier);
    EXPECT_DOUBLE_EQ(account->getBalance(), 40000.0);
}

TEST_F(AccountServiceTests, TransferBetweenCustomerAndEnterpriseAccounts) {
    CustomerInformation custInfo{"John", "Doe"};
    EnterpriseInformation entInfo{"1234567-8", "TechCorp"};
    
    AccountId custId = service.createCustomerAccount(custInfo);
    AccountId entId = service.createEnterpriseAccount(entInfo);
    
    service.depositToAccount(custId.UniqueIdentifier, 5000.0);
    service.transferBetweenAccounts(custId.UniqueIdentifier, entId.UniqueIdentifier, 3000.0);
    
    std::unique_ptr<IAccount> custAccount = service.getAccount(custId.UniqueIdentifier);
    std::unique_ptr<IAccount> entAccount = service.getAccount(entId.UniqueIdentifier);
    
    EXPECT_DOUBLE_EQ(custAccount->getBalance(), 2000.0);
    EXPECT_DOUBLE_EQ(entAccount->getBalance(), 3000.0);
}
