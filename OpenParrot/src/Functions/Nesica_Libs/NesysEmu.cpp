#include "StdInc.h"
#include "NesysEmu.h"
//#if _M_IX86
#include "Utility/Utils.h"
#include "NesysNewsFile.h"
#include <Utility\GameDetect.h>
#pragma optimize("", off)
NesysEmu::NesysEmu()
	: m_initialized(false)
{
	m_commandHandlers[LCOMMAND_CLIENT_START] = [=](const uint8_t* data, size_t length)
	{
		struct __declspec(align(4)) _LIBRARY_INFO
		{
			unsigned int pid;
			char exePath[261];
			char version[33];
		};

		const _LIBRARY_INFO* linfo = reinterpret_cast<const _LIBRARY_INFO*>(data);

#ifdef _DEBUG
		info(true, "NESYS CLIENT START: pid %d, exePath %s, version %s", linfo->pid, linfo->exePath, linfo->version);
#endif

		SendResponse(SCOMMAND_CLIENT_START_REPLY, nullptr);
	};

	m_commandHandlers[LCOMMAND_CONNECT_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		SendResponse(SCOMMAND_CONNECT_REPLY, nullptr);


		Sleep(100);

		// h
		struct __declspec(align(4)) NESYS_TENPO_TABLE
		{
			int tenpo_id;
			char tenpo_name[31];
			char tenpo_addr[33];
			char ticket[33];
			char pref_name[17];
		};

		struct NESYS_NEWS_TABLE
		{
			int type;
			int size;
			char iFilePath[1024];
		};

		struct nesys_cert_init_response
		{
			NESYS_TENPO_TABLE tenpo;
			NESYS_NEWS_TABLE news;
			uint32_t serverSize;
			char server[];
		};

		const char* wat = "card_id=7020392010281502";

		nesys_cert_init_response* response = (nesys_cert_init_response*)malloc(sizeof(nesys_cert_init_response) + strlen(wat));
		response->tenpo.tenpo_id = 1337;
		strcpy(response->tenpo.tenpo_name, "leet");
		strcpy(response->tenpo.tenpo_addr, "l33t");
		strcpy(response->tenpo.ticket, "teel");
		strcpy(response->tenpo.pref_name, "t33l");

		response->news.type = 0;
		response->news.size = strlen("./OpenParrot/news.png");
		strcpy(response->news.iFilePath, "./OpenParrot/news.png");

		response->serverSize = strlen(wat);
		memcpy(response->server, wat, strlen(wat));

		SendResponse(SCOMMAND_CERT_INIT_NOTICE, response, sizeof(nesys_cert_init_response) + strlen(wat));
		free(response);
	};

	m_commandHandlers[LCOMMAND_DISCONNECT_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		SendResponse(SCOMMAND_DISCONNECT_REPLY, nullptr);
	};

	m_commandHandlers[LCOMMAND_GAME_FREE_START_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		SendResponse(SCOMMAND_GAME_STATUS_REPLY, nullptr);
	};

	m_commandHandlers[LCOMMAND_GAME_FREE_END_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		SendResponse(SCOMMAND_GAME_STATUS_REPLY, nullptr);
	};

	m_commandHandlers[LCOMMAND_GAME_START_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		SendResponse(SCOMMAND_GAME_STATUS_REPLY, nullptr);
	};

	m_commandHandlers[LCOMMAND_GAME_END_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		SendResponse(SCOMMAND_GAME_STATUS_REPLY, nullptr);
	};

	// get card
	m_commandHandlers[LCOMMAND_CARD_SELECT_REQUEST] = [=](const uint8_t* dataa, size_t length)
	{
		OutputDebugStringA("Getting card data");
		struct __declspec(align(4)) _CARD_GETDATA
		{
			char cardId[16];
			unsigned int dataType;
			unsigned int paramSize;
			char param[1];
		};

		const _CARD_GETDATA* card = reinterpret_cast<const _CARD_GETDATA*>(dataa);

		FILE* f = fopen(va("card_%.16s_%d.bin", card->cardId, card->dataType), "rb");

		size_t dataSize = 16384;
		char data[16384];
		memset(data, 0x0, 0x4000);

		if (f)
		{
			dataSize = fread(data, 1, 16384, f);
			fclose(f);

			data[dataSize] = '\0';

			++dataSize;
		}

		struct cardstatus
		{
			uint32_t status;
			uint32_t times;
			uint32_t days;
			uint32_t size;
			char data[];
		};

		cardstatus* s = (cardstatus*)malloc(sizeof(cardstatus) + dataSize);
		s->status = 1;
		s->times = 100;
		s->days = 100;
		s->size = dataSize;
		memcpy(s->data, data, s->size);
		if(GameDetect::currentGame != GameID::CrimzonClover) // if someone fixes card code, remove this
		SendResponse(SCOMMAND_CARD_SELECT_REPLY, s, sizeof(cardstatus) + dataSize);
	};

	struct __declspec(align(4)) _CARD_SENDDATA
	{
		char cardId[16];
		unsigned int frequency;
		unsigned int transferMode;
		unsigned int dataType;
		unsigned int dataSize;
		char data[1];
	};

	// new card
	m_commandHandlers[LCOMMAND_CARD_INSERT_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		struct cardstatus
		{
			uint32_t status;
			uint32_t times;
			uint32_t days;
			uint32_t size;
			char data[];
		};

		cardstatus s;
		s.status = 1;
		s.times = 0;
		s.days = 0;
		s.size = 0;

		SendResponse(SCOMMAND_CARD_INSERT_REPLY, &s);
	};

	// update card
	m_commandHandlers[LCOMMAND_CARD_UPDATE_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		const _CARD_SENDDATA* card = reinterpret_cast<const _CARD_SENDDATA*>(data);

		FILE* f = fopen(va("card_%s_%d.bin", card->cardId, card->dataType), "wb");
		if (f)
		{
			fwrite(card->data, 1, card->dataSize, f);
			fclose(f);
		}

		struct cardstatus
		{
			uint32_t status;
			uint32_t times;
			uint32_t days;
			uint32_t size;
			char data[];
		};

		cardstatus* s = (cardstatus*)malloc(sizeof(cardstatus) + card->dataSize);
		s->status = 1;
		s->times = 100;
		s->days = 100;
		s->size = card->dataSize;
		memcpy(s->data, card->data, s->size);

		SendResponse(SCOMMAND_CARD_UPDATE_REPLY, s, sizeof(cardstatus) + card->dataSize);
	};

	m_commandHandlers[LCOMMAND_INCOME_FREE_START_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		struct reply
		{
			int playerid;
			int status;
		} r;

		r.playerid = 0;
		r.status = 1;

		SendResponse(SCOMMAND_INCOME_STATUS_REPLY, &r);
	};

	m_commandHandlers[LCOMMAND_INCOME_START_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		struct reply
		{
			int playerid;
			int status;
		} r;

		r.playerid = 0;
		r.status = 1;

		SendResponse(SCOMMAND_INCOME_STATUS_REPLY, &r);
	};

	m_commandHandlers[LCOMMAND_RANKING_DATA_REQUEST] = [=](const uint8_t* dataa, size_t length)
	{
		struct ranking
		{
			uint32_t status;
			uint32_t size;
			char data[];
		};

		ranking r;
		r.status = 1;
		r.size = 0;

		SendResponse(SCOMMAND_RANKING_DATA_REPLY, &r);
	};

	m_commandHandlers[LCOMMAND_UPLOAD_CONFIG_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		struct __declspec(align(4)) _UPLOAD_CONFIG
		{
			unsigned int dataType;
			unsigned int dataSize;
			char data[1];
		};

		const _UPLOAD_CONFIG* config = reinterpret_cast<const _UPLOAD_CONFIG*>(data);

		int status = 1;
		SendResponse(SCOMMAND_UPLOAD_CONFIG_REPLY, &status);
	};

	m_commandHandlers[LCOMMAND_SERVICE_VERSION_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		struct _SERVICE_VER
		{
			char version[33];
		};

		_SERVICE_VER version;
		strcpy(version.version, "13.37 1337/01/01");

		SendResponse(SCOMMAND_SERVICE_VERSION_REPLY, &version);
	};

	m_commandHandlers[LCOMMAND_SET_INCOME_MODE_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		SendResponse(SCOMMAND_SET_INCOME_MODE_REPLY, nullptr);
	};

	m_commandHandlers[LCOMMAND_GAMESTATUS_RESET_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		SendResponse(SCOMMAND_GAMESTATUS_RESET_REPLY, nullptr);
	};

	m_commandHandlers[LCOMMAND_LOCALNW_INFO_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		// may also need 'notice' replies
		SendResponse(SCOMMAND_LOCALNW_INFO_REPLY, nullptr);

		Sleep(100);

		struct nesys_nwinfo_advertise_table_t
		{
			char mac[16];
			char ip[16];
			char dhcp[16];
			char gateway[16];
			int gameid;
			int attr;
		};

		struct nwi
		{
			int s;
			int size;
			nesys_nwinfo_advertise_table_t data;
		};

		nwi nw;
		nw.s = 1;
		nw.size = sizeof(nw.data);

		strcpy(nw.data.ip, config["Network"]["Ip"].c_str());
		strcpy(nw.data.gateway, config["Network"]["Gateway"].c_str());
		strcpy(nw.data.dhcp, config["Network"]["Dns1"].c_str());
		strcpy(nw.data.mac, "DEADBABECAFE");
		nw.data.gameid = 42;
		nw.data.attr = 0;

		SendResponse(SCOMMAND_LOCALNW_INFO_NOTICE, &nw);
	};

	m_commandHandlers[LCOMMAND_GLOBALADDR_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		struct nesys_globaladdr_table_t
		{
			int globaladdrStatus;
			int responseTime;
			char globaladdrData[32];
		};

		nesys_globaladdr_table_t globalAddr;
		globalAddr.globaladdrStatus = 1;
		globalAddr.responseTime = 42;
		strcpy(globalAddr.globaladdrData, "127.1.3.37");

		SendResponse(SCOMMAND_GLOBALADDR_REPLY, &globalAddr);
	};

	m_commandHandlers[LCOMMAND_ADAPTER_INFO_REQUEST] = [=](const uint8_t* data, size_t length)
	{
		struct NNS_LANINTERFACE
		{
			int Dhcp_Enabled;
			char gateway[32];
			char ipAddr[32];
			char submask[32];
			char dhcpServers[32];
			char dnsServers[32];
			char macAddress[64];
		};

		struct nesys_laninterface_table_t
		{
			int interfaceStatus;
			NNS_LANINTERFACE interfaceData;
		};

		nesys_laninterface_table_t lan;
		lan.interfaceStatus = 1;
		
		lan.interfaceData.Dhcp_Enabled = strcmp(config["Network"]["Dhcp"].c_str(), "1") == 0;
		strcpy(lan.interfaceData.ipAddr, config["Network"]["Ip"].c_str());
		strcpy(lan.interfaceData.submask, config["Network"]["Mask"].c_str());
		strcpy(lan.interfaceData.gateway, config["Network"]["Gateway"].c_str());
		strcpy(lan.interfaceData.dhcpServers, config["Network"]["Dns1"].c_str());
		strcpy(lan.interfaceData.dnsServers, config["Network"]["Dns1"].c_str());
		strcpy(lan.interfaceData.macAddress, "DEADBABECAFE");

		SendResponse(SCOMMAND_ADAPTER_INFO_REPLY, &lan);
	};
}

