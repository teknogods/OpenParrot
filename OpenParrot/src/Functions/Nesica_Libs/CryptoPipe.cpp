#include <StdInc.h>
#if _M_IX86
#include "CryptoPipe.h"
#include <wincrypt.h>
#pragma comment(lib, "Advapi32.lib")

// TODO: NEED TO BE PLACED IN FILES!
const char *key_magicalbeat = "0702000000A40000525341320002000001000100411FE010238446E4C51A60E182DD275B6C1D35DE1B3B1392C7F9A70DFB01C8BF2309CC85E74D88ECE52041F985D785BB7CBD3F3A8BFC89E8D2A5A8440796CDD2BD2B2D5FB954F36193AA685E568DC940284E45BBDDFF89E721EBBF670362F8F6D5D73B0CAA16AD31C8F819A0936FAB11BB11A855C213D9C583A8621A97AE82DA7108DC76DAB389CD14A5E577C5E9A1908A0D0EF56A3FE1AA0F6103450F4650C8218FC47412A3ADBDD146E68E5FC49D4D64AD28641941FC6FD057DE15233BAD288FA032390566E9B4FFED0B9F5A31A2A587B3F68405BF1A3A79AE05BA256C8C2C81ADBD14DFAA8EF774684AD6D606574BB426DF079270A0A333E65B281D6FECD260D618F32475ACE360DE3A8CB6540192A94FA00E01109F77BBA9709428C2E3A3";
const char *key_bbcf = "0702000000a400005253413200020000010001008f35cf7db98ccacf0e4586b2bc2816911e6629328edd4678424312e1c698e4a8341232da9a37ef7fe4257185522b618b49d3939b8e83b1a591e0c06d5d0223c50da989e3ab190e4b989ebaee4d04428a00bfb6e7840f17a5e7f6c2e8d5e776eb0b3af84ad026ef84cb69e8e7933a5a99342018764317a73780507cdb5b6154d629b8da775bb09b132e3dfac255b771863809acfa64fea8f07a94801096d9046955d8729eb98779f8561fdf8f6531ad051546aacd74702f486e456e3c0361346ed6033004ee562885912cafdd91d9a4844d267437eee304b4f06c46b75bbe972b31a414972fdb5e0a69999734685e54598f0565f9afc0cd162434fb9d37e983154fefb3080f8ff41ff9be8596fa4042b96a441f6b1f8eb416c127a57f256ff055";
const char *key_persona4arena = "0702000000A40000525341320002000001000100359073C4EF2B8E9556FFDED09E5E1C4374169BA2A2077E9993C34E7FB3B499D9624F620018AFAEEED697D7E72432B488E8E4A2A1B648EE24C9E878D9145A36D87F39DFCFD4AA3751C70039DFB4CE022A85A81D7440287538A41EB3B3D9A0F0FD4B481E274B71AF8BDD36AC6F7A930CC67E41BE6E65628A50F3779082555FF7D97FB45E4D6973BEB316D220BC45E2F20FBD1FD9271CEA65AC14B09136D99A31A179CE1933F5343FE930D858769FD7B861C7BF3ECC187792A9C200856822C1AB2138628CB7183DBCE115589A4A0F213FD77B91758D13D762714666B97A428723895512E57C29AB3C68113120D7CB3269A605AC483601BD6B1BDAC238759D564EA6DA64A445FC502251FBEAA2621BD157040C60637371096A6B261D0FCB767F9F7C";
const char *key_bbcp = "0702000000a400005253413200020000010001008f35cf7db98ccacf0e4586b2bc2816911e6629328edd4678424312e1c698e4a8341232da9a37ef7fe4257185522b618b49d3939b8e83b1a591e0c06d5d0223c50da989e3ab190e4b989ebaee4d04428a00bfb6e7840f17a5e7f6c2e8d5e776eb0b3af84ad026ef84cb69e8e7933a5a99342018764317a73780507cdb5b6154d629b8da775bb09b132e3dfac255b771863809acfa64fea8f07a94801096d9046955d8729eb98779f8561fdf8f6531ad051546aacd74702f486e456e3c0361346ed6033004ee562885912cafdd91d9a4844d267437eee304b4f06c46b75bbe972b31a414972fdb5e0a69999734685e54598f0565f9afc0cd162434fb9d37e983154fefb3080f8ff41ff9be8596fa4042b96a441f6b1f8eb416c127a57f256ff055";
const char *key_kofxiiiclimax = "0702000000A400005253413200020000010001000D20722B0858DCFAE9783F33E67BED82A0C1A3FE17FBB733B056A2337ADA9DAAE028079B07EE25CBCC9C2B72AA6154CEEF898C8CA9F168684D9A3806D6F622D4FF5CFFDDA03325FD7322D1CBF9CD8E45773E116B353871EEF7E5DCAAE3DCE1F8F326C1922DE4B76F1DD075DB788D67B6719126FBB2497FA0CB855F56651334DA152F3AED5D8A53C92A83F08177535B44420FEB2617E932791DC9DB0A26250755FDC37B74BCFF5DDDFDB2E52AE59D7E52042A022D37A9D73339ECA29F0DF60C1F792BCB25A3BCC5ED53D6A0DA2D403246C12EA958C4F5DAE9FFA0D10B7C31ECC3E5DC9A2418104B3A7825FBC527177EE00056176A3176323DE62CC834D36095D26CC9841D331A72814FE0D04BCE0B3A666C6896E0571E77323D5B6797B20168B9";
const char *key_persona4ultimix = "0702000000a40000525341320002000001000100359073c4ef2b8e9556ffded09e5e1c4374169ba2a2077e9993c34e7fb3b499d9624f620018afaeeed697d7e72432b488e8e4a2a1b648ee24c9e878d9145a36d87f39dfcfd4aa3751c70039dfb4ce022a85a81d7440287538a41eb3b3d9a0f0fd4b481e274b71af8bdd36ac6f7a930cc67e41be6e65628a50f3779082555ff7d97fb45e4d6973beb316d220bc45e2f20fbd1fd9271cea65ac14b09136d99a31a179ce1933f5343fe930d858769fd7b861c7bf3ecc187792a9c200856822c1ab2138628cb7183dbce115589a4a0f213fd77b91758d13d762714666b97a428723895512e57c29ab3c68113120d7cb3269a605ac483601bd6b1bdac238759d564ea6da64a445fc502251fbeaa2621bd157040c60637371096a6b261d0fcb767f9f7c";
const char *key_usf4 = "0702000000a40000525341320002000001000100b77cb7dfef27ca5ed4895576c6a68518bab782e60dae9dffad94434fb9366125e46630352261afe960e8401a3c5ee1cfc15ec2b1849e02a3dcc30e47ea3100a5c7175f9207aab497a9c9c0bf19d30a9fbb294a761147607927163685624dced391d3ae2211c6c11350077070386a195a8641f0e7920ddbcb200f990f55c76dc73de119d62751e5cb6cfa3653481d3e711b666c9a6cc9c2a4e50f4e55ce2831a821d75448cda3a2d6f3b359012bff571842c34a844989a4af0a4a5081cb8dc0780173865658cd8b999f9baaa3febc9c95b1477cd6af5060af603e597b3547fd0021a422c3a2cd0028523b5bd12b23bd4a6d2924ca920870cba2f38477631589cc4251590fba955d114f2ce0d6228c4932c1f2d9b753cda2d646591c48e164820e";

