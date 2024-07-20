#pragma once

#include "sdk.h"

;

class base_api
{
public:
	bool console_init = false;

	void* find_sub_start(void * address, int maxsearch);
	void* find_sub_end(void * address, int maxsearch);
	int find_sub_size(void * address, int maxsearch);
	void* find_string(void * p, const char * string, int len);
	void* get_interfaces(const char * name, const char * interfacen, const char * ptr_name,
	                     create_interface pinterface);
	DWORD find_pattern(DWORD address, DWORD len, const char * pattern);
	void log_file(const char * message, ...);

	void clear_console();

	HMODULE get_module(const char * name);
	DWORD get_client_sig(char * pattern);
	DWORD get_sig(char * mod, char * pattern);
	DWORD get_engine_sig(char * pattern);
	DWORD get_call_func(DWORD instrptr);

	void error_box(const char * error);
};

extern base_api api;
