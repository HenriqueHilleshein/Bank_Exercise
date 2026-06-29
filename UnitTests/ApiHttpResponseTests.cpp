#include <gtest/gtest.h>

#include <ApiHttpResponse.h>

TEST(ApiHttpResponseTests, ReturnsBadRequestForInvalidArgument) {
    const std::invalid_argument error("Request body must be valid JSON");

    EXPECT_EQ(ApiHttpResponse::statusCodeForException(error), 400);
}

TEST(ApiHttpResponseTests, ReturnsNotFoundForNotFoundMessage) {
    const std::runtime_error error("Account not found");

    EXPECT_EQ(ApiHttpResponse::statusCodeForException(error), 404);
}

TEST(ApiHttpResponseTests, ReturnsBadRequestForOtherRuntimeErrors) {
    const std::runtime_error error("Failed to commit transfer transaction");

    EXPECT_EQ(ApiHttpResponse::statusCodeForException(error), 400);
}