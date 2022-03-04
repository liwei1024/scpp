#pragma once
#include <curl/curl.h>
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "Crypt32.lib")
#pragma comment (lib, "Wldap32.lib")

namespace scpp {
	class CurlHttp
	{
		CURL* curl_ptr;

		struct curl_slist* headers;

		std::string url;
	public:


		CurlHttp();
		~CurlHttp();

		inline void set_url(std::string value) {
			url = value;
		}

		template<typename T>
		inline void set_option(CURLoption option, T value) {
			curl_easy_setopt(curl_ptr, option, value);
		}

		inline void set_header(std::string value) {
			curl_slist_append(headers, value.c_str());
		}

		CURLcode get(std::string& response, int timeout = 30);

		CURLcode post(std::string& data, std::string& response, int timeout = 30);

		CURLcode download_file(const char outfilename[FILENAME_MAX]);
	};

}