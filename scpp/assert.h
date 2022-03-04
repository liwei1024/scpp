#pragma once
#include <windows.h>
#define JM_XORSTR_DISABLE_AVX_INTRINSICS // ��Ӧ�Ͱ汾cpu
#include "xorstr.h"


static inline void assert_string(const char * format, ...) {
	if (format != nullptr)
	{
		va_list arg_list;
		va_start(arg_list, format);
		size_t length = (size_t)_vscprintf(format, arg_list) + 1;//�����1 ������
		if (length > 1)
		{
			char* buffer = new char[length];
			vsprintf_s(buffer, length, format, arg_list);
			::MessageBoxA(NULL, buffer, xorstr_("ϵͳ����"), MB_OK);
			delete[]buffer;
		}
		va_end(arg_list);

		exit(0);
	}
}

static inline void assert_string(const wchar_t* format, ...) {
	if (format != nullptr) {
		va_list arg_list = va_list();
		va_start(arg_list, format);
		size_t length = (size_t)_vscwprintf(format, arg_list) + 1;//�����1 ������
		if (length > 1)
		{
			wchar_t* buffer = new wchar_t[length];
			vswprintf_s(buffer, length, format, arg_list);
			::MessageBoxW(NULL, buffer, xorstr_(L"ϵͳ����"), MB_OK);
			delete[]buffer;
		}
		va_end(arg_list);
		exit(0);
	}
}

#undef assert_
#define assert_(expression) if(!(expression)){ assert_string(xorstr_(L"������� %s �����ļ� %s �����к� %d"),xorstr_(_CRT_WIDE(#expression)),xorstr_(_CRT_WIDE(__FILE__)), (unsigned)(__LINE__)); }