NesysEmu::~NesysEmu()
{
	if (m_initialized)
	{
		Shutdown();
	}
}

void NesysEmu::ProcessRequest(const uint8_t* data, size_t length)
{
	uintptr_t endData = reinterpret_cast<uintptr_t>(data) + length;

	while (reinterpret_cast<uintptr_t>(data) < endData)
	{
		auto header = reinterpret_cast<const NesysCommandHeader*>(data);

		if (header->command != CCOMMAND_NONE)
		{
			ProcessRequestInternal(header);
		}

		data += sizeof(header) + header->length + 4;
	}	
}

void NesysEmu::ProcessRequestInternal(const NesysCommandHeader* header)
{
	auto handler = m_commandHandlers.find(header->command);

	if (handler != m_commandHandlers.end())
	{
#ifdef _DEBUG
		info(true, "got nesys command: %d\n", header->command);
#endif

		handler->second(header->data, header->length);
		return;
	}

#ifdef _DEBUG
	info(true, "got unhandled nesys command: %d\n", header->command); //
#endif
}

static thread_local HANDLE g_curPipe;

void NesysEmu::ProcessPipe(HANDLE hPipe)
{
	uint8_t requestBuffer[8192];

	while (true)
	{
		DWORD bytesRead;
		auto success = ReadFile(hPipe, requestBuffer, sizeof(requestBuffer), &bytesRead, nullptr);

		if (!success || bytesRead == 0)
		{
			break;
		}

		g_curPipe = hPipe;
		ProcessRequest(requestBuffer, bytesRead);

		Sleep(150);
	}

	CloseHandle(hPipe);
}

