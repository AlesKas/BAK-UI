#include "utils.h"

std::string API_ADDR;

std::string readConfig(std::string tag) {
    QSettings settings("qt.ini", QSettings::IniFormat);
    settings.beginGroup("API");
    std::string value = settings.value(tag.c_str()).toString().toUtf8().constData();
    settings.endGroup();
    return value;
}

bool isFirstRun() {
    QCoreApplication::setOrganizationName("MySoft");
    QCoreApplication::setOrganizationDomain("mysoft.com");
    QCoreApplication::setApplicationName("Star Runner");
    QSettings settings("qt.ini", QSettings::IniFormat);
    settings.beginGroup("API");
    std::string homeIP = settings.value("homeIP").toString().toUtf8().constData();
    std::string publicIP = settings.value("homeIP").toString().toUtf8().constData();
    settings.endGroup();
    if (homeIP == "" || publicIP == "") {
        return true;
    }
    return false;
}

void createConfig(std::string homeIP, std::string publicIP) {
    std::string path = QDir::currentPath().toUtf8().constData();
    QSettings* settings = new QSettings(QDir::currentPath() + "/qt.ini", QSettings::IniFormat);
    settings->beginGroup("API");
    settings->setValue("homeIP", homeIP.c_str());
    settings->setValue("publicIP", publicIP.c_str());
    settings->setValue("firstRun", false);
    settings->endGroup();
    settings->sync();
}

void initApiAddr() {
    std::string addr1 = readConfig("homeIP");
    std::string addr2 = readConfig("publicIP");
    std::string addr = addr1 + "/apistatus";
    std::string readBuffer;
    long httpCode;
    httpCode = makeCurlRequest("GET", addr.c_str(), &readBuffer, NULL, 1);
    if (httpCode == 200) {
        API_ADDR = addr1;
    } else {
        addr = addr2 + "/apistatus";
        httpCode = makeCurlRequest("GET", addr.c_str(), &readBuffer, NULL, 1);
        bool success = false;
        try {
            json j = json::parse(readBuffer);
            success = j["success"].get<bool>();
        }  catch (std::exception) {
            success = true;
        }
        if (success) {
            API_ADDR = addr2;
        } else {
            QMessageBox::critical(NULL, "Critical error", "Cannot connect to server.");
            exit(-1);
        }
    }
}

std::size_t callback(const char* in, std::size_t size, std::size_t num, std::string* out) {
        const std::size_t totalBytes(size * num);
        out->append(in, totalBytes);
        return totalBytes;
    }

long makeCurlRequest(const char* method, const char* url, std::string *returnData, const char* postData, int timeout) {
    long returnCode;
    returnData->clear();
    CURL *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)returnData);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
    if (postData != NULL) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "charset: utf-8");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
    }

    curl_easy_perform(curl);
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
