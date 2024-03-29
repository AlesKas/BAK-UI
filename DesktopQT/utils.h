#ifndef UTILS_H
#define UTILS_H

#include <sstream>
#include <iostream>
#include <curl/curl.h>
#include <openssl/sha.h>
#include <QMessageBox>
#include <nlohmann/json.hpp>
#include <QSettings>
#include <QApplication>
#include <fstream>
#include <QDir>

using json = nlohmann::json;

const std::map<std::string, std::string> iconPaths {
    {"png", ":/icons/img/image.png"},
    {"jpg", ":/icons/img/image.png"},
    {"jpeg", ":/icons/img/image.png"},
    {"jpx", ":/icons/img/image.png"},
    {"gif", ":/icons/img/image.png"},
    {"mp4", ":/icons/img/video-player.png"},
    {"m4v", ":/icons/img/video-player.png"},
    {"mkv", ":/icons/img/video-player.png"},
    {"webm", ":/icons/img/video-player.png"},
    {"mov", ":/icons/img/video-player.png"},
    {"avi", ":/icons/img/video-player.png"},
};

void initApiAddr();
bool isFirstRun();
void createConfig(std::string homeIP, std::string publicIP);
std::string to_hex(unsigned char ch);
std::string sha256(std::string line);
std::string getIcon(std::string fileType);
std::string splitStringByLength(std::string input);
void showMessaggeBox(const char* message, const char* title, QMessageBox::Icon icon);
std::size_t callback(const char* in, std::size_t size, std::size_t num, std::string* out);
long makeCurlRequest(const char* method, const char* url, std::string* returnData, const char* postData, int timeout);
#endif // UTILS_H
