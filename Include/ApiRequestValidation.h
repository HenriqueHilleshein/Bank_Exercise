#pragma once

#include <AccountId.h>
#include <nlohmann/json.hpp>

#include <stdexcept>
#include <string>

namespace ApiRequestValidation {

struct CustomerAccountRequest {
    std::string firstName;
    std::string lastName;
};

struct EnterpriseAccountRequest {
    std::string yTunnus;
    std::string companyName;
};

struct CustomerLookupRequest {
    std::string firstName;
    std::string lastName;
};

struct EnterpriseLookupRequest {
    std::string yTunnus;
    std::string companyName;
};

struct AccountAmountRequest {
    double amount;
};

struct TransferRequest {
    AccountId::Identifier toUniqueIdentifier;
    double amount;
};

inline nlohmann::json parseJsonBody(const std::string& body)
{
    try {
        return nlohmann::json::parse(body);
    } catch (const nlohmann::json::parse_error&) {
        throw std::invalid_argument("Request body must be valid JSON");
    }
}

inline std::string requireStringField(const nlohmann::json& body, const char* fieldName)
{
    if (!body.contains(fieldName)) {
        throw std::invalid_argument(std::string("Missing ") + fieldName);
    }

    if (!body.at(fieldName).is_string()) {
        throw std::invalid_argument(std::string("Field ") + fieldName + " must be a string");
    }

    return body.at(fieldName).get<std::string>();
}

inline std::string optionalStringField(const nlohmann::json& body, const char* fieldName)
{
    if (!body.contains(fieldName) || body.at(fieldName).is_null()) {
        return "";
    }

    if (!body.at(fieldName).is_string()) {
        throw std::invalid_argument(std::string("Field ") + fieldName + " must be a string");
    }

    return body.at(fieldName).get<std::string>();
}

inline double requirePositiveDoubleField(const nlohmann::json& body, const char* fieldName)
{
    if (!body.contains(fieldName)) {
        throw std::invalid_argument(std::string("Missing ") + fieldName);
    }

    if (!body.at(fieldName).is_number()) {
        throw std::invalid_argument(std::string("Field ") + fieldName + " must be numeric");
    }

    const double value = body.at(fieldName).get<double>();
    if (value <= 0.0) {
        throw std::invalid_argument(std::string("Field ") + fieldName + " must be greater than zero");
    }

    return value;
}

inline AccountId::Identifier requireIntegerField(const nlohmann::json& body, const char* fieldName)
{
    if (!body.contains(fieldName)) {
        throw std::invalid_argument(std::string("Missing ") + fieldName);
    }

    if (!body.at(fieldName).is_number_integer() && !body.at(fieldName).is_number_unsigned()) {
        throw std::invalid_argument(std::string("Field ") + fieldName + " must be an integer");
    }

    return body.at(fieldName).get<AccountId::Identifier>();
}

inline CustomerAccountRequest parseCustomerAccountRequest(const std::string& body)
{
    const auto json = parseJsonBody(body);
    return {
        requireStringField(json, "firstName"),
        requireStringField(json, "lastName")
    };
}

inline EnterpriseAccountRequest parseEnterpriseAccountRequest(const std::string& body)
{
    const auto json = parseJsonBody(body);
    return {
        requireStringField(json, "yTunnus"),
        requireStringField(json, "companyName")
    };
}

inline CustomerLookupRequest parseCustomerLookupRequest(const std::string& body)
{
    const auto json = parseJsonBody(body);
    return {
        requireStringField(json, "firstName"),
        requireStringField(json, "lastName")
    };
}

inline EnterpriseLookupRequest parseEnterpriseLookupRequest(const std::string& body)
{
    const auto json = parseJsonBody(body);
    return {
        requireStringField(json, "yTunnus"),
        optionalStringField(json, "companyName")
    };
}

inline AccountAmountRequest parseAccountAmountRequest(const std::string& body)
{
    const auto json = parseJsonBody(body);
    return {
        requirePositiveDoubleField(json, "amount")
    };
}

inline TransferRequest parseTransferRequest(const std::string& body)
{
    const auto json = parseJsonBody(body);
    return {
        requireIntegerField(json, "toUniqueIdentifier"),
        requirePositiveDoubleField(json, "amount")
    };
}

} // namespace ApiRequestValidation