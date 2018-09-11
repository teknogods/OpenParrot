#include <Windows.h>

typedef signed(__stdcall *dll_entry_main)();
typedef signed(__stdcall *dll_entry_init)();

// LibAVS
typedef int(__cdecl *property_create)(int a1, char *a2, int a3);

typedef int(__cdecl *property_node_create)(int a1, int a2, int a3, const char*a4, const char*a5);
typedef int(__cdecl *property_node_create2)(int a1, int a2, int a3, const char*a4, DWORD a5);

typedef int(__cdecl *property_node_datasize)(int a1);

typedef int(__cdecl *property_search)(int a1, int a2, const char *a3);

typedef int(__cdecl *property_get_error)(int a1);

typedef int(__cdecl *avs_boot)(int a1, void *a2, int a3, void *a4, int a5, void *a6, int a7);

typedef unsigned int(__cdecl *log_body_misc)(const char *a1, int a2);

typedef int(__cdecl *std_getenv)(const char*a1, int a2, int a3);

// libAVS ea3
typedef int(__cdecl *ea3_boot)(int a1);
typedef int(__stdcall *ea3_get_sic)();

property_create property_create_func_p;
property_node_create property_node_create_func_p;
property_node_create2 property_node_create_func_p2;
property_node_datasize property_node_datasize_func_p;
property_search property_search_func_p;
property_get_error property_get_error_func_p;
avs_boot avs_boot_func_p;
log_body_misc log_body_misc_func_p;
std_getenv std_getenv_func_p;

// libavs ea3
ea3_boot ea3_boot_func_p;
ea3_get_sic ea3_get_sic_func_p;

void *avsMemRegion1 = 0;
void *avsMemRegion2 = 0;

int ReturnsFalse()
{
	return 0;
}

