#include "scpp.h"



#define DefaultUserAgent xorstr_("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36")


using namespace std;



static size_t receive_data(void* contents, size_t size, size_t nmemb, void* stream) {
    string* str = (string*)stream;
    (*str).append((char*)contents, size * nmemb);
    return size * nmemb;
}

static size_t writedata2file(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

namespace scpp {
    CurlHttp::CurlHttp()
    {
        curl_global_init(CURL_GLOBAL_ALL);

        headers = NULL;
        headers = curl_slist_append(headers, DefaultUserAgent);

        curl_ptr = curl_easy_init();
        curl_easy_setopt(curl_ptr, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl_ptr, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl_ptr, CURLOPT_HTTPHEADER, headers);

        url = "";

    }

    CurlHttp::~CurlHttp()
    {
        curl_easy_cleanup(curl_ptr);
    }

    CURLcode CurlHttp::get(std::string& response, int timeout)
    {
        curl_easy_setopt(curl_ptr, CURLOPT_URL, url.c_str());
        //curl_easy_setopt(curl_ptr, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl_ptr, CURLOPT_TIMEOUT, timeout);
        curl_easy_setopt(curl_ptr, CURLOPT_WRITEFUNCTION, receive_data);
        curl_easy_setopt(curl_ptr, CURLOPT_WRITEDATA, (void*)&response);

        return curl_easy_perform(curl_ptr);;
    }

    CURLcode CurlHttp::post(std::string& data, std::string& response, int timeout)
    {
        curl_easy_setopt(curl_ptr, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_ptr, CURLOPT_POST, 1L);
        curl_easy_setopt(curl_ptr, CURLOPT_POSTFIELDS, data.data());
        curl_easy_setopt(curl_ptr, CURLOPT_TIMEOUT, timeout);
        curl_easy_setopt(curl_ptr, CURLOPT_WRITEFUNCTION, receive_data);
        curl_easy_setopt(curl_ptr, CURLOPT_WRITEDATA, (void*)&response);

        return curl_easy_perform(curl_ptr);;
    }

    CURLcode CurlHttp::download_file(const char outfilename[FILENAME_MAX]) {

        FILE* fp;
        if (fopen_s(&fp, outfilename, "wb"))
            return CURLE_REMOTE_FILE_EXISTS;

        curl_easy_setopt(curl_ptr, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_ptr, CURLOPT_WRITEFUNCTION, writedata2file);
        curl_easy_setopt(curl_ptr, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl_ptr, CURLOPT_NOPROGRESS, FALSE);

        auto res = curl_easy_perform(curl_ptr);
        fclose(fp);
        return res;
    }
}