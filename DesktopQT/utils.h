#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <curl/curl.h>

std::size_t callback(const char*, std::size_t, std::size_t, std::string*);
long makeCurlRequest(const char*, std::string*, CURL*, const char*);
#endif // UTILS_H