int eaBootExt(int a1)
{
	int v1; // eax
	int v2; // eax
	int v3; // eax
	int v4; // eax
	int v5; // eax
	int v6; // eax
	int v7; // eax
	int v8; // eax
	int v9; // eax
	int v10; // eax
	int v11; // eax
	int v12; // eax
	int v13; // eax
	int v14; // eax
	int v15; // eax
	int v16; // eax
	int v17; // eax
	int v18; // eax
	int v19; // eax
	int v20; // eax
	int v21; // eax
	int v22; // eax
	int v23; // eax
	int v24; // eax
	int v25; // eax
	int v26; // eax
	int v27; // eax
	int v28; // eax
	int v29; // eax
	int v30; // eax
	int v31; // eax
	int v32; // eax
	int v33; // eax
	int v34; // eax
	int v35; // eax
	int v36; // eax
	int v37; // eax
	int v38; // eax

	property_node_create_func_p2(a1, 0, 7, "/ea3/pos/nr_coinslot", 1);
	property_node_create_func_p2(a1, 0, 7, "/ea3/pos/nr_article", 3);
	property_node_create_func_p2(a1, 0, 7, "/ea3/pos/nr_payment", 3);
	v1 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/start/type", "consume");
	if (v1)
		property_node_datasize_func_p(v1);
	v2 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/start/player_ref", "/opt/coin/slot1/player");
	if (v2)
		property_node_datasize_func_p(v2);
	v3 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/start/credit_ref", "/opt/coin/start");
	if (v3)
		property_node_datasize_func_p(v3);
	property_node_create_func_p2(a1, 0, 3, "/ea3/pos/coin/start/article", 1);
	v4 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/start/event", "game.s");
	if (v4)
		property_node_datasize_func_p(v4);
	v5 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/continue/type", "consume");
	if (v5)
		property_node_datasize_func_p(v5);
	v6 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/continue/player_ref", "/opt/coin/slot1/player");
	if (v6)
		property_node_datasize_func_p(v6);
	v7 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/continue/credit_ref", "/opt/coin/continue");
	if (v7)
		property_node_datasize_func_p(v7);
	property_node_create_func_p2(a1, 0, 3, "/ea3/pos/coin/continue/article", 2);
	v8 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/continue/event", "game.c");
	if (v8)
		property_node_datasize_func_p(v8);
	v9 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/slot1/type", "throw");
	if (v9)
		property_node_datasize_func_p(v9);
	v10 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/slot1/src", "coin");
	if (v10)
		property_node_datasize_func_p(v10);
	property_node_create_func_p2(a1, 0, 3, "/ea3/pos/coin/slot1/slot", 1);
	v11 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/slot1/player_ref", "/opt/coin/slot1/player");
	if (v11)
		property_node_datasize_func_p(v11);
	v12 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/slot1/coin_credit_ref", "/opt/coin/slot1/coin_credit");
	if (v12)
		property_node_datasize_func_p(v12);
	v13 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/slot1/event", "coin.i");
	if (v13)
		property_node_datasize_func_p(v13);
	v14 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/slots/type", "throw");
	if (v14)
		property_node_datasize_func_p(v14);
	v15 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/slots/src", "service");
	if (v15)
		property_node_datasize_func_p(v15);
	v16 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/slots/coin_event", "/coin/slot1");
	if (v16)
		property_node_datasize_func_p(v16);
	v17 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/slots/event", "service.p");
	if (v17)
		property_node_datasize_func_p(v17);
	v18 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/read/type", "consume");
	if (v18)
		property_node_datasize_func_p(v18);
	v19 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/read/player_ref", "/opt/coin/slot1/player");
	if (v19)
		property_node_datasize_func_p(v19);
	v20 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/read/credit_ref", "/opt/coin/event");
	if (v20)
		property_node_datasize_func_p(v20);
	property_node_create_func_p2(a1, 0, 3, "/ea3/pos/coin/read/article", 3);
	v21 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/read/event", "pdata.r");
	if (v21)
		property_node_datasize_func_p(v21);
	v22 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/write/type", "consume");
	if (v22)
		property_node_datasize_func_p(v22);
	v23 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/write/player_ref", "/opt/coin/slot1/player");
	if (v23)
		property_node_datasize_func_p(v23);
	v24 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/write/credit_ref", "/opt/coin/event");
	if (v24)
		property_node_datasize_func_p(v24);
	property_node_create_func_p2(a1, 0, 3, "/ea3/pos/coin/write/article", 3);
	v25 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/write/event", "pdata.w");
	if (v25)
		property_node_datasize_func_p(v25);
	v26 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/start_free/type", "consume");
	if (v26)
		property_node_datasize_func_p(v26);
	v27 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/start_free/player_ref", "/opt/coin/slot1/player");
	if (v27)
		property_node_datasize_func_p(v27);
	v28 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/start_free/credit_ref", "/opt/coin/event");
	if (v28)
		property_node_datasize_func_p(v28);
	property_node_create_func_p2(a1, 0, 3, "/ea3/pos/coin/start_free/article", 3);
	v29 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/start_free/event", "game.s.free");
	if (v29)
		property_node_datasize_func_p(v29);
	v30 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/exstart/type", "consume");
	if (v30)
		property_node_datasize_func_p(v30);
	v31 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/exstart/player_ref", "/opt/coin/slot1/player");
	if (v31)
		property_node_datasize_func_p(v31);
	v32 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/exstart/credit_ref", "/opt/coin/exstart");
	if (v32)
		property_node_datasize_func_p(v32);
	property_node_create_func_p2(a1, 0, 3, "/ea3/pos/coin/exstart/article", 1);
	v33 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/exstart/event", "HGT.game.s.ex");
	if (v33)
		property_node_datasize_func_p(v33);
	v34 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/end/type", "consume");
	if (v34)
		property_node_datasize_func_p(v34);
	v35 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/end/player_ref", "/opt/coin/slot1/player");
	if (v35)
		property_node_datasize_func_p(v35);
	v36 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/end/credit_ref", "/opt/coin/event");
	if (v36)
		property_node_datasize_func_p(v36);
	property_node_create_func_p2(a1, 0, 3, "/ea3/pos/coin/end/article", 3);
	v37 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/end/event", "game.e");
	if (v37)
		property_node_datasize_func_p(v37);
	v38 = property_node_create_func_p(a1, 0, 11, "/ea3/pos/coin/freeplay_ref", "/opt/coin/freeplay");
	if (v38)
		property_node_datasize_func_p(v38);
	return 0;
}

