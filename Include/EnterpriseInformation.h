#pragma once
#include <nlohmann/json.hpp>
#include <string>

struct EnterpriseInformation {
    std::string YTunnus;
    std::string CompanyName;
};

inline nlohmann::json enterpriseInformation_to_json(const EnterpriseInformation& a) {
    return {{"yTunnus", a.YTunnus}, {"companyName", a.CompanyName}};
};