DWORD dword_44DEC4;
DWORD dword_44DEC0;

void LogStuff(const char* text, int a)
{
	//printf("%s", text);
}

DWORD LinkRC5Data(const char* file, void* buf, int gameId)
{
	DWORD num = 0;
	char key[308];
	if (!buf)
		return 308;
	switch (gameId)
	{
	case 0:
		for (int i = 0; i < 308; i++)
		{
			sscanf(&key_magicalbeat[i * 2], "%02x", &num);
			key[i] = num;
		}
		break;
	case 1:
		for (int i = 0; i < 308; i++)
		{
			sscanf(&key_bbcf[i * 2], "%02x", &num);
			key[i] = num;
		}
		break;
	case 2:
		for (int i = 0; i < 308; i++)
		{
			sscanf(&key_persona4arena[i * 2], "%02x", &num);
			key[i] = num;
		}
		break;
	case 3:
		for (int i = 0; i < 308; i++)
		{
			sscanf(&key_bbcp[i * 2], "%02x", &num);
			key[i] = num;
		}
		break;
	case 4:
		for (int i = 0; i < 308; i++)
		{
			sscanf(&key_kofxiiiclimax[i * 2], "%02x", &num);
			key[i] = num;
		}
		break;
	case 5:
		for (int i = 0; i < 308; i++)
		{
			sscanf(&key_persona4ultimix[i * 2], "%02x", &num);
			key[i] = num;
		}
		break;
	case 6:
		for (int i = 0; i < 308; i++)
		{
			sscanf(&key_usf4[i * 2], "%02x", &num);
			key[i] = num;
		}
		break;
	default:
		for (int i = 0; i < 308; i++)
		{
			sscanf(&key_usf4[i * 2], "%02x", &num);
			key[i] = num;
		}
		break;
	}
	memcpy(buf, key, 308);
	return 308;
}

