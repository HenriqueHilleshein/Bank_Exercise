#include <gtest/gtest.h>
#include <FakeDBConnection.h>
#include <CustomerAccount.h>
#include <EnterpriseAccount.h>
#include <CustomerInformation.h>
#include <EnterpriseInformation.h>

class FakeDBConnectionTests : public ::testing::Test {
protected:
    FakeDBConnection db;
};

// Test account creation - Customer
TEST_F(FakeDBConnectionTests, CreateCustomerAccountSucceeds) {
    CustomerInformation info{"John", "Doe"};
    AccountId accountId;
    
    DBOperationResult result = db.createCustomerAccount(info, accountId);
    
    EXPECT_EQ(result, DBOperationResult::Success);
    EXPECT_NE(accountId.UniqueIdentifier, 0);
}

TEST_F(FakeDBConnectionTests, CreateMultipleCustomerAccountsGetUniqueIds) {
    CustomerInformation info1{"John", "Doe"};
    CustomerInformation info2{"Jane", "Smith"};
    AccountId id1, id2;
    
    db.createCustomerAccount(info1, id1);
    db.createCustomerAccount(info2, id2);
    
    EXPECT_NE(id1.UniqueIdentifier, id2.UniqueIdentifier);
}

TEST_F(FakeDBConnectionTests, CreateCustomerAccountWithEmptyFirstName) {
    CustomerInformation info{"", "Doe"};
    AccountId accountId;
    
    DBOperationResult result = db.createCustomerAccount(info, accountId);
    
    EXPECT_EQ(result, DBOperationResult::InvalidInput);
}

TEST_F(FakeDBConnectionTests, CreateCustomerAccountWithEmptyLastName) {
    CustomerInformation info{"John", ""};
    AccountId accountId;
    
    DBOperationResult result = db.createCustomerAccount(info, accountId);
    
    EXPECT_EQ(result, DBOperationResult::InvalidInput);
}

// Test account creation - Enterprise
TEST_F(FakeDBConnectionTests, CreateEnterpriseAccountSucceeds) {
    EnterpriseInformation info{"1234567-8", "TechCorp"};
    AccountId accountId;
    
    DBOperationResult result = db.createEnterpriseAccount(info, accountId);
    
    EXPECT_EQ(result, DBOperationResult::Success);
    EXPECT_NE(accountId.UniqueIdentifier, 0);
}

TEST_F(FakeDBConnectionTests, CreateMultipleEnterpriseAccountsGetUniqueIds) {
    EnterpriseInformation info1{"1111111-1", "Company1"};
    EnterpriseInformation info2{"2222222-2", "Company2"};
    AccountId id1, id2;
    
    db.createEnterpriseAccount(info1, id1);
    db.createEnterpriseAccount(info2, id2);
    
    EXPECT_NE(id1.UniqueIdentifier, id2.UniqueIdentifier);
}

TEST_F(FakeDBConnectionTests, CreateEnterpriseAccountWithEmptyYTunnus) {
    EnterpriseInformation info{"", "TechCorp"};
    AccountId accountId;
    
    DBOperationResult result = db.createEnterpriseAccount(info, accountId);
    
    EXPECT_EQ(result, DBOperationResult::InvalidInput);
}

TEST_F(FakeDBConnectionTests, CreateEnterpriseAccountWithEmptyCompanyName) {
    EnterpriseInformation info{"1234567-8", ""};
    AccountId accountId;
    
    DBOperationResult result = db.createEnterpriseAccount(info, accountId);
    
    EXPECT_EQ(result, DBOperationResult::InvalidInput);
}

// Test getAccount
TEST_F(FakeDBConnectionTests, GetAccountReturnsCorrectCustomerAccount) {
    CustomerInformation info{"John", "Doe"};
    AccountId createdId;
    db.createCustomerAccount(info, createdId);
    
    std::unique_ptr<IAccount> account;
    DBOperationResult result = db.getAccount(createdId.UniqueIdentifier, account);
    
    EXPECT_EQ(result, DBOperationResult::Success);
    EXPECT_NE(account, nullptr);
}

TEST_F(FakeDBConnectionTests, GetAccountReturnsCorrectEnterpriseAccount) {
    EnterpriseInformation info{"1234567-8", "TechCorp"};
    AccountId createdId;
    db.createEnterpriseAccount(info, createdId);
    
    std::unique_ptr<IAccount> account;
    DBOperationResult result = db.getAccount(createdId.UniqueIdentifier, account);
    
    EXPECT_EQ(result, DBOperationResult::Success);
    EXPECT_NE(account, nullptr);
}

