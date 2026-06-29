#include <Server.h>
#include <nlohmann/json.hpp>
#include <iostream>

Server::Server(AccountService* accountService)
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
            int uniqueIdentifier = std::stoi(req.matches[1]);
            std::unique_ptr<IAccount> account = _accountService->getAccount(uniqueIdentifier);
            res.set_content(account->getAccountDetails().dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = 404;
            res.set_content(nlohmann::json{{"error", ex.what()}}.dump(), "application/json");
        }
    });

    svr.Post("/accounts/enterprises", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto json = nlohmann::json::parse(req.body);
            if (!json.contains("yTunnus") || !json.contains("companyName")) {
                res.status = 400;
                res.set_content(nlohmann::json{{"error", "Missing yTunnus or companyName"}}.dump(), "application/json");
                return;
            }

            EnterpriseInformation info{
                json["yTunnus"].get<std::string>(),
                json["companyName"].get<std::string>()
            };
            AccountId accountId = _accountService->createEnterpriseAccount(info);
            res.set_content(nlohmann::json{{"accountId", accountId_to_json(accountId)}}.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = 400;
            res.set_content(nlohmann::json{{"error", ex.what()}}.dump(), "application/json");
        }
    });

    svr.Post("/accounts/customers", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto json = nlohmann::json::parse(req.body);
            if (!json.contains("firstName") || !json.contains("lastName")) {
                res.status = 400;
                res.set_content(nlohmann::json{{"error", "Missing firstName or lastName"}}.dump(), "application/json");
                return;
            }

            CustomerInformation info{
                json["firstName"].get<std::string>(),
                json["lastName"].get<std::string>()
            };
            AccountId accountId = _accountService->createCustomerAccount(info);
            res.set_content(nlohmann::json{{"accountId", accountId_to_json(accountId)}}.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = 400;
            res.set_content(nlohmann::json{{"error", ex.what()}}.dump(), "application/json");
        }
    });

    svr.Post("/accounts/enterprises/lookup", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto json = nlohmann::json::parse(req.body);
            if (!json.contains("yTunnus")) {
                res.status = 400;
                res.set_content(nlohmann::json{{"error", "Missing yTunnus"}}.dump(), "application/json");
                return;
            }

            EnterpriseInformation info{
                json["yTunnus"].get<std::string>(),
                json.value("companyName", std::string())
            };
            AccountId accountId = _accountService->getAccountIdWithEnterpriseInformation(info);
            res.set_content(nlohmann::json{{"accountId", accountId_to_json(accountId)}}.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = 404;
            res.set_content(nlohmann::json{{"error", ex.what()}}.dump(), "application/json");
        }
    });

    svr.Post("/accounts/customers/lookup", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto json = nlohmann::json::parse(req.body);
            if (!json.contains("firstName") || !json.contains("lastName")) {
                res.status = 400;
                res.set_content(nlohmann::json{{"error", "Missing firstName or lastName"}}.dump(), "application/json");
                return;
            }

            CustomerInformation info{
                json["firstName"].get<std::string>(),
                json["lastName"].get<std::string>()
            };
            AccountId accountId = _accountService->getAccountIdWithCustomerInformation(info);
            res.set_content(nlohmann::json{{"accountId", accountId_to_json(accountId)}}.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = 404;
            res.set_content(nlohmann::json{{"error", ex.what()}}.dump(), "application/json");
        }
    });

    svr.Post(R"(/accounts/(\d+)/deposit)", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto json = nlohmann::json::parse(req.body);
            if (!json.contains("amount")) {
                res.status = 400;
                res.set_content(nlohmann::json{{"error", "Missing amount"}}.dump(), "application/json");
                return;
            }

            int uniqueIdentifier = std::stoi(req.matches[1]);
            double amount = json["amount"].get<double>();
            _accountService->depositToAccount(uniqueIdentifier, amount);
            res.set_content(nlohmann::json{{"success", true}}.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = 400;
            res.set_content(nlohmann::json{{"error", ex.what()}}.dump(), "application/json");
        }
    });

    svr.Post(R"(/accounts/(\d+)/withdraw)", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto json = nlohmann::json::parse(req.body);
            if (!json.contains("amount")) {
                res.status = 400;
                res.set_content(nlohmann::json{{"error", "Missing amount"}}.dump(), "application/json");
                return;
            }

            int uniqueIdentifier = std::stoi(req.matches[1]);
            double amount = json["amount"].get<double>();
            bool success = _accountService->withdrawFromAccount(uniqueIdentifier, amount);
            if (!success) {
                res.status = 400;
                res.set_content(nlohmann::json{{"success", false}, {"error", "Insufficient funds"}}.dump(), "application/json");
                return;
            }
            res.set_content(nlohmann::json{{"success", true}}.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = 400;
            res.set_content(nlohmann::json{{"error", ex.what()}}.dump(), "application/json");
        }
    });

    svr.Post(R"(/accounts/(\d+)/transfer)", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto json = nlohmann::json::parse(req.body);
            if (!json.contains("toUniqueIdentifier") || !json.contains("amount")) {
                res.status = 400;
                res.set_content(nlohmann::json{{"error", "Missing toUniqueIdentifier or amount"}}.dump(), "application/json");
                return;
            }

            int fromUniqueIdentifier = std::stoi(req.matches[1]);
            int toUniqueIdentifier = json["toUniqueIdentifier"].get<int>();
            double amount = json["amount"].get<double>();
            _accountService->transferBetweenAccounts(fromUniqueIdentifier, toUniqueIdentifier, amount);
            res.set_content(nlohmann::json{{"success", true}}.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = 400;
            res.set_content(nlohmann::json{{"error", ex.what()}}.dump(), "application/json");
        }
    });
}
