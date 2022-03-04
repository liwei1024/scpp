#include "scpp.h"

#define BST (+1)
#define CCT (+8)


using namespace boost::property_tree;

using namespace std;

namespace scpp {


	std::string utf8_to_ansi(std::string utf8_string)
	{
		BSTR    bstr;
		char* ansi;
		int     len;
		const char* buf = utf8_string.c_str();

		len = ::MultiByteToWideChar(CP_UTF8, 0, buf, (int)strlen(buf) + 1, NULL, NULL);
		bstr = ::SysAllocStringLen(NULL, len);

		::MultiByteToWideChar(CP_UTF8, 0, buf, (int)strlen(buf) + 1, bstr, len);

		len = ::WideCharToMultiByte(CP_ACP, 0, bstr, -1, NULL, 0, NULL, NULL);
		ansi = new char[len];

		::WideCharToMultiByte(CP_ACP, 0, bstr, -1, ansi, len, NULL, NULL);
		::SysFreeString(bstr);

		std::string r(ansi);
		delete[] ansi;
		return r;
	}


	std::string create_machine_code(std::string card_no)
	{
		DWORD  serial_num = 0;//序列号  
		if (::GetVolumeInformation(xorstr_(L"c:\\"),
			nullptr,
			0,
			&serial_num,
			nullptr,
			nullptr,
			nullptr,
			0) == FALSE || serial_num == 0)
		{
			return "";
		}

		std::string serial_num_string = std::to_string(serial_num) + card_no;

		boost::uuids::detail::md5 boost_md5;
		boost_md5.process_bytes(serial_num_string.c_str(), serial_num_string.size());
		boost::uuids::detail::md5::digest_type digest;
		boost_md5.get_digest(digest);
		const auto char_digest = reinterpret_cast<const char*>(&digest);
		serial_num_string.clear();
		boost::algorithm::hex(char_digest, char_digest + sizeof(boost::uuids::detail::md5::digest_type), std::back_inserter(serial_num_string));

		string_to_upper(serial_num_string);

		return serial_num_string;

	}


	bool get_service_state(std::string service_name, uint32_t& service_state)
	{
		SC_HANDLE hServiceMgr = NULL;//
		SC_HANDLE hServiceDDK = NULL;//


		service_state = -1;

		hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (hServiceMgr == NULL)
		{
			return false;
		}

		hServiceDDK = OpenServiceA(hServiceMgr, service_name.c_str(), SERVICE_ALL_ACCESS);
		CloseServiceHandle(hServiceMgr);
		if (hServiceDDK != NULL)
		{
			SERVICE_STATUS ss;
			if (QueryServiceStatus(hServiceDDK, &ss))
			{
				service_state = ss.dwCurrentState;
			}

			CloseServiceHandle(hServiceDDK);
		}
		return true;
	}


	bool is_runas_administrator()
	{
		bool bElevated = false;
		HANDLE hToken = NULL;

		// Get current process token
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
			return false;

		TOKEN_ELEVATION tokenEle;
		DWORD dwRetLen = 0;

		// Retrieve token elevation information
		if (GetTokenInformation(hToken, TokenElevation, &tokenEle, sizeof(tokenEle), &dwRetLen))
		{
			if (dwRetLen == sizeof(tokenEle))
			{
				bElevated = tokenEle.TokenIsElevated;
			}
		}

		CloseHandle(hToken);
		return bElevated;
	}

	void message(const char* format, ...)
	{
		va_list argList;
		va_start(argList, format);
		size_t length = size_t(_vscprintf(format, argList)) + 1;//必须加1 否则会蹦
		char* buffer = new char[length];
		vsprintf_s(buffer, length, format, argList);
		::MessageBoxA(NULL, buffer, "", MB_OK);
		va_end(argList);
		delete[]buffer;
	}

	void message(const wchar_t* format, ...)
	{
		va_list argList;
		va_start(argList, format);
		size_t length = size_t(_vscwprintf(format, argList)) + 1;//必须加1 否则会蹦
		wchar_t* buffer = new wchar_t[length];
		vswprintf_s(buffer, length, format, argList);
		::MessageBoxW(NULL, buffer, L"", MB_OK);
		va_end(argList);
		delete[]buffer;
	}

	void dbgout(const char* format, ...)
	{
		va_list argList;
		va_start(argList, format);
		size_t length = size_t(_vscprintf(format, argList)) + 1;//必须加1 否则会蹦
		char* buffer = new char[length];
		vsprintf_s(buffer, length, format, argList);
		::OutputDebugStringA(buffer);
		va_end(argList);
		delete[]buffer;
	}

