#include <StdInc.h>
#include <string>
#include <vector>
#include <sstream>
#include <codecvt>

#define UPDC32(octet,crc) (crc_32_tab[((crc) ^ ((BYTE)octet)) & 0xff] ^ ((crc) >> 8))

uint32_t GetCRC32(const void* pData, int length)
{
	uint32_t _crc = 0xFFFFFFFF;
	unsigned char* pCur = (unsigned char*)pData;
	int remaining = length;
	for (; remaining--; ++pCur)
		_crc = (_crc >> 8) ^ kCrc32Table[(_crc ^ *pCur) & 0xff];

	return _crc ^ 0xFFFFFFFF;
}
#ifdef _DEBUG
void info(bool unused, const char* format, ...)
{
	va_list args;
	char buffer[1024];

	va_start(args, format);
	int len = _vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	buffer[len] = '\n';
	buffer[len + 1] = '\0';

	OutputDebugStringA(buffer);
}
#else
void info(bool unused, const char* format, ...)
{
}
#endif
int strcmp(const char* str1, const char* str2, bool csensitive)
{
	return (csensitive ? ::strcmp(str1, str2) : ::_stricmp(str1, str2));
}

int Compare(const std::string& str1, const std::string& str2, bool case_sensitive)
{
	if (str1.length() == str2.length())
		return strcmp(str1.c_str(), str2.c_str(), case_sensitive);

	return (str1.length() < str2.length() ? -1 : 1);
}

BOOL FileExists(char* szPath)
{
	DWORD dwAttrib = GetFileAttributesA(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool ToBool(const std::string& s)
{
	if (s.empty())
	{
		return false;
	}

	if (s.size() == 1) return s.front() != '0';
	return !!Compare(s, "false", false);
}

int ToInt(const std::string& s) // handy function
{
        return stoi(s);		// real simple. :)
}

bool IpToByte(const char* ip, char bytes[4])
{
	return (sscanf(ip, "%hhu.%hhu.%hhu.%hhu", &bytes[0], &bytes[1], &bytes[2], &bytes[3]) == 4);
}

DWORD crc32buf(unsigned char *buf, size_t len)
{
	register DWORD oldcrc32;

	oldcrc32 = 0xFFFFFFFF;

	for (; len; --len, ++buf)
	{
		oldcrc32 = UPDC32(*buf, oldcrc32);
	}

	return ~oldcrc32;
}

void GetDesktopResolution(int& x, int& y)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	x = desktop.right;
	y = desktop.bottom;
}

#define BUFFER_COUNT 8
#define BUFFER_LENGTH 32768

const char* va(const char* string, ...)
{
	static thread_local int currentBuffer;
	static thread_local std::vector<char> buffer;

	if (!buffer.size())
	{
		buffer.resize(BUFFER_COUNT * BUFFER_LENGTH);
	}

	int thisBuffer = currentBuffer;

	va_list ap;
	va_start(ap, string);
	int length = vsnprintf(&buffer[thisBuffer * BUFFER_LENGTH], BUFFER_LENGTH, string, ap);
	va_end(ap);

	if (length >= BUFFER_LENGTH)
	{
		//GlobalError("Attempted to overrun string in call to va()!");
		return "";
	}

	buffer[(thisBuffer * BUFFER_LENGTH) + BUFFER_LENGTH - 1] = '\0';

	currentBuffer = (currentBuffer + 1) % BUFFER_COUNT;

	return &buffer[thisBuffer * BUFFER_LENGTH];
}

const wchar_t* va(const wchar_t* string, ...)
{
	static thread_local int currentBuffer;
	static thread_local std::vector<wchar_t> buffer;

	if (!buffer.size())
	{
		buffer.resize(BUFFER_COUNT * BUFFER_LENGTH);
	}

	int thisBuffer = currentBuffer;

	va_list ap;
	va_start(ap, string);
	int length = vswprintf(&buffer[thisBuffer * BUFFER_LENGTH], BUFFER_LENGTH, string, ap);
	va_end(ap);

	if (length >= BUFFER_LENGTH)
	{
		//GlobalError("Attempted to overrun string in call to va()!");
		return L"";
	}

	buffer[(thisBuffer * BUFFER_LENGTH) + BUFFER_LENGTH - 1] = '\0';

	currentBuffer = (currentBuffer + 1) % BUFFER_COUNT;

	return &buffer[thisBuffer * BUFFER_LENGTH];
}

static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> m_converter;

std::string ToNarrow(const std::wstring& wide)
{
	return m_converter.to_bytes(wide);
}

std::wstring ToWide(const std::string& narrow)
{
	return m_converter.from_bytes(narrow);
}
