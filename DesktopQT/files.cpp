#include "files.h"

std::string APP_DIRECTORY;

void setUpDirs() {

    std::string tmp = tmpPath + std::string("BAK.XXXXXX");
    char *tmpChar = &tmp[0];
    char *tmp_dirname = mkdtemp(tmpChar);
    if (tmp_dirname != NULL) {
        APP_DIRECTORY = tmp_dirname;
    }
}

void cleanUp() {
    std::uintmax_t n = std::filesystem::remove_all(APP_DIRECTORY);
}

long makePostFileCurlRequest(UserWorkspace* uw, const char* url, const char* postData) {
    long returnCode;
    CURL *curl;
    struct stat stats;
    stat(postData, &stats);

    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    struct curl_slist *headerlist = NULL;
    static const char buf[] =  "Expect:";

    curl_global_init(CURL_GLOBAL_ALL);

    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "fileName",
                 CURLFORM_FILE, postData,
                 CURLFORM_END);

    curl = curl_easy_init();

    headerlist = curl_slist_append(headerlist, buf);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, stats.st_size);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    uw->setCursor(Qt::BusyCursor);
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &returnCode);

    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all(headerlist);

    uw->setCursor(Qt::ArrowCursor);
    return returnCode;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

long downloadAndOpen(UserWorkspace* uw, std::string url, std::string path, std::string fileName) {
    long returnCode = 200;
    CURL *curl;
    FILE *fp;
    CURLcode res;
    std::string filePathName = path + fileName;

    struct stat buffer;
    std::replace(filePathName.begin(), filePathName.end(), '/', '-');
    std::string output = APP_DIRECTORY + "/" + filePathName;
    bool fileExists = stat((APP_DIRECTORY+"/"+filePathName).c_str(), &buffer) == 0;

    if (!fileExists) {
        char outfilename[FILENAME_MAX];
        strcpy(outfilename, output.c_str());
        curl = curl_easy_init();
        fp = fopen(outfilename, "wb");
        auto escaped = curl_easy_escape(curl, fileName.c_str(), 0);
        url += escaped;

        curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        uw->setCursor(Qt::BusyCursor);
        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &returnCode);
        curl_easy_cleanup(curl);
        fclose(fp);
    }
    if (res == CURLE_OK || fileExists) {
        pid_t pid = fork();
        if (pid == 0) {
            execl("/usr/bin/xdg-open", "xdg-open", output.c_str(), (char *)0);
        }
        uw->setCursor(Qt::ArrowCursor);
    }
    return returnCode;
}

long deleteCurlRequest(UserWorkspace* uw, std::string url, std::string fileName) {
    long returnCode;
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    auto escaped = curl_easy_escape(curl, fileName.c_str(), 0);
    url += escaped;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    uw->setCursor(Qt::BusyCursor);
    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &returnCode);
    curl_easy_cleanup(curl);
    uw->setCursor(Qt::ArrowCursor);
    return returnCode;
}