	void dbgout(const wchar_t* format, ...)
	{
		va_list argList;
		va_start(argList, format);
		size_t length = size_t(_vscwprintf(format, argList)) + 1;//必须加1 否则会蹦
		wchar_t* buffer = new wchar_t[length];
		vswprintf_s(buffer, length, format, argList);
		::OutputDebugStringW(buffer);
		va_end(argList);
		delete[]buffer;
	}

	std::string format(const char* format, ...)
	{
		char* buffer = nullptr;
		std::string str = "";
		if (nullptr != format)
		{
			va_list marker = nullptr;
			va_start(marker, format); //初始化变量参数
			size_t length = _vscprintf(format, marker) + 1; //获取格式化字符串长度
			buffer = new char[length];
			memset(buffer, '\0', length);
			_vsnprintf_s(buffer, length, length, format, marker);
			str = buffer;
			delete[]buffer;
			va_end(marker); //重置变量参数
		}
		return str;
	}

	std::wstring format(const wchar_t* format, ...)
	{
		wchar_t* buffer = nullptr;
		std::wstring str = L"";
		if (nullptr != format)
		{
			va_list marker = nullptr;
			va_start(marker, format); //初始化变量参数
			size_t length = _vscwprintf(format, marker) + 1; //获取格式化字符串长度
			buffer = new wchar_t[length];
			memset(buffer, L'\0', length);
			_vsnwprintf_s(buffer, length, length, format, marker);
			str = buffer;
			delete[]buffer;
			va_end(marker); //重置变量参数
		}
		return str;
	}

	long random(long min, long max)
	{
		std::default_random_engine e;    // 生成无符号随机整数
		std::uniform_int_distribution<long> u(min, max);
		std::chrono::system_clock::time_point time_point_now = std::chrono::system_clock::now(); // 获取当前时间点
		std::chrono::system_clock::duration duration_since_epoch = time_point_now.time_since_epoch(); // 从1970-01-01 00:00:00到当前时间点的时长
		time_t microseconds_since_epoch = std::chrono::duration_cast<std::chrono::microseconds>(duration_since_epoch).count(); // 将时长转换为微秒数
		e.seed((uint32_t)microseconds_since_epoch);
		return u(e);
	}

	void random_sleep(long min, long max)
	{
		::Sleep(random(min, max));
	}


	bool has_process(const TCHAR* process_name)
	{
		PROCESSENTRY32W pe32;
		pe32.dwSize = sizeof(pe32);
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		BOOL bMore = ::Process32First(hProcessSnap, &pe32);
		while (bMore)
		{
			if (_tcsicmp(pe32.szExeFile, process_name) == 0) {
				return true;
			}
			bMore = ::Process32Next(hProcessSnap, &pe32);
		}
		::CloseHandle(hProcessSnap);
		return false;
	}

	bool has_window(HWND hWnd, long x, long y, long r, long g, long b)
	{
		HDC hDc = ::GetWindowDC(hWnd);
		if (hDc == NULL)
			return false;

		COLORREF color = ::GetPixel(hDc, x, y);
		if (color <= 0)
			return false;
		if (
			GetRValue(color) != r ||
			GetGValue(color) != g ||
			GetBValue(color) != b
			)
			return false;
		return true;
	}

	std::string print_window_rgb(HWND hWnd, long x, long y)
	{
		HDC hDc = ::GetWindowDC(hWnd);
		if (hDc == NULL)
			return "";

		COLORREF color = ::GetPixel(hDc, x, y);
		if (color <= 0)
			return "";

		return format(xorstr_("xy = (%d,%d) | rgb = (0x%x,0x%x,0x%x)"), x, y, GetRValue(color), GetGValue(color), GetBValue(color));

	}

	std::string get_module_path(const char* module_name)
	{
		char file_path[MAX_PATH] = {};
		::GetModuleFileNameA(GetModuleHandleA(module_name), file_path, MAX_PATH);
		return std::string(file_path);
	}

	std::wstring get_module_path(const wchar_t* module_name)
	{
		wchar_t file_path[MAX_PATH] = {};
		::GetModuleFileNameW(GetModuleHandleW(module_name), file_path, MAX_PATH);
		return std::wstring(file_path);
	}

