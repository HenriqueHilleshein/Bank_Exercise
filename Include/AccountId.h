#pragma once
#include <nlohmann/json.hpp>

struct AccountId {
    int UniqueIdentifier;
    long CreationTime;
};


inline nlohmann::json accountId_to_json(const AccountId& a) {
    return {{"UniqueIdentifier", a.UniqueIdentifier}, {"CreationTime", a.CreationTime}};
};