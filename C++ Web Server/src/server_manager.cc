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

    debug_ = true;

    size_ = sizeof(ip_);
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
          printf("Error Apertura puerto");
        }
        else{
          if (debug_){
            printf("Server started succesfully.\n");
          }
        }
      }
      
    }
  }

  void Server::slisten(){

    while (true){
      socket_cliente_ = accept(socket_, (SOCKADDR*)&ip_c_, NULL);

      bytes_ = recvfrom(socket_cliente_, buffer_, 512, 0, (SOCKADDR*)&ip_, &size_);
      
      std::string request(buffer_);

      if (bytes_ > 0){
        if (debug_){
          cout << request << endl;
        }
        parseRequest(request);
        memset(buffer_, '\0', 512);
      }
      

      closesocket(socket_cliente_);
    }
  }


  void Server::parseRequest(std::string req){
    // Parse request to know if is a GET request

    if (req.find("GET") != std::string::npos) {
      // GET REQUEST
      GETRequest(req);
    }
    else if (req.find("POST") != std::string::npos){
      //POST REQUEST
    }
    else if (req.find("PUT") != std::string::npos){
      //PUT REQUEST
    }
    else if (req.find("DELETE") != std::string::npos){
      //DELETE REQUEST
    }

  }

  void Server::GETRequest(std::string req){
    const char * buf = req.c_str();

    // Get requested resource (Path)
    char* resource_path = getRequestPath(buf);
    // Get extension of that file
    std::string resource_extension(resource_path);
    resource_extension = resource_extension.substr(resource_extension.find_last_of(".") + 1);
    
    sendRequestedContent(resource_path, resource_extension.c_str());
  }

  void Server::sendRequestedContent(const char *res_name, const char *res_ext){
    std:string return_buffer = "HTTP/1.1 200 - OK\n";

    //printf("Req: %s\n", res_name);
    printf("Req: %s\n", res_ext);

    if (strcmp(res_ext, "html") == 0 || strcmp(res_ext, "htm") == 0){
        return_buffer += "content-type: text/html\n";
    }

    if (strcmp(res_ext, "css") == 0){
      return_buffer += "content-type: text/css\n";
    }

    if ( strcmp(res_ext, "jpg") == 0 
      || strcmp(res_ext, "jpeg") == 0
      || strcmp(res_ext, "png") == 0){
      return_buffer += "content-type: image/*\n";
    }

    if (strcmp(res_ext, "js") == 0){
      return_buffer += "content-type: application/javascript\n";
    }


    /*
    if (strcmp(res_ext, "ico") == 0){
      return_buffer += "content-type: text/css\n";
    }
    */

    /* LOAD FILE*/
    std::stringstream file_streamstring;
    std::string file_string;

    std::string resource_full_path = base_url;
                resource_full_path += res_name;
    std::ifstream file(resource_full_path);

    //printf("%s", resource_full_path.c_str());
    
    if (file.is_open()){
      file_streamstring << file.rdbuf();
      file.close();
      file_string = file_streamstring.str();
    }
    const char* file_src = file_string.c_str();

    std::stringstream file_size_str;
    std::string str;
    file_size_str << file_string.size();
    file_size_str >> str;

    return_buffer += "accept-ranges: bytes\n";
    return_buffer += "content-lenght: ";
    return_buffer += str;
    return_buffer += "\n";
    return_buffer += "connection: keep-Alive\n\n";
    
    return_buffer += file_src;

    send(socket_cliente_, return_buffer.c_str(), file_string.size(), 0);
    

  }
  
  
  char* Server::getRequestPath(const char *buf){
    
    // Parse GET request
    // http://stackoverflow.com/questions/24462863/parse-url-path-of-get-request

    const char *start = buf;
    const char *end;
    char *path = NULL;
    size_t pathLen;

    /* Verify that there is a 'GET ' at the beginning of the string. */
    if (strncmp("GET ", start, 4)){
      printf("Parse error: 'GET ' is missing.\n");
      WSACleanup();
    }
    
    /* Set the start pointer at the first character beyond the 'GET '. */
    start += 4;

    /* From the start position, set the end pointer to the first white-space character found in the string. */
    end = start;

    while (*end && !isspace(*end)){
      ++end;

      /* Calculate the path length, and allocate sufficient memory for the path plus string termination. */
      pathLen = (end - start);
      path = (char*)malloc(pathLen + 1);
      if (NULL == path){
        fprintf(stderr, "malloc() failed. \n");
      }

    }

    /* Copy the path string to the path storage. */
    memcpy(path, start, pathLen);
    
    /* Terminate the string. */
    path[pathLen] = '\0';

    /* Return the allocated storage, or NULL in the event of an error, to the caller. */
    return(path);
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

