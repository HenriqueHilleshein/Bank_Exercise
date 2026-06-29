#include <gtest/gtest.h>

#include <ApiRequestValidation.h>

using namespace ApiRequestValidation;

TEST(ApiRequestValidationTests, ParseCustomerAccountRequestSucceeds) {
    const auto request = parseCustomerAccountRequest(R"({"firstName":"John","lastName":"Doe"})");

    EXPECT_EQ(request.firstName, "John");
    EXPECT_EQ(request.lastName, "Doe");
}

TEST(ApiRequestValidationTests, ParseCustomerAccountRequestThrowsOnInvalidJson) {
    EXPECT_THROW(parseCustomerAccountRequest("not json"), std::invalid_argument);
}

TEST(ApiRequestValidationTests, ParseCustomerAccountRequestThrowsOnMissingField) {
    EXPECT_THROW(parseCustomerAccountRequest(R"({"firstName":"John"})"), std::invalid_argument);
}

TEST(ApiRequestValidationTests, ParseCustomerAccountRequestThrowsOnWrongType) {
    EXPECT_THROW(parseCustomerAccountRequest(R"({"firstName":123,"lastName":"Doe"})"), std::invalid_argument);
}

TEST(ApiRequestValidationTests, ParseEnterpriseAccountRequestSucceeds) {
    const auto request = parseEnterpriseAccountRequest(R"({"yTunnus":"1234567-8","companyName":"TechCorp"})");

    EXPECT_EQ(request.yTunnus, "1234567-8");
    EXPECT_EQ(request.companyName, "TechCorp");
}

TEST(ApiRequestValidationTests, ParseEnterpriseLookupRequestAllowsMissingCompanyName) {
    const auto request = parseEnterpriseLookupRequest(R"({"yTunnus":"1234567-8"})");

    EXPECT_EQ(request.yTunnus, "1234567-8");
    EXPECT_EQ(request.companyName, "");
}

TEST(ApiRequestValidationTests, ParseAccountAmountRequestSucceeds) {
    const auto request = parseAccountAmountRequest(R"({"amount":50.25})");

    EXPECT_DOUBLE_EQ(request.amount, 50.25);
}

TEST(ApiRequestValidationTests, ParseAccountAmountRequestThrowsOnMissingAmount) {
    EXPECT_THROW(parseAccountAmountRequest(R"({})"), std::invalid_argument);
}

TEST(ApiRequestValidationTests, ParseAccountAmountRequestThrowsOnZeroAmount) {
    EXPECT_THROW(parseAccountAmountRequest(R"({"amount":0})"), std::invalid_argument);
}

TEST(ApiRequestValidationTests, ParseAccountAmountRequestThrowsOnNegativeAmount) {
    EXPECT_THROW(parseAccountAmountRequest(R"({"amount":-1})"), std::invalid_argument);
}

TEST(ApiRequestValidationTests, ParseAccountAmountRequestThrowsOnWrongType) {
    EXPECT_THROW(parseAccountAmountRequest(R"({"amount":"50"})"), std::invalid_argument);
}

TEST(ApiRequestValidationTests, ParseTransferRequestSucceeds) {
    const auto request = parseTransferRequest(R"({"toUniqueIdentifier":1001,"amount":25.0})");

    EXPECT_EQ(request.toUniqueIdentifier, 1001);
    EXPECT_DOUBLE_EQ(request.amount, 25.0);
}

TEST(ApiRequestValidationTests, ParseTransferRequestThrowsOnWrongTargetType) {
    EXPECT_THROW(parseTransferRequest(R"({"toUniqueIdentifier":"1001","amount":25.0})"), std::invalid_argument);
}

TEST(ApiRequestValidationTests, ParseTransferRequestThrowsOnZeroAmount) {
    EXPECT_THROW(parseTransferRequest(R"({"toUniqueIdentifier":1001,"amount":0})"), std::invalid_argument);
}

TEST(ApiRequestValidationTests, ParseTransferRequestThrowsOnNegativeAmount) {
    EXPECT_THROW(parseTransferRequest(R"({"toUniqueIdentifier":1001,"amount":-25.0})"), std::invalid_argument);
}