void NesysEmu::SendResponse(nesys_command command, const void* data, size_t dataSize)
{
	size_t outSize = (sizeof(NesysCommandHeader) * 1) + dataSize;

	uint8_t* buffer = (uint8_t*)malloc(outSize);
	((NesysCommandHeader*)buffer)->command = command;
	((NesysCommandHeader*)buffer)->length = dataSize;

	if (dataSize > 0)
	{
		memcpy(buffer + sizeof(NesysCommandHeader), data, dataSize);
	}

	//((NesysCommandHeader*)(buffer + sizeof(NesysCommandHeader) + dataSize))->command = CCOMMAND_NONE;
	//((NesysCommandHeader*)(buffer + sizeof(NesysCommandHeader) + dataSize))->length = 0;

	DWORD outWritten;
	WriteFile(g_curPipe, buffer, outSize, &outWritten, nullptr);

	free(buffer);
}

void NesysEmu::Initialize(bool isDarius)
{
	std::thread([=]()
	{
		while (true)
		{
			HANDLE pipe = CreateNamedPipeW(isDarius ? L"\\\\.\\pipe\\nesystest" : L"\\\\.\\pipe\\nesys_games", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 8192, 8192, 0, nullptr);

			if (pipe == INVALID_HANDLE_VALUE)
			{
				return;
			}

			bool connected = ConnectNamedPipe(pipe, nullptr) ? true : GetLastError() == ERROR_PIPE_CONNECTED;

			if (!connected)
			{
				CloseHandle(pipe);
				continue;
			}

			std::thread([=]()
			{
				ProcessPipe(pipe);
			}).detach();
		}
	}).detach();

	m_initialized = true;
}

