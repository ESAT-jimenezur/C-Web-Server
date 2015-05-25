#include "../include/server_manager.h"

namespace iJos{
  Server* Server::Instance() {
    static Server instance;
    return &instance;
  }

  Server::Server(){}

  void Server::init(){
    // Fill buffer with zeros
    memset(buffer_, '\0', 1024);

    debug_ = false;

    started_with_port_param_ = false;

    size_ = sizeof(ip_);
    server_port_ = 8080;
    server_ip_ = "0.0.0.0";
  }

  void Server::winsockInit(){

    if (!started_with_port_param_){
      unsigned int n_port;
      cout << "Select server port (Default will be 8080) ";
      cin >> n_port;

      if (n_port <= 1024 || n_port > 65535){
        cout << "Port should be a number greater than 1024 and lower than 65535\n";
        cout << "Server setup will exit now.\n Restart the server to start configuration again\n";
        system("pause");
        system("exit");
      }
      else{
        server_port_ = n_port;
      }

    }


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

      bytes_ = recvfrom(socket_cliente_, buffer_, 1024, 0, (SOCKADDR*)&ip_, &size_);
      
      std::string request(buffer_);

      if (bytes_ > 0){
        if (debug_){
          cout << request << endl;
        }
        parseRequest(request);
        memset(buffer_, '\0', 1024);
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
    const char *buf = req.c_str();

    // Get requested resource (Path)
    char* resource_path = getRequestPath(buf);
    // Get extension of that file
    std::string resource_extension(resource_path);
    resource_extension = resource_extension.substr(resource_extension.find_last_of(".") + 1);
    
    //std::cout << " *** REQ *** \n" << req << "\n *** REQ *** " << endl;

    const char *virtualhost;
    virtualhost = strtok((char*)buf, "\n");
    virtualhost = strtok(NULL, " ");

    
    std::string port_str = ":" + std::to_string(getServerPort());
    const char *port_char = port_str.c_str();
    

    virtualhost = strtok(NULL, port_char);
    virtualhost = strtok(NULL, "\r");

    //std::cout << virtualhost << endl;

    sendRequestedContent(resource_path, resource_extension.c_str(), virtualhost);
  }

  void Server::sendRequestedContent(const char *res_name, const char *res_ext, const char *res_domain){

  std:string return_buffer;

    // If GET call is empty, return index
    if (strcmp(res_name, "/") == 0 && strcmp(res_ext, "/") == 0){
      res_name = "/index.html";
      res_ext = "html";
    }

    std::stringstream file_streamstring;
    std::string file_string;

    std::string resource_full_path = base_url;
    resource_full_path += res_name;

    std::ifstream file(resource_full_path, ios::binary);

    if (file.is_open()){
      file_streamstring << file.rdbuf();
      file.close();
      file_string = file_streamstring.str();
      // Start creating request
     return_buffer = "HTTP/1.1 200 - OK\n";
    }
    else{ // File not exist
    return_buffer = "HTTP/1.1 404 - Not Found\n";
    }
    const char* file_src = file_string.c_str();

    std::stringstream file_size_str;
    std::string str;
    file_size_str << file_string.size();
    file_size_str >> str;
    

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
      //return_buffer += "content-disposition: attachment;\n"; // Test this
    }

    if (strcmp(res_ext, "js") == 0){
      return_buffer += "content-type: application/javascript\n";
    }

    if (strcmp(res_ext, "ico") == 0){
      // This breaks the server :/
      // return_buffer += "content-type: image/x-icon\n";
    }
    
    
    

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
  
  void Server::serverStartedWithParams(bool status){
    started_with_port_param_ = status;
  }

  void Server::close(){
    closesocket(socket_);
  }

  
}