HCRYPTPROV GenerateKey()
{
	HCRYPTPROV result; // eax@2
	HCRYPTPROV phProv; // [sp+4h] [bp-4h]@1

	phProv = 0;
	if (CryptAcquireContextA(&phProv, "TypeXAppCrypt", "Microsoft Base Cryptographic Provider v1.0", 1u, 0)
		|| (result = CryptAcquireContextA(&phProv, "TypeXAppCrypt", "Microsoft Base Cryptographic Provider v1.0", 1u, CRYPT_NEWKEYSET)) != 0)
	{
		result = phProv;
	}
	return result;
}

bool __cdecl ReadFromPipe_410390(HANDLE hPipe, BYTE** a2, DWORD* a3, bool* pMode)
{
	signed int dwSizeBytePos; // esi@1
	int dwAllocSizeState; // ebp@1
	char i; // bl@2
	DWORD dwLastError; // eax@4
					   //unsigned __int16 v8; // dx@16
	int dwAllocSize; // ebp@18
	HANDLE hHeap1; // eax@18
	BYTE *pResultBuffer; // esi@18
	bool bResult; // al@19
	HANDLE hHeap2; // eax@21
	int v14 = 0; // [sp+0h] [bp-18h]@0
	unsigned char Buffer; // [sp+13h] [bp-5h]@3
	DWORD NumberOfBytesRead; // [sp+14h] [bp-4h]@3

	dwSizeBytePos = 0;
	dwAllocSizeState = 0;
	while (true)
	{
		for (i = 0;; i = 1)
		{
			do
			{
			readByteLabel:
				if (ReadFile(hPipe, &Buffer, 1u, &NumberOfBytesRead, 0))
					break;
				dwLastError = GetLastError();
				if (dwLastError == ERROR_BROKEN_PIPE)
					return false;
			} while (dwLastError != ERROR_IO_PENDING);
			if (Buffer != 0xFF)
				break;
			if (i)
			{
				i = 0;
				goto LABEL_9;
			}
		}
		if ((Buffer == 0xFE || Buffer == 0xFD) && i)
		{
			*pMode = Buffer != 0xFE;
			dwSizeBytePos = 1;
			continue;
		}
		break;
	}
LABEL_9:
	switch (dwSizeBytePos)
	{
	case 1:
		dwAllocSizeState |= Buffer;
		++dwSizeBytePos;
		goto readByteLabel;
	case 2:
		dwAllocSizeState |= Buffer << 8;
		++dwSizeBytePos;
		goto readByteLabel;
	case 3:
		dwAllocSizeState |= Buffer << 16;
		++dwSizeBytePos;
		goto readByteLabel;
	case 4:
		dwAllocSize = (Buffer << 24) | dwAllocSizeState;
		hHeap1 = GetProcessHeap();
		pResultBuffer = (BYTE*)HeapAlloc(hHeap1, 0, dwAllocSize);
		if (!pResultBuffer)
		{
			//LogStuff("Heap Allo\n", v14);
			return false;
		}
		if (!ReadFile(hPipe, pResultBuffer, dwAllocSize, &NumberOfBytesRead, 0))
		{
			hHeap2 = GetProcessHeap();
			HeapFree(hHeap2, 0, pResultBuffer);
			return false;
		}
		*a3 = dwAllocSize;
		*a2 = pResultBuffer;
		bResult = true;
		break;
	default:
		goto readByteLabel;
	}
	return bResult;
}