int __stdcall ea3boot(BYTE serviceMode, const char* pcbId, const char* hddId)
{
	int v3; // esi

	int v9; // eax
	int v10; // eax
	int v11; // eax
	int v12; // eax
	int v13; // eax
	int v14; // eax
	int v15; // eax
	int v16; // eax
	int v17; // eax
	int v18; // eax
	char v26[0x5000];
	int v19;
	v3 = property_create_func_p(23, v26, 0x5000);
	v9 = property_node_create_func_p(v3, 0, 11, "/ea3/soft/model", "HGT");
	if (v9)
		property_node_datasize(v9);
	v10 = property_node_create_func_p(v3, 0, 11, "/ea3/soft/dest", "E");
	if (v10)
		property_node_datasize(v10);
	v11 = property_node_create_func_p(v3, 0, 11, "/ea3/soft/spec", "A");
	if (v11)
		property_node_datasize(v11);
	v12 = property_node_create_func_p(v3, 0, 11, "/ea3/soft/rev", "A");
	if (v12)
		property_node_datasize(v12);
	v13 = property_node_create_func_p(v3, 0, 11, "/ea3/soft/ext", "2009062401");
	if (v13)
		property_node_datasize(v13);
	property_node_create_func_p2(v3, 0, 6, "/ea3/timezone", 32400);
	v14 = property_node_create_func_p(v3, 0, 11, "/ea3/id/pcbid", pcbId);
	if (v14)
		property_node_datasize(v14);
	v15 = property_node_create_func_p(v3, 0, 11, "/ea3/id/hardid", hddId);
	if (v15)
		property_node_datasize(v15);
	v16 = property_node_create_func_p(v3, 0, 11, "/ea3/encoding", "SHIFT_JIS");
	if (v16)
		property_node_datasize(v16);
	v17 = property_node_create_func_p(v3, 0, 11, "/ea3/network/services", "services.eamuse.konami.fun");
	if (v17)
		property_node_datasize(v17);
	eaBootExt(v3);
	property_node_create_func_p2(v3, 0, 7, "/ea3/facility/sz_prop_buf", 4096);
	property_node_create_func_p2(v3, 0, 7, "/ea3/pos/sz_opt_buf", 0x10000);
	property_node_create_func_p2(v3, 0, 7, "/ea3/network/sz_xrpc_buf", 0x8000);
	property_node_create_func_p2(v3, 0, 3, "/ea3/option/service", serviceMode != 0);
	property_node_create_func_p2(v3, 0, 3, "/ea3/option/posevent", 1);
	property_node_create_func_p2(v3, 0, 3, "/ea3/option/antiresale", 1);
	property_node_create_func_p2(v3, 0, 3, "/ea3/option/pcbevent", 1);
	property_node_create_func_p2(v3, 0, 3, "/ea3/service/package", 1);
	property_node_create_func_p2(v3, 0, 3, "/ea3/service/cardmng", 1);
	property_node_create_func_p2(v3, 0, 7, "/ea3/pkglist/nr_entry", 100);
	property_node_create_func_p2(v3, 0, 7, "/ea3/pkglist/nr_list_desc", 2);
	property_node_create_func_p2(v3, 0, 7, "/ea3/pkglist/nr_file_desc", 2);
	property_node_create_func_p2(v3, 0, 52, "/ea3/proxy/cache/use_previous_cache", 1);
	property_node_create_func_p2(v3, 0, 5, "/ea3/proxy/cache/max_items", 100);
	property_node_create_func_p2(v3, 0, 6, "/ea3/proxy/cache/retry_time/direct", 1800);
	property_node_create_func_p2(v3, 0, 9, "/ea3/proxy/cache/max_size", 947912704);
	//log_body_misc("ea3 boot", "センタ\x81[サ\x81[バ\x81[に\x90ﾚ続しています\x81B\n");
	if (v3)
		v18 = property_search_func_p(v3, 0, "/ea3");
	else
		v18 = 0;
	ea3_boot_func_p(v18);
	v19 = ea3_get_sic_func_p();
	return 1;
	//return log_body_misc_func_p("ea3 boot", "SIC: %s", v19);
}

