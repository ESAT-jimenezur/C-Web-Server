#ifndef __SERVER_MANAGER_H__
#define __SERVER_MANAGER_H__

#include <WinSock2.h>
#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

using namespace std;

namespace iJos{

  class Server{
  public:

    // Singleton instance
    static Server *Instance();

    // Setters
    void setServerIP(char* ip);
    void setServerPort(int port);
    void serverStartedWithParams(bool status);

    //Getters
    char* getServerIP();
    int getServerPort();

    void init();
    void winsockInit();
    void slisten();
    void close();

  private:
    static Server *instance;
    bool started_with_port_param_;

    Server();

    void parseRequest(std::string req);

    void GETRequest(std::string req);

    char* getRequestPath(const char *buffer);
    char* getResourceExtension(const char *res);

    void sendRequestedContent(const char *res_name, const char *res_ext, const char *res_domain);


    const std::string base_url = "www";

    WSADATA wsa_;
    SOCKET socket_, socket_cliente_;

    bool debug_;

    struct sockaddr_in ip_, ip_c_;

    char buffer_[512];
    int size_, bytes_;

    int server_port_;
    char* server_ip_;

  };

}
#endif //__SERVER_MANAGER_H__