#pragma once
#ifndef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#endif // 

#ifndef BOOST_BIND_GLOBAL_PLACEHOLDERS
#define BOOST_BIND_GLOBAL_PLACEHOLDERS //禁用因为版本问题 编译时会出现的警告
#endif // !1

#include <windows.h>
#include <wingdi.h>
#include <tlhelp32.h>
#include <atlstr.h>
#include <winsvc.h> 
#include <shlobj_core.h>
#include <windows.h>
#include <winsock.h>

#include <functional>
#include <string>
#include <ctime>
#include <random>
#include <chrono>
#include <codecvt>
#include <regex>
#include <thread>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/uuid/detail/md5.hpp>

#include "xorstr.h"
#include "typedefs.h"
#include "assert.h"

#include "curl_http.h"
#include "tcp_client.h"
#include "tcp_server.h"


struct OSVersionInfo
{
	DWORD major_ver;
	DWORD minor_ver;
	DWORD build_number;
};

namespace scpp
{

	static inline std::wstring string_to_wstring(const std::string& s)
	{
		using default_convert = std::codecvt<wchar_t, char, std::mbstate_t>;
		static std::wstring_convert<default_convert>conv(new default_convert("CHS"));
		return conv.from_bytes(s);
	}
	static inline std::string wstring_to_string(const std::wstring& s)
	{
		using default_convert = std::codecvt<wchar_t, char, std::mbstate_t>;
		static std::wstring_convert<default_convert>conv(new default_convert("CHS"));
		return conv.to_bytes(s);
	}

	static inline std::string ansi_to_utf8(const std::string& s)
	{
		static std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;
		return conv.to_bytes(string_to_wstring(s));
	}
	static inline std::string utf8_to_ansi(const std::string& s)
	{
		static std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;
		return wstring_to_string(conv.from_bytes(s));
	}

	std::string create_machine_code(std::string card_no);
	bool get_service_state(std::string service_name, uint32_t& service_state);
	bool is_runas_administrator();
	void message(const char* format, ...);
	void message(const wchar_t* format, ...);
	void dbgout(const char* format, ...);
	void dbgout(const wchar_t* format, ...);
	std::string format(const char* format, ...);
	std::wstring format(const wchar_t* format, ...);
	long random(long min, long max);
	void random_sleep(long min, long max);
	bool has_process(const TCHAR* process_name);
	bool has_window(HWND hWnd, long x, long y, long r, long g, long b);
	std::string print_window_rgb(HWND hWnd, long x, long y);
	std::string get_module_path(const char* module_name);
	std::wstring get_module_path(const wchar_t* module_name);
	std::string remaining_to_date(unsigned long long time);
	bool window_is_topmost(HWND hwnd);
	std::string get_current_process_path();
	std::string get_current_process_full_path();
	std::string get_temp_directory();
	std::string create_random_string();
	bool get_os_version_info(OSVersionInfo& os_version);
	bool is_wow64();
	std::string get_desktop_path();
	void string_to_upper(std::string& str);
	template<typename T>
	__forceinline bool between(T value, T min, T max) {
		return value >= min && value <= max;
	}
	template<typename T>
	__forceinline u32 manhattan_distance(const T& p1, const T& p2) {
		auto dx = abs(p1.x - p2.x);
		auto dy = abs(p1.y - p2.y);
		return (u32)sqrt(dx * dx + dy * dy);
	}
	__forceinline u32 create_xy_index(s32 x, s32 y, u32 w = 20000) {
		return y * w + x;
	}
	template<typename T>
	__forceinline u32 create_xy_index(const T& p, u32 w = 20000) {
		return create_xy_index(p.x, p.y, w);
	}
	class RunCost
	{
		std::chrono::steady_clock::time_point start;
	public:
		RunCost() {
			start = std::chrono::steady_clock::now();
		}
		std::string finish(std::string prefix = "finish") {
			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double, std::micro> elapsed = end - start; // std::micro 表示以微秒为时间单位
			return format("%s time: %lf ms", prefix.c_str(), elapsed.count() / 1000);
		}
	};

	template<typename T>
	__forceinline bool in_array(const T& value, std::vector<T> arr) {
		return std::find(arr.begin(), arr.end(), value) != arr.end();
	}

	template<typename T>
	__forceinline bool not_in_array(const T& value, std::vector<T> arr) {
		return std::find(arr.begin(), arr.end(), value) == arr.end();
	}

	template<typename T>
	__forceinline u32 calc_per(const T& min, const T& max) {
		if (min == 0)
			return 0;
		if (max == 0)
			return 0;
		return (u32)(((double)min / (double)max) * 100);
	}

	u64 time(std::string date = "");
	std::string date(u64 time, std::string format = xorstr_("%Y-%m-%d"));

	template<typename T>
	inline void mao_pao_sort(std::vector<T>& list, std::function<bool(T& l, T& r)> cmp) {
		if (list.size() <= 1)
			return;

		auto size = list.size();
		for (size_t i = 0; i < size - 1; i++)
		{
			for (size_t j = 0; j < size - i - 1; j++)
			{
				if (!cmp(list[j], list[j + 1])) {
					std::swap(list[j], list[j + 1]);
				}
			}
		}
	}

	template<typename T>
	inline void mao_pao_sort(std::vector<T>* list, std::function<bool(T& l, T& r)> cmp) {
		if (list->size() <= 1)
			return;

		auto size = list->size();
		for (size_t i = 0; i < size - 1; i++)
		{
			for (size_t j = 0; j < size - i - 1; j++)
			{
				if (!cmp(list->at(j), list->at(j + 1))) {
					std::swap(list->at(j), list->at(j + 1));
				}
			}
		}
	}

	inline std::string match_substr(std::string s, std::string regex_str) {
		std::smatch m;
		std::regex e(regex_str);
		std::string result;
		if (std::regex_search(s, m, e)) {
			result = m[0];
		}
		return result;
	}

	//bool get_intranet_ip(std::string& ip);

	u32 sunday(std::vector<u8> t, std::vector<u8> p);

	class thread {
		DWORD thread_id;
		HANDLE thread_handle;
	public:

		bool thread_switch;

		std::function<void(thread*)> thread_start_address;

		void* thread_context;

		thread();

		~thread();

		bool start(std::function<void(thread*)> start_address, void* context = nullptr);

		void stop(DWORD dwMilliseconds = INFINITE);

		DWORD get_thread_id();

		HANDLE get_thread_handle();
	};

	std::string string_rigth(std::string str, int count);

	std::string string_left(std::string str, int count);

	bool find_drive_list(std::vector<std::string>* drive_list);

	void each_file_path(std::string file_name, std::string file_path, std::string& find_file_path);

	std::string find_file_path(std::string file_name);
};

