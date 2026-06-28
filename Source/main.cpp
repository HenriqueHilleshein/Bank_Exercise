#include <FakeDBConnection.h>
#include <AccountService.h>
#include <Server.h>

int main() {
    FakeDBConnection db;
    AccountService service(&db);
    Server server(&service);
    server.run("0.0.0.0", 8080);
    return 0;
}