	std::string remaining_to_date(unsigned long long time)
	{
		unsigned long long day = 0;
		unsigned long long hour = 0;
		unsigned long long minute = 0;
		unsigned long long second = 0;
		day = time / 60 / 60 / 24;
		hour = (time / 60 / 60) % 24;
		minute = (time / 60) % 60;
		second = time % 60;
		return format(xorstr_("剩余时间 %ld天%ld时%ld分%ld秒"), day, hour, minute, second);
	}

	bool window_is_topmost(HWND hwnd)
	{
		if (GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST)
			return true;
		return false;
	}

	std::string get_current_process_path() {

		TCHAR path_buff[MAX_PATH] = { 0 };
		GetModuleFileNameW(NULL, path_buff, MAX_PATH);
		std::string root_path(CW2A(path_buff).m_psz);
		return root_path.substr(0, root_path.find_last_of("\\", root_path.size()));
	}

	std::string get_current_process_full_path() {

		char file_path[MAX_PATH] = {};
		::GetModuleFileNameA(GetModuleHandleW(NULL), file_path, MAX_PATH);
		return std::string(file_path);
	}

	std::string get_temp_directory() {
		char temp_directory[MAX_PATH] = { 0 };
		const uint32_t get_temp_path_ret = GetTempPathA(sizeof(temp_directory), temp_directory);
		if (!get_temp_path_ret || get_temp_path_ret > MAX_PATH)
		{
			return "";
		}
		return std::string(temp_directory);
	}

