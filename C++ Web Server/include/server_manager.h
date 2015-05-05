#ifndef __SERVER_MANAGER_H__
#define __SERVER_MANAGER_H__

#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include <string.h>


namespace iJos{

  class Server{
  public:

    // Singleton instance
    static Server *Instance();

    // Setters
    void setServerIP(char* ip);
    void setServerPort(int port);

    //Getters
    char* getServerIP();
    int getServerPort();

    void init();
    void winsockInit();
    void slisten();
    void close();

  private:
    static Server *instance;

    Server();

    WSADATA wsa_;
    SOCKET socket_, socket_cliente_;

    struct sockaddr_in ip_, ip_c_;

    char buffer_[512];
    int request_, bytes_;

    int server_port_;
    char* server_ip_;

  };

}
#endif //__SERVER_MANAGER_H__