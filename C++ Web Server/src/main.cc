
#include "../include/server_manager.h"

int main(){


  iJos::Server* server = iJos::Server::Instance();
  server->init();
  server->winsockInit();
  server->slisten();
  server->close();
  

  return 0;
}