#ifndef UTILS_H
#define UTILS_H

#include <sstream>
#include <iostream>
#include <curl/curl.h>
#include <openssl/sha.h>
#include "qmessagebox.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

const std::string API_ADDR = "http://85.160.74.136";

void showMessaggeBox(const char*, const char*, QMessageBox::Icon);
std::size_t callback(const char*, std::size_t, std::size_t, std::string*);
long makeCurlRequest(const char*, std::string*, const char*);
std::string to_hex(unsigned char);
std::string sha256(std::string);
#endif // UTILS_H
