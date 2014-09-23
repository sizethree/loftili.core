#ifndef _LOFTILI_REQUEST_H
#define _LOFTILI_REQUEST_H

#include <string>
#include <iostream>
#include <sstream>
#include "loftili.h"
#include "communication/response.h"
#include "communication/json.h"

namespace loftili {

class Request {

  public:
    Request();
    Request(ahc_info info);
    Request(std::string url, std::string method);
    ~Request();
    static size_t receiver(char* ptr, size_t size, size_t nmemb, void* userdata);
    void send(Response* res);
    void send(Json* doc, Response* res);
    char* query(std::string key);

  public:
    std::string url;
    std::string method;
    int c_method;
    CURL* curl;
    MHD_Connection* connection;

  private:
    bool writing;

};

}

#endif
