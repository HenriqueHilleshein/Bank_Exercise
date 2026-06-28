#pragma once
#include <httplib.h>

class Server {
public:
    Server();
    void run(const std::string& host, int port);

private:
    httplib::Server svr;
    void setupRoutes();
};