#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <curl/curl.h>

#define API_ADDR "http://127.0.0.1:8000";

std::size_t callback(const char*, std::size_t, std::size_t, std::string*);
long makeCurlRequest(const char*, std::string*, CURL*, const char*);
#endif // UTILS_H
