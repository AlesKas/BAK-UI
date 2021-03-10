#include "utils.h"

std::string API_ADDR;

void initApiAddr() {
    std::string addr1 = "http://192.168.0.10:8000";
    std::string addr2 = "http://85.160.74.136";
    std::string addr = addr1 + "/apistatus";
    std::string readBuffer;
    long httpCode;
    httpCode = makeCurlRequest(addr.c_str(), &readBuffer, NULL, 1);
    if (httpCode != 200) {
        addr = addr2 + "/apistatus";
        httpCode = makeCurlRequest(addr.c_str(), &readBuffer, NULL, 1);
        if (httpCode != 200) {
            showMessaggeBox("Cannot connect to server.", "Critical error", QMessageBox::Critical);
            exit(-1);
        } else {
            API_ADDR = addr2;
        }
    } else {
        API_ADDR = addr1;
    }

}

std::size_t callback(const char* in, std::size_t size, std::size_t num, std::string* out) {
        const std::size_t totalBytes(size * num);
        out->append(in, totalBytes);
        return totalBytes;
    }

long makeCurlRequest(const char* url, std::string *returnData, const char* postData, int timeout) {
    long returnCode;
    returnData->clear();
    CURL *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url);

    // Don't bother trying IPv6, which would increase DNS resolution time.
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

    // Don't wait forever, time out after 5 seconds.
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

    // Hook up data container (will be passed as the last parameter to the
    // callback handling function).  Can be any pointer type, since it will
    // internally be passed as a void pointer.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)returnData);
    if (postData != NULL) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "charset: utf-8");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
    }

    curl_easy_perform(curl);
    // Run our HTTP GET command, capture the HTTP response code, and clean up.
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &returnCode);
    curl_easy_cleanup(curl);

    return returnCode;
}

std::string to_hex(unsigned char ch) {
    std::stringstream ss;
    ss << std::hex << (int) ch;
    return ss.str();
}


std::string sha256(std::string line) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, line.c_str(), line.length());
    SHA256_Final(hash, &sha256);

    std::string output = "";
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        output += to_hex(hash[i]);
    }
    return output;
}

void showMessaggeBox(const char* message, const char* title, QMessageBox::Icon icon) {
    QMessageBox box;
    box.setWindowTitle(title);
    box.setIcon(icon);
    box.setText(message);
    box.exec();
}

std::string splitStringByLength(std::string input) {
    for (int i = 12; i < input.length(); i+=12) {
        input.insert(i, "\n");
    }
    return input;
}

std::string getIcon(std::string fileType) {
    if (iconPaths.find(fileType) != iconPaths.end()) {
        return iconPaths.at(fileType);
    } else {
        return ":/icons/img/document.png";
    }
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

    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, stats.st_size);
    curl_easy_setopt (curl, CURLOPT_VERBOSE, 0L);
    uw->setCursor(Qt::BusyCursor);
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &returnCode);

    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all(headerlist);

    uw->setCursor(Qt::ArrowCursor);
    return returnCode;
}
