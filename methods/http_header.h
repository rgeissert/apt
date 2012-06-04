
#ifndef APT_HTTP_HEADER_H
#define APT_HTTP_HEADER_H

#include <apt-pkg/strutl.h>

#include <vector>
#include <string>
#include <map>

class HttpHeader {
    std::string HeaderName;
    std::string HeaderValue;

    public:
    HttpHeader();
    HttpHeader(const std::string &Header);
    HttpHeader(const std::string &HeaderName, const std::string &HeaderValue);

    bool empty();
    std::string name();
    std::string value();
    std::vector<HttpHeader> split();
};

class HttpLinkHeader {
    HttpHeader Header;
    std::string URIRef;
    std::map<std::string, std::string> Params;

    public:
    HttpLinkHeader();
    HttpLinkHeader(HttpHeader NewHeader);

    bool empty();
    std::string getURI();
    bool has(std::string Key);
    std::string param(std::string Key);
    size_t paramsCount();
};

#endif
