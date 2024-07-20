#include "sdk.h"
#include "baseapi.h"

#define INRANGE(x,a,b)    (x >= a && x <= b)
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

void* base_api::find_sub_start(void * address, int maxsearch)
{
	for (unsigned char * i = static_cast<unsigned char*>(address); maxsearch != 1; i--)
	{
		maxsearch--;
		if (maxsearch == 1)
			break;
		if (*i >> 4 == 0x5 && *(i + 1) == 0x8b && (*(i + 2) & 0xf) == 0xc)
			return i;
	}
	return 0;
}

void* base_api::find_sub_end(void * address, int maxsearch)
{
	for (unsigned char * i = static_cast<unsigned char*>(address); maxsearch != 1; i++)
	{
		maxsearch--;

		if (maxsearch == 1)
			break;

		if (*static_cast<unsigned char*>(i) == 0xC3 || *static_cast<unsigned char*>(i) == 0xC2)
			return i;
	}

	return 0;
}

int base_api::find_sub_size(void * address, int maxsearch)
{
	int start = (int)find_sub_start(address, maxsearch);
	int end = (int)find_sub_end(address, maxsearch);

	if (!start)
		return 0;

	if (!end)
		return 0;

	return end - start;
}

void* base_api::find_string(void * p, const char * string, int len)
{
	if (!p)
		return FALSE;

	char *start, *str = 0;
	int fag = 0;
	int stringlen = strlen(string);

	try
	{
		for (start = static_cast<char*>(p); !str; start++)
		{
			fag++;
			if (len && len != 0 && fag > len)
				return NULL;
			if (memcmp(start, string, stringlen) == 0)
			{
				str = start;
				break;
			}
		}
		for (;; start--)
		{
			if (*(char**)start == str)
				return start;
		}
	}
	catch (...)
	{
		start = 0;
		return FALSE;
	}

	return FALSE;
}

void* base_api::get_interfaces(const char * name, const char * interfacen, const char * ptr_name,
                               create_interface pinterface)
{
	std::string string_interface = "";
	std::string interface_version = "0";

	for (int i = 0; i <= 99; i++)
	{
		string_interface = interfacen;
		string_interface += interface_version;
		string_interface += std::to_string(i);

		void * func_ptr{pinterface(string_interface.c_str(), NULL)};
		if ((DWORD)func_ptr != 0x0)
			return func_ptr;
		if (i >= 99 && interface_version == "0")
		{
			interface_version = "00";
			i = 0;
		}
		else if (i >= 99 && interface_version == "00")
			log_file("%s: 0x%x (error)\n", ptr_name, (DWORD)func_ptr);
	}
	return FALSE;
}

DWORD base_api::find_pattern(DWORD address, DWORD len, const char * pattern)
{
	const char * pat{pattern};
	DWORD firstMatch = NULL;
	for (DWORD pCur = address; pCur < len; pCur++)
	{
		if (!*pat) return firstMatch;
		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat))
		{
			if (!firstMatch) firstMatch = pCur;
			if (!pat[2]) return firstMatch;
			if (*(PWORD)pat == '\?\ ? ' || *(PBYTE)pat != '\ ? ') pat += 3;
				else pat += 2;
		}
		else
		{
			pat = pattern;
			firstMatch = 0;
		}
	}
	return NULL;
}

HMODULE base_api::get_module(const char * name)
{
	HMODULE hmModuleHandle{NULL};
	do
	{
		hmModuleHandle = GetModuleHandle(static_cast<LPCSTR>(name));
		Sleep(1);
	}
	while (hmModuleHandle == NULL);

	return hmModuleHandle;
}

void base_api::log_file(const char * message, ...)
{
	if (!console_init)
	{
		auto is_console_allocated = AllocConsole();
		auto is_console_attached = AttachConsole(GetCurrentProcessId());

		freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
		freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);

		SetConsoleTitleA("yocto-base console");

		console_init = is_console_allocated && is_console_attached;
	}
	va_list va_alist;
	static char szLogbuf[4096];
	FILE * fp;
	struct tm * current_tm;
	time_t current_time;

	time(&current_time);
	current_tm = localtime(&current_time);

	sprintf(szLogbuf, "[YOCTO] [%02d:%02d:%02d] ", current_tm->tm_hour, current_tm->tm_min, current_tm->tm_sec);

	va_start(va_alist, message);
	_vsnprintf(szLogbuf + strlen(szLogbuf), sizeof(szLogbuf) - strlen(szLogbuf), message, va_alist);
	va_end(va_alist);
	sprintf(szLogbuf, "%s\n", szLogbuf);

	if ((fp = fopen("yocto.log", "a")) != NULL)
	{
		fprintf(fp, "%s", szLogbuf);
		fclose(fp);
	}

	printf(szLogbuf);
}

void base_api::clear_console()
{
	if (!console_init)
		return;

	fclose(stdout);
	fclose(stdin);

	FreeConsole();
	PostMessageW(GetConsoleWindow(), WM_CLOSE, 0, 0);
}

DWORD base_api::get_client_sig(char * pattern)
{
	static HMODULE hmModule{get_module("client.dll")};
	static PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hmModule;
	static PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)(((DWORD)hmModule) + pDOSHeader->e_lfanew);
	return find_pattern(((DWORD)hmModule) + pNTHeaders->OptionalHeader.BaseOfCode,
	                    ((DWORD)hmModule) + pNTHeaders->OptionalHeader.SizeOfCode, pattern);
	return 0;
}

DWORD base_api::get_sig(char * mod, char * pattern)
{
	static HMODULE hmModule{get_module(mod)};
	static PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hmModule;
	static PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)(((DWORD)hmModule) + pDOSHeader->e_lfanew);
	return find_pattern(((DWORD)hmModule) + pNTHeaders->OptionalHeader.BaseOfCode,
	                    ((DWORD)hmModule) + pNTHeaders->OptionalHeader.SizeOfCode, pattern);
	return 0;
}

DWORD base_api::get_engine_sig(char * pattern)
{
	static HMODULE hmModule{get_module("engine.dll")};
	static PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hmModule;
	static PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)(((DWORD)hmModule) + pDOSHeader->e_lfanew);
	return find_pattern(((DWORD)hmModule) + pNTHeaders->OptionalHeader.BaseOfCode,
	                    ((DWORD)hmModule) + pNTHeaders->OptionalHeader.SizeOfCode, pattern);
	return 0;
}

void base_api::error_box(const char * error)
{
	std::string s{error};
	int len;
	int slength = static_cast<int>(s.length()) + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t * buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	MessageBoxW(NULL, r.c_str(), 0, 0);
}

DWORD base_api::get_call_func(DWORD instrptr)
{
	DWORD jumpMask = instrptr + 1;
	return ((*(PDWORD)(jumpMask)) + jumpMask + 4);
}

base_api api;
