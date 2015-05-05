#include "../include/server_manager.h"

namespace iJos{
  Server* Server::Instance() {
    static Server instance;
    return &instance;
  }

  Server::Server(){
  }

  void Server::init(){
    // Fill buffer with zeros
    memset(buffer_, '\0', 512);

    request_ = sizeof(ip_);
    server_port_ = 8080;
    server_ip_ = "0.0.0.0";
  }

  void Server::winsockInit(){

    if (WSAStartup(MAKEWORD(2, 0), &wsa_) != 0){
      printf("WSA Initialization failed.\n");
    }
    else{
      // Init socket
      socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if (socket_ == INVALID_SOCKET) {
        printf("Error en socket(): %ld\n", WSAGetLastError());
        WSACleanup();
      }

      ip_.sin_family = AF_INET;
      ip_.sin_addr.s_addr = inet_addr(server_ip_);
      ip_.sin_port = htons(server_port_);
      if (bind(socket_, (SOCKADDR*)&ip_, sizeof(ip_)) == SOCKET_ERROR){
        printf("\nError en bind: %ld\n",WSAGetLastError());
        WSACleanup();
      }
      else{

        if (listen(socket_, SOMAXCONN)){
          
        
        }

        printf("Server started succesfully.\n");
      }
      
    }
  }

  void Server::slisten(){

    while (true){
      socket_cliente_ = accept(socket_, (SOCKADDR*)&ip_c_, NULL);

      request_ = recvfrom(socket_cliente_, buffer_, 512, 0, (SOCKADDR*)&ip_, &request_);
      //printf("Cliente conectado con exito\n");
      //printf("IP cliente:%s", inet_ntoa(ip_c_.sin_addr));
      //printf("\n Puerto cliente:%d", ntohs(ip_c_.sin_port));
      
      if (request_ > 0){
        printf("%s \n", buffer_);
        memset(buffer_, '\0', 512);
      }
      

      closesocket(socket_cliente_);
    }
  }


  void Server::setServerIP(char* ip){
    server_ip_ = ip;
  }

  void Server::setServerPort(int port){
    server_port_ = port;
  }

  char* Server::getServerIP(){
    return server_ip_;
  }

  int Server::getServerPort(){
    return server_port_;
  }


  void Server::close(){
    closesocket(socket_);
  }
  
}

