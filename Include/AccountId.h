#pragma once
#include <cstdint>
#include <nlohmann/json.hpp>

struct AccountId {
    using Identifier = int;
    using CreationTimestamp = std::int64_t;

    Identifier UniqueIdentifier;
    CreationTimestamp CreationTime;
};


inline nlohmann::json accountId_to_json(const AccountId& a) {
    return {{"UniqueIdentifier", a.UniqueIdentifier}, {"CreationTime", a.CreationTime}};
};