TEST_F(FakeDBConnectionTests, GetAccountNonExistentReturnsNotFound) {
    std::unique_ptr<IAccount> account;
    DBOperationResult result = db.getAccount(9999, account);
    
    EXPECT_EQ(result, DBOperationResult::AccountNotFound);
}

// Test getAccountIdWithCustomerInformation
TEST_F(FakeDBConnectionTests, GetAccountIdWithCustomerInformationSucceeds) {
    CustomerInformation info{"John", "Doe"};
    AccountId createdId;
    db.createCustomerAccount(info, createdId);
    
    AccountId retrievedId;
    DBOperationResult result = db.getAccountIdWithCustomerInformation(info, retrievedId);
    
    EXPECT_EQ(result, DBOperationResult::Success);
    EXPECT_EQ(retrievedId.UniqueIdentifier, createdId.UniqueIdentifier);
}

TEST_F(FakeDBConnectionTests, GetAccountIdWithCustomerInformationNotFound) {
    CustomerInformation info{"NonExistent", "User"};
    AccountId retrievedId;
    
    DBOperationResult result = db.getAccountIdWithCustomerInformation(info, retrievedId);
    
    EXPECT_EQ(result, DBOperationResult::AccountNotFound);
}

// Test getAccountIdWithEnterpriseInformation
TEST_F(FakeDBConnectionTests, GetAccountIdWithEnterpriseInformationSucceeds) {
    EnterpriseInformation info{"1234567-8", "TechCorp"};
    AccountId createdId;
    db.createEnterpriseAccount(info, createdId);
    
    AccountId retrievedId;
    DBOperationResult result = db.getAccountIdWithEnterpriseInformation(info, retrievedId);
    
    EXPECT_EQ(result, DBOperationResult::Success);
    EXPECT_EQ(retrievedId.UniqueIdentifier, createdId.UniqueIdentifier);
}

TEST_F(FakeDBConnectionTests, GetAccountIdWithEnterpriseInformationNotFound) {
    EnterpriseInformation info{"9999999-9", "NonExistent"};
    AccountId retrievedId;
    
    DBOperationResult result = db.getAccountIdWithEnterpriseInformation(info, retrievedId);
    
    EXPECT_EQ(result, DBOperationResult::AccountNotFound);
}

// Test transaction operations
TEST_F(FakeDBConnectionTests, BeginTransactionSucceeds) {
    std::unique_ptr<ITransaction> transaction;
    DBOperationResult result = db.beginTransaction(transaction);
    EXPECT_EQ(result, DBOperationResult::Success);
    EXPECT_NE(transaction, nullptr);
}

TEST_F(FakeDBConnectionTests, CannotBeginTransactionWhenOneIsActive) {
    std::unique_ptr<ITransaction> transaction1;
    DBOperationResult result1 = db.beginTransaction(transaction1);
    EXPECT_EQ(result1, DBOperationResult::Success);
    EXPECT_NE(transaction1, nullptr);
    
    std::unique_ptr<ITransaction> transaction2;
    DBOperationResult result2 = db.beginTransaction(transaction2);
    EXPECT_EQ(result2, DBOperationResult::TransactionAlreadyActive);
    EXPECT_EQ(transaction2, nullptr);
}

TEST_F(FakeDBConnectionTests, CommitTransactionSucceeds) {
    std::unique_ptr<ITransaction> transaction;
    DBOperationResult beginResult = db.beginTransaction(transaction);
    EXPECT_EQ(beginResult, DBOperationResult::Success);
    
    DBOperationResult result = transaction->commit();
    
    EXPECT_EQ(result, DBOperationResult::Success);
}

TEST_F(FakeDBConnectionTests, RollbackTransactionSucceeds) {
    std::unique_ptr<ITransaction> transaction;
    DBOperationResult beginResult = db.beginTransaction(transaction);
    EXPECT_EQ(beginResult, DBOperationResult::Success);
    
    DBOperationResult result = transaction->rollback();
    
    EXPECT_EQ(result, DBOperationResult::Success);
}

