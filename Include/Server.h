#pragma once
#include <httplib.h>
#include <AccountService.h>

class Server {
public:
    Server(AccountService* accountService);
    void run(const std::string& host, int port);

private:
    httplib::Server svr;
    AccountService* _accountService;
    void setupRoutes();
};