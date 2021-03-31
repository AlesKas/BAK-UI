#ifndef FILES_H
#define FILES_H

#include <sys/stat.h>
#include <stdio.h>
#include <curl/curl.h>
#include <string>
#include <userworkspace.h>
#include <iostream>
#include <wchar.h>

#if defined(WIN32) || defined (WIN64)
    const std::string tmpPath = getenv("TMP");
    #include <windows.h>
    #include <dirent.h>
#endif
#ifdef linux
    const std::string tmpPath = "/tmp/";
    #include <unistd.h>
    #include <sys/wait.h>
    #include <filesystem>
#endif

void cleanUp();
void setUpDirs();
long deleteCurlRequest(UserWorkspace* uw, std::string url, std::string fileName, std::string* returnData);
long downloadAndOpen(UserWorkspace* uw, std::string url, std::string path, std::string fileName, std::string* returnData);
long makePostFileCurlRequest(UserWorkspace* uw, const char* url, const char* postData, std::string* returnData);

#endif // FILES_H
