#ifndef FILES_H
#define FILES_H

#include <sys/stat.h>
#include <stdio.h>
#include <curl/curl.h>
#include <string>
#include <userworkspace.h>
#include <iostream>
#include <sys/wait.h>
#include <filesystem>
#include <wchar.h>

#ifdef _WIN32
    const std::string tmpPath = getenv("TMPDIR");
#endif
#ifdef linux
    const std::string tmpPath = "/tmp/";
    #include <unistd.h>
#endif

void cleanUp();
void setUpDirs();
long deleteCurlRequest(UserWorkspace* uw, std::string url, std::string fileName);
long downloadAndOpen(UserWorkspace* uw, std::string url, std::string path, std::string fileName);
long makePostFileCurlRequest(UserWorkspace* uw, const char* url, const char* postData);

#endif // FILES_H