signed int avsBoot()
{
	int v0; // esi
	int v1; // eax
	int v2; // eax
	void *v3; // eax
	void *v4; // edi
	void *v5; // eax
	int v6; // ecx
	int v7; // eax
	bool v8; // sf
	const char *v9; // eax
	const char *v10; // eax
	signed int result; // eax
	char v12[0x550]; // [esp+Ch] [ebp-550h]

	v0 = property_create_func_p(7, v12, 0x550);
	CreateDirectoryA("./CONF", 0);
	CreateDirectoryA("./CONF/NVRAM", 0);
	CreateDirectoryA("./CONF/RAW", 0);
	v1 = property_node_create_func_p(v0, 0, 11, "/config/fs/nvram/device", "./CONF/NVRAM");
	if (v1)
		property_node_datasize_func_p(v1);
	v2 = property_node_create_func_p(v0, 0, 11, "/config/fs/raw/device", "./CONF/RAW");
	if (v2)
		property_node_datasize(v2);
	property_node_create_func_p2(v0, 0, 5, "/config/thread/nr_semaphore", 32);
	property_node_create_func_p2(v0, 0, 5, "/config/net/nr_protocol", 8);
	property_node_create_func_p2(v0, 0, 5, "/config/net/nr_socket", 8);
	property_node_create_func_p2(v0, 0, 3, "/config/net/enable_raw", 1);
	property_node_create_func_p2(v0, 0, 6, "/config/time/gmt_offset", 0);
	if (property_get_error_func_p(v0))
		return 1;
	v3 = malloc(0x400000u);
	v4 = v3;
	avsMemRegion1 = v3;
	v5 = malloc(0x400000u);
	avsMemRegion2 = v5;
	if (!v4)
	{
		if (v5)
		{
			free(v5);
			avsMemRegion2 = 0;
		}
		return 1;
	}
	if (v5)
	{
		if (v0)
		{
			v7 = property_search_func_p(v0, 0, "/config");
			v4 = avsMemRegion1;
			v6 = v7;
			v5 = avsMemRegion2;
		}
		else
		{
			v6 = 0;
		}
		avs_boot_func_p(v6, v4, 0x400000, v5, 0x400000, ReturnsFalse, 0);
		result = 0;
	}
	else
	{
		free(v4);
		avsMemRegion1 = 0;
		result = 1;
	}
	return result;
}

#include <stdio.h>
#include <iostream>

inline bool file_exists(const std::string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

void* f(void* a)
{
	return a;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("ERROR: param needs to be target dll\n");
		printf("USAGE: canaryloader.exe path/to/some.dll\n");
		return 0;
	}

	if (!file_exists(argv[1]))
	{
		printf("ERROR: file does not exist.\n");
		return 0;
	}

	auto gameBinary = LoadLibraryA(argv[1]);
	auto libavs = LoadLibraryA("libavs-win32.dll");
	auto libavsea3 = LoadLibraryA("libavs-win32-ea3.dll");

	dll_entry_main dllMain = (dll_entry_main)GetProcAddress(gameBinary, "dll_entry_main");

	dll_entry_init dllInit = (dll_entry_init)GetProcAddress(gameBinary, "dll_entry_init");

	// Resolve libavs-win32.dll apis
	property_create_func_p = (property_create)GetProcAddress(libavs, "property_create");
	property_node_create_func_p = (property_node_create)GetProcAddress(libavs, "property_node_create");
	property_node_create_func_p2 = (property_node_create2)GetProcAddress(libavs, "property_node_create");
	property_node_datasize_func_p = (property_node_datasize)GetProcAddress(libavs, "property_node_datasize");
	property_search_func_p = (property_search)GetProcAddress(libavs, "property_search");
	property_get_error_func_p = (property_get_error)GetProcAddress(libavs, "property_get_error");
	avs_boot_func_p = (avs_boot)GetProcAddress(libavs, "avs_boot");
	log_body_misc_func_p = (log_body_misc)GetProcAddress(libavs, "log_body_misc");
	std_getenv_func_p = (std_getenv)GetProcAddress(libavs, "std_getenv");

	// Resolve libavs-win32-ea.dll apis
	ea3_boot_func_p = (ea3_boot)GetProcAddress(libavsea3, "ea3_boot");
	ea3_get_sic_func_p = (ea3_get_sic)GetProcAddress(libavsea3, "ea3_get_sic");
	// Init libavs-win32.dll
	avsBoot();

	// Init libavs-win32-ea3.dll
	ea3boot(0, "DEADFACE107B4493AC12", "0100107B4493AC12");

	dllInit();
	dllMain();

	while (true)
		Sleep(100);

	return 1;
}