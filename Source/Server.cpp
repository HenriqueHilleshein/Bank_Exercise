#include <Server.h>
#include <ApiRequestValidation.h>
#include <ApiHttpResponse.h>
#include <nlohmann/json.hpp>
#include <iostream>

namespace {

AccountId::Identifier parseIdentifier(const std::string& rawIdentifier)
{
    return std::stoi(rawIdentifier);
}

} // namespace

Server::Server(IAccountService* accountService)
    : _accountService(accountService)
{
    setupRoutes();
}

void Server::run(const std::string& host, int port)
{
    std::cout << "Server listening on " << host << ":" << port << std::endl;
    svr.listen(host.c_str(), port);
}

void Server::setupRoutes()
{
    // Set CORS headers on all requests (pre-routing, before any handler)
    svr.set_pre_routing_handler([](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Accept");
        return httplib::Server::HandlerResponse::Unhandled;
    });

    // Handle preflight (OPTIONS) requests
    svr.Options(R"(/.*)", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("", "application/json");
    });

    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("OK", "text/plain");
    });

    svr.Get(R"(/accounts/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            AccountId::Identifier uniqueIdentifier = parseIdentifier(req.matches[1]);
            std::unique_ptr<IAccount> account = _accountService->getAccount(uniqueIdentifier);
            res.set_content(account->getAccountDetails().dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = ApiHttpResponse::statusCodeForException(ex);
            res.set_content(nlohmann::json{{"error", ex.what()}}.dump(), "application/json");
        }
    });

    svr.Post("/accounts/enterprises", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            const auto request = ApiRequestValidation::parseEnterpriseAccountRequest(req.body);
            EnterpriseInformation info{request.yTunnus, request.companyName};
            AccountId accountId = _accountService->createEnterpriseAccount(info);
            res.set_content(nlohmann::json{{"accountId", accountId_to_json(accountId)}}.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = ApiHttpResponse::statusCodeForException(ex);
            res.set_content(nlohmann::json{{"error", ex.what()}}.dump(), "application/json");
        }
    });

    svr.Post("/accounts/customers", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            const auto request = ApiRequestValidation::parseCustomerAccountRequest(req.body);
            CustomerInformation info{request.firstName, request.lastName};
            AccountId accountId = _accountService->createCustomerAccount(info);
            res.set_content(nlohmann::json{{"accountId", accountId_to_json(accountId)}}.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = ApiHttpResponse::statusCodeForException(ex);
            res.set_content(nlohmann::json{{"error", ex.what()}}.dump(), "application/json");
        }
    });

    svr.Post("/accounts/enterprises/lookup", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            const auto request = ApiRequestValidation::parseEnterpriseLookupRequest(req.body);
            EnterpriseInformation info{request.yTunnus, request.companyName};
            AccountId accountId = _accountService->getAccountIdWithEnterpriseInformation(info);
            res.set_content(nlohmann::json{{"accountId", accountId_to_json(accountId)}}.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = ApiHttpResponse::statusCodeForException(ex);
            res.set_content(nlohmann::json{{"error", ex.what()}}.dump(), "application/json");
        }
    });

    svr.Post("/accounts/customers/lookup", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            const auto request = ApiRequestValidation::parseCustomerLookupRequest(req.body);
            CustomerInformation info{request.firstName, request.lastName};
            AccountId accountId = _accountService->getAccountIdWithCustomerInformation(info);
            res.set_content(nlohmann::json{{"accountId", accountId_to_json(accountId)}}.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = ApiHttpResponse::statusCodeForException(ex);
            res.set_content(nlohmann::json{{"error", ex.what()}}.dump(), "application/json");
        }
    });

    svr.Post(R"(/accounts/(\d+)/deposit)", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            const auto request = ApiRequestValidation::parseAccountAmountRequest(req.body);
            AccountId::Identifier uniqueIdentifier = parseIdentifier(req.matches[1]);
            _accountService->depositToAccount(uniqueIdentifier, request.amount);
            res.set_content(nlohmann::json{{"success", true}}.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = ApiHttpResponse::statusCodeForException(ex);
            res.set_content(nlohmann::json{{"error", ex.what()}}.dump(), "application/json");
        }
    });

    svr.Post(R"(/accounts/(\d+)/withdraw)", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            const auto request = ApiRequestValidation::parseAccountAmountRequest(req.body);
            AccountId::Identifier uniqueIdentifier = parseIdentifier(req.matches[1]);
            bool success = _accountService->withdrawFromAccount(uniqueIdentifier, request.amount);
            if (!success) {
                res.status = 400;
                res.set_content(nlohmann::json{{"success", false}, {"error", "Insufficient funds"}}.dump(), "application/json");
                return;
            }
            res.set_content(nlohmann::json{{"success", true}}.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = ApiHttpResponse::statusCodeForException(ex);
            res.set_content(nlohmann::json{{"error", ex.what()}}.dump(), "application/json");
        }
    });

    svr.Post(R"(/accounts/(\d+)/transfer)", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            const auto request = ApiRequestValidation::parseTransferRequest(req.body);
            AccountId::Identifier fromUniqueIdentifier = parseIdentifier(req.matches[1]);
            _accountService->transferBetweenAccounts(fromUniqueIdentifier, request.toUniqueIdentifier, request.amount);
            res.set_content(nlohmann::json{{"success", true}}.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = ApiHttpResponse::statusCodeForException(ex);
            res.set_content(nlohmann::json{{"error", ex.what()}}.dump(), "application/json");
        }
    });
}
