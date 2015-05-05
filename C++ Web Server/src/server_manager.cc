#include "../include/server_manager.h"

namespace iJos{
  Server* Server::Instance() {
    static Server instance;
    return &instance;
  }

  void Server::init(){
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 0), &wsa);

    if (WSAStartup(MAKEWORD(2, 0), &wsa) != 0){
      printf("WSA Initialization failed.\n");
    }
    else{
      printf("Server Started. \n");
    }
  }

  void Server::listen(){
    while (true){

    }
  }
  
}

