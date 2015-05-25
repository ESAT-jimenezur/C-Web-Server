
#include "../include/server_manager.h"

int main(int argc, char** argv){

  iJos::Server* server = iJos::Server::Instance();
  server->init();

  // Handle port passed as parameter
  if (argc > 1) {
    const char* param = argv[1];
    int port = atoi(param);
    server->setServerPort(port);
    server->serverStartedWithParams(true);
  }

  server->winsockInit();
  server->slisten();
  server->close();

  return 0;
}