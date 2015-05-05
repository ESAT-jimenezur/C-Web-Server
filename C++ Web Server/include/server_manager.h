#ifndef __SERVER_MANAGER_H__
#define __SERVER_MANAGER_H__

#include <WinSock2.h>
#include <stdio.h>


namespace iJos{

  class Server{
  public:

    // Singleton instance
    static Server *Instance();

    void init();
    void listen();

  private:
    static Server *instance;



  };

}
#endif //__SERVER_MANAGER_H__