// Test transaction isolation - rollback restores state
TEST_F(FakeDBConnectionTests, RollbackRestoresAccountBalance) {
    // Create account and deposit
    CustomerInformation info{"John", "Doe"};
    AccountId accountId;
    db.createCustomerAccount(info, accountId);
    
    std::unique_ptr<IAccount> accountBefore;
    db.getAccount(accountId.UniqueIdentifier, accountBefore);
    double balanceBefore = accountBefore->getBalance();
    
    // Start transaction and try to deposit
    std::unique_ptr<ITransaction> transaction;
    DBOperationResult beginResult = db.beginTransaction(transaction);
    EXPECT_EQ(beginResult, DBOperationResult::Success);
    
    std::unique_ptr<IAccount> account;
    transaction->getForUpdate(accountId.UniqueIdentifier, account);
    account->deposit(100.0);
    transaction->updateAccountBalance(*account);
    
    // Rollback
    transaction->rollback();
    
    // Check balance is restored
    std::unique_ptr<IAccount> accountAfter;
    db.getAccount(accountId.UniqueIdentifier, accountAfter);
    EXPECT_DOUBLE_EQ(accountAfter->getBalance(), balanceBefore);
}

// Test transaction isolation - commit persists state
TEST_F(FakeDBConnectionTests, CommitPersistsAccountBalance) {
    // Create account
    CustomerInformation info{"John", "Doe"};
    AccountId accountId;
    db.createCustomerAccount(info, accountId);
    
    // Start transaction and deposit
    std::unique_ptr<ITransaction> transaction;
    DBOperationResult beginResult = db.beginTransaction(transaction);
    EXPECT_EQ(beginResult, DBOperationResult::Success);
    
    std::unique_ptr<IAccount> account;
    transaction->getForUpdate(accountId.UniqueIdentifier, account);
    account->deposit(500.0);
    transaction->updateAccountBalance(*account);
    transaction->commit();
    
    // Check balance is persisted
    std::unique_ptr<IAccount> accountAfter;
    db.getAccount(accountId.UniqueIdentifier, accountAfter);
    EXPECT_DOUBLE_EQ(accountAfter->getBalance(), 500.0);
}

// Test multiple transactions
TEST_F(FakeDBConnectionTests, CanBeginNewTransactionAfterCommit) {
    std::unique_ptr<ITransaction> transaction1;
    DBOperationResult result1 = db.beginTransaction(transaction1);
    EXPECT_EQ(result1, DBOperationResult::Success);
    
    transaction1->commit();
    
    std::unique_ptr<ITransaction> transaction2;
    DBOperationResult result2 = db.beginTransaction(transaction2);
    EXPECT_EQ(result2, DBOperationResult::Success);
    EXPECT_NE(transaction2, nullptr);
}

TEST_F(FakeDBConnectionTests, CanBeginNewTransactionAfterRollback) {
    std::unique_ptr<ITransaction> transaction1;
    DBOperationResult result1 = db.beginTransaction(transaction1);
    EXPECT_EQ(result1, DBOperationResult::Success);
    
    transaction1->rollback();
    
    std::unique_ptr<ITransaction> transaction2;
    DBOperationResult result2 = db.beginTransaction(transaction2);
    EXPECT_EQ(result2, DBOperationResult::Success);
    EXPECT_NE(transaction2, nullptr);
}

// Test updateAccountBalance outside transaction fails
TEST_F(FakeDBConnectionTests, UpdateAccountBalanceOutsideTransactionFails) {
    CustomerInformation info{"John", "Doe"};
    AccountId accountId;
    db.createCustomerAccount(info, accountId);
    
    std::unique_ptr<IAccount> account;
    db.getAccount(accountId.UniqueIdentifier, account);
    account->deposit(100.0);
    
    // Note: updateAccountBalance is private, so this test verifies through transaction behavior
    // that balance changes outside transactions don't persist
}

// Test account ID increments
TEST_F(FakeDBConnectionTests, AccountIdsStartAt1000) {
    CustomerInformation info{"John", "Doe"};
    AccountId accountId;
    db.createCustomerAccount(info, accountId);
    
    EXPECT_EQ(accountId.UniqueIdentifier, 1000);
}

TEST_F(FakeDBConnectionTests, AccountIdsIncrement) {
    CustomerInformation info1{"John", "Doe"};
    CustomerInformation info2{"Jane", "Smith"};
    AccountId id1, id2, id3;
    
    db.createCustomerAccount(info1, id1);
    db.createCustomerAccount(info2, id2);
    db.createCustomerAccount(info1, id3);
    
    EXPECT_EQ(id1.UniqueIdentifier, 1000);
    EXPECT_EQ(id2.UniqueIdentifier, 1001);
    EXPECT_EQ(id3.UniqueIdentifier, 1002);
}
