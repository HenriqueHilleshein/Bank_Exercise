#pragma once
#include <memory>
#include <IAccount.h>
#include <DBOperationResult.h>

class ITransaction
{
public:
    virtual ~ITransaction() = default;

    virtual DBOperationResult getForUpdate(const int uniqueIdentifier, std::unique_ptr<IAccount>& outAccount) = 0;
    virtual DBOperationResult updateAccountBalance(const IAccount& account) = 0;
    virtual DBOperationResult commit() = 0;
    virtual DBOperationResult rollback() = 0;
};
