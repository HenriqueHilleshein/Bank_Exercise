#pragma once

enum class DBOperationResult
{
    Success,
    AccountNotFound,
    InvalidInput,
    OperationFailed,
    TransactionAlreadyActive,
    NoActiveTransaction
};
