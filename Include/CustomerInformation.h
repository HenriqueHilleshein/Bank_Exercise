#pragma once
#include <nlohmann/json.hpp>
#include <string>

struct CustomerInformation {
    std::string FirstName;
    std::string LastName;
};

inline nlohmann::json customerInformation_to_json(const CustomerInformation& a) {
    return {{"firstName", a.FirstName}, {"lastName", a.LastName}};
};