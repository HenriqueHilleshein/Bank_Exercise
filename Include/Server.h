#pragma once
#include <httplib.h>
#include <IAccountService.h>

class Server {
public:
    Server(IAccountService* accountService);
    void run(const std::string& host, int port);

private:
    httplib::Server svr;
    IAccountService* _accountService;
    void setupRoutes();
};