void NesysEmu::Shutdown()
{

}

static NesysEmu g_nesysEmu;

#include <iphlpapi.h>

static DWORD(WINAPI* g_origGetIfEntry)(PMIB_IFROW ifRow);

DWORD WINAPI GetIfEntryFunc(
	_Inout_ PMIB_IFROW pIfRow
)
{
	DWORD rv = g_origGetIfEntry(pIfRow);

	pIfRow->dwOperStatus = IF_OPER_STATUS_CONNECTED;

	return 0;
}

void WriteNewsFile()
{
	// News for NESYS Emu
	FILE *pFile = fopen(".\\OpenParrot\\news.png", "wb");
	if (pFile != NULL)
	{
		fwrite(nesysNews, sizeof(char), sizeof(nesysNews), pFile);
		fclose(pFile);
	}
}

void init_NesysEmu(bool IsDarius)
{
	CreateDirectoryA("OpenParrot", nullptr);
	WriteNewsFile();
	MH_Initialize();
	MH_CreateHookApi(L"iphlpapi.dll", "GetIfEntry", GetIfEntryFunc, (void**)&g_origGetIfEntry);
	MH_EnableHook(MH_ALL_HOOKS);

	g_nesysEmu.Initialize(IsDarius);
}
#pragma optimize("", on)