	std::string create_random_string() {

		char ranodm_string[MAX_PATH] = {};

		srand((unsigned)time(NULL) * GetCurrentThreadId());

		//Randomize name for log in registry keys, usn jornal and other shits
		memset(ranodm_string, 0, sizeof(ranodm_string));
		static const char alphanum[] =
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		int len = rand() % 20 + 10;
		for (int i = 0; i < len; ++i)
			ranodm_string[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

		return std::string(ranodm_string);
	}

	bool get_os_version_info(OSVersionInfo& os_version) {

		bool ret = false;
		HMODULE mod_ntdll = NULL;
		if (mod_ntdll = ::LoadLibraryW(L"ntdll.dll"))
		{
			typedef void (WINAPI* pfRTLGETNTVERSIONNUMBERS)(DWORD*, DWORD*, DWORD*);
			pfRTLGETNTVERSIONNUMBERS pfRtlGetNtVersionNumbers;
			pfRtlGetNtVersionNumbers = (pfRTLGETNTVERSIONNUMBERS)::GetProcAddress(mod_ntdll, "RtlGetNtVersionNumbers");
			if (pfRtlGetNtVersionNumbers)
			{
				pfRtlGetNtVersionNumbers(&os_version.major_ver, &os_version.minor_ver, &os_version.build_number);
				os_version.build_number &= 0x0ffff;
				ret = TRUE;
			}
			::FreeLibrary(mod_ntdll);
			mod_ntdll = NULL;
		}
		return ret;
	}

	/*
	wow64 是表示32位系统
	*/
	bool is_wow64()
	{
		typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
		LPFN_ISWOW64PROCESS fnIsWow64Process;
		BOOL ret = false;
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32")), "IsWow64Process");
		if (NULL != fnIsWow64Process)
			fnIsWow64Process(GetCurrentProcess(), &ret);
		return (bool)ret;
	}


	std::string get_desktop_path() {
		WCHAR path[255];
		//获取当前用户的桌面路径
		SHGetSpecialFolderPath(0, path, CSIDL_DESKTOPDIRECTORY, 0);
		return std::string(CW2A(path).m_psz);
	}

	void string_to_upper(std::string& str) {
		for (size_t i = 0, len = str.size(); i < len; ++i)
			str[i] = std::toupper(str[i]);
	}

	u64 time(std::string date)
	{
		time_t t = ::time(NULL);
		return u64(t) + 8 * 3600;
	}

	std::string date(u64 time, std::string format)
	{
		time_t t;
		tm p;
		t = (time_t)time;
		errno_t err_no = gmtime_s(&p, &t);
		char buffer[80]{ 0 };
		strftime(buffer, sizeof(buffer), format.c_str(), &p);
		return std::string(buffer);
	}




	/*bool get_intranet_ip(std::string & ip) {
		WSADATA wsaData;
		char name[155];
		hostent *hostinfo;
		if (::WSAStartup(MAKEWORD(2, 0), &wsaData) == 0)
		{
			if (::gethostname(name, sizeof(name)) == 0)
			{
				hostinfo = ::gethostbyname(name);
				if (hostinfo != NULL)
				{
					ip = std::string(inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list));
					return true;
				}
			}
			::WSACleanup();
		}

		return false;
	}*/

	u32 sunday(std::vector<u8> t, std::vector<u8> p)
	{
		//256 表示 0x00~0xff
		static u32 max_byte_size = 256;
		static u32 shift[256] = { 0 };
		auto n = (u32)t.size();
		auto m = (u32)p.size();
		// 默认值，移动m+1位 256 表示 0x00~0xff
		for (u32 i = 0; i < max_byte_size; i++) {
			shift[i] = m + 1;
		}
		// 模式串P中每个字母出现的最后的下标
		// 所对应的主串参与匹配的最末位字符的下一位字符移动到该位，所需要的移动位数
		for (u32 i = 0; i < m; i++) {
			shift[p[i]] = m - i;
		}
		// 模式串开始位置在主串的哪里
		u32 s = 0;
		// 模式串已经匹配到的位置
		u32 j;
		while (s < n - m) {
			j = 0;
			while (t[s + j] == p[j]) {
				j++;
				// 匹配成功
				if (j >= m) {
					return s;
				}
			}
			// 找到主串中当前跟模式串匹配的最末字符的下一个字符
			// 在模式串中出现最后的位置
			// 所需要从(模式串末尾+1)移动到该位置的步数
			s += shift[t[s + m]];
		}
		return -1;
	}

	thread::thread()
	{
		thread_switch = true;

	}
	thread::~thread() {
		if (thread_handle)
		{
			CloseHandle(thread_handle);
			thread_handle = NULL;
		}
	}

	bool thread::start(std::function<void(thread*)> start_address, void* context) {
		thread_start_address = start_address;
		thread_context = context;
		thread_switch = true;
		thread_handle = ::CreateThread(
			NULL,
			0,
			[](LPVOID param)->DWORD {
				thread* t = (thread*)param;
				t->thread_start_address(t);
				return 0;
			},
			this,
			0,
			&thread_id
		);
		return thread_handle != NULL;
	}

	void thread::stop(DWORD dwMilliseconds) {
		if (!thread_handle)
			return;

		this->thread_switch = false;

		WaitForSingleObject(thread_handle, dwMilliseconds);

	}

	DWORD thread::get_thread_id() {
		return thread_id;
	}
	HANDLE thread::get_thread_handle() {
		return thread_handle;
	}

	std::string get_file_name(std::string file) {
		std::string root_path(file);
		auto last_of = root_path.find_last_of("\\", root_path.size());
		if (last_of == -1)
		{
			return file;
		}
		return root_path.substr(last_of, root_path.size());
	}


	std::string string_rigth(std::string str, int count) {
		return str.substr(str.size() - count);
	}

	std::string string_left(std::string str, int count) {
		return str.substr(0, count);
	}

	bool find_drive_list(std::vector<std::string>* drive_list) {
		char  drive_name[] = { "C:" };
		while (strcmp(&drive_name[0], "Z:") != 0)
		{
			switch (GetDriveTypeA(drive_name))
			{
			case 3: {
				drive_list->push_back(drive_name);
				break;
			}
			default:
				break;
			}
			drive_name[0] += 1;
		}
		return !drive_list->empty();
	}

	void each_file_path(std::string file_name, std::string file_path, std::string& find_file_path) {

		if (!find_file_path.empty())
			return;

		if (string_rigth(file_name, 1) != "\\")
			file_path += "\\";

		WIN32_FIND_DATAA FindFileData = {};

		HANDLE hFind = FindFirstFileA(format("%s*.*", file_path.c_str()).c_str(), &FindFileData);

		if (hFind == INVALID_HANDLE_VALUE)
			return;
		do
		{
			if (!find_file_path.empty())
				break;

			if (FindFileData.cFileName[0] == '.')//过滤.和..
				continue;

			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
				continue;

			std::string name = get_file_name(FindFileData.cFileName);

			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				each_file_path(file_name, format("%s%s", file_path.c_str(), name.c_str()).c_str(), find_file_path);
			}
			else if (name == file_name) {
				find_file_path = file_path;
				break;
			}
		} while (FindNextFileA(hFind, &FindFileData));

		FindClose(hFind);
	}

	std::string find_file_path(std::string file_name) {
		std::string find_file_path;

		std::vector<std::string> drive_list = {};

		if (find_drive_list(&drive_list)) {

			for (auto& drive : drive_list) {
				std::thread t(
					[&]()->void {
						each_file_path(file_name, drive.c_str(), find_file_path);
					});
				t.detach();
			}

			while (find_file_path.empty())
				Sleep(10);
		}

		return find_file_path;
	}
}