HANDLE PipeStuff_410550(int gameId)
{
	HANDLE result; // eax@1
	HANDLE hPipe; // edi@1
	DWORD dwRC5DataLen; // esi@4
	unsigned __int8 *pRC5DataBuf; // ebx@4
	BYTE *pbDataTemp; // esi@12
	HANDLE hHeap1; // eax@12
	BYTE *v6; // esi@13
	HANDLE hHeap2; // eax@13
	DWORD v8; // ST14_4@17
	HANDLE hHeap3; // eax@17
	BYTE *v10; // esi@17
	HANDLE hHeap4; // eax@17
	int v12 = 0; // [sp-Ch] [bp-38h]@1
	char v13; // [sp+6h] [bp-26h]@6
	bool bMode;
	DWORD dwRead;
	//char a4[5]; // [sp+7h] [bp-25h]@7
	DWORD pdwDataLen; // [sp+Ch] [bp-20h]@15
	HCRYPTKEY v16; // [sp+10h] [bp-1Ch]@9
	HCRYPTPROV hProv; // [sp+14h] [bp-18h]@3
	HCRYPTKEY hKey; // [sp+18h] [bp-14h]@11
	DWORD dwDataLen; // [sp+1Ch] [bp-10h]@7
	BYTE *pbData; // [sp+20h] [bp-Ch]@7
	int Buffer; // [sp+24h] [bp-8h]@8
	HCRYPTKEY phKey; // [sp+28h] [bp-4h]@4

	hProv = GenerateKey();
	result = CreateNamedPipeA("\\\\.\\pipe\\TtxAppCtyptPipe", PIPE_ACCESS_DUPLEX, PIPE_WAIT, 1u, 0x400u, 0x400u, 1000u, 0);
	hPipe = result;
	if (result != INVALID_HANDLE_VALUE)
	{
		dwRC5DataLen = LinkRC5Data("C:\\windows\\system32\\8B97D021-071D-45dd-A592-0ABB3BAEF01A", 0, gameId);
		pRC5DataBuf = (unsigned __int8 *)malloc(dwRC5DataLen);
		LinkRC5Data("C:\\windows\\system32\\8B97D021-071D-45dd-A592-0ABB3BAEF01A", pRC5DataBuf, gameId);
		result = (HANDLE)CryptImportKey(hProv, pRC5DataBuf, dwRC5DataLen, 0, 0, &phKey);
		if (result)
		{
			dword_44DEC0 = 1;
			ConnectNamedPipe(hPipe, 0);
			v13 = 0;
			while (true)
			{
				while (!ReadFromPipe_410390(hPipe, &pbData, &dwDataLen, &bMode))
				{
					Buffer = 0;
					WriteFile(hPipe, &Buffer, 4u, &dwRead, 0);
					DisconnectNamedPipe(hPipe);
					ConnectNamedPipe(hPipe, 0);
				}
				//LogStuff("Recieved.\n", v12);
				v16 = 0;
				if (bMode)
				{
					if (v13)
						CryptDestroyKey(hKey);
					pbDataTemp = pbData;
					CryptImportKey(hProv, pbData, dwDataLen, 0, 0, &hKey);
					v13 = 1;
					hHeap1 = GetProcessHeap();
					HeapFree(hHeap1, 0, pbDataTemp);
				}
				else
				{
					v6 = pbData;
					Buffer = CryptImportKey(hProv, pbData, dwDataLen, phKey, 1u, &v16);
					hHeap2 = GetProcessHeap();
					HeapFree(hHeap2, 0, v6);
					if (Buffer)
					{
						if (CryptExportKey(v16, hKey, 1u, 0, 0, &pdwDataLen))
						{
							v8 = pdwDataLen;
							hHeap3 = GetProcessHeap();
							v10 = (BYTE *)HeapAlloc(hHeap3, 0, v8);
							CryptExportKey(v16, hKey, 1u, 0, v10, &pdwDataLen);
							WriteFile(hPipe, &pdwDataLen, 4u, &dwRead, 0);
							WriteFile(hPipe, v10, pdwDataLen, &dwRead, 0);
							hHeap4 = GetProcessHeap();
							HeapFree(hHeap4, 0, v10);
						}
						else
						{
							Buffer = 0;
							//LogStuff("It failed in the export of the key. \n", v12);
							WriteFile(hPipe, &Buffer, 4u, &dwRead, 0);
						}
						if (v16)
							CryptDestroyKey(v16);
					}
					else
					{
						Buffer = 0;
						//LogStuff("It failed in the import of the key. \n", v12);
						WriteFile(hPipe, &Buffer, 4u, &dwRead, 0);
					}
				}
			}
		}
		dword_44DEC0 = 2;
	}
	else
	{
		dword_44DEC0 = 2;
	}
	return result;
}

unsigned long __stdcall TXAppCryptThread(LPVOID sId)
{
	int id = *static_cast<int*>(sId);
	PipeStuff_410550(id);
	delete sId;
	return 1;
}

void init_CryptoPipe(NesicaKey id)
{
	static NesicaKey gameId = id;
	CreateThread(nullptr, 0, TXAppCryptThread, &gameId, 0, nullptr);
}
#endif