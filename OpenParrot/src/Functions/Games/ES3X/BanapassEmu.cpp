﻿#include <filesystem>
#include <thread>
#include <chrono>
#include <StdInc.h>
#include "Functions/Types.h"
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#define BANA_API_VERSION "Ver 1.6.0"
#pragma optimize("",off)
// ORIGINAL CODE FORKED FROM: https://github.com/shiibe/OpenBanapass/tree/taiko/OpenBanapass
// THANKS!

void randomHex(char str[], int length)
{
	//hexadecimal characters
	char hexCharacterTable[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	srand(time(NULL));
	int i;
	for (i = 0; i < length; i++)
	{
		str[i] = hexCharacterTable[rand() % 16];
	}
	str[length] = 0;
}

void randomNumberString(char str[], int length)
{
	char CharacterTable[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	srand(time(NULL));
	int i;
	for (i = 0; i < length; i++)
	{
		str[i] = CharacterTable[rand() % 10];
	}
	str[length] = 0;
}
bool readerActive = false;
bool isReading = false;

std::string getProfileString(LPCSTR name, LPCSTR key, LPCSTR def, LPCSTR filename) {
	char temp[1024];
	const DWORD result = GetPrivateProfileStringA(name, key, def, temp, sizeof(temp), filename);
	return std::string(temp, result);
}

void createCard() {
	return;
}

void StartAttachThread(long (*callback)(long, long, long*), long* someStructPtr) {
	// this is a really ugly hack, forgive me
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(100ms);

	callback(0, 0, someStructPtr);
}

void StartResetThread(long (*callback)(int, int, long*), long* someStructPtr) {
	// this is a really ugly hack, forgive me
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(100ms);

	callback(0, 0, someStructPtr);
}

void StartReqActionThread(void (*callback)(long, int, long*), long* someStructPtr) {
	// this is a really ugly hack, forgive me
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(100ms);

	callback(0, 0, someStructPtr);
}

void StartReadThread(void (*callback)(int, int, void*, void*), void* cardStuctPtr) {
	while (readerActive)
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(100ms);
		isReading = false;
		if (GetAsyncKeyState('C'))
		{
			isReading = true;
			// Raw card data and some other stuff, who cares
			unsigned char rawCardData[168] = {
				0x01, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x92, 0x2E, 0x58, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x7F, 0x5C, 0x97, 0x44, 0xF0, 0x88, 0x04, 0x00,
				0x43, 0x26, 0x2C, 0x33, 0x00, 0x04, 0x06, 0x10, 0x30, 0x30, 0x30, 0x30,
				0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
				0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
				0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30,
				0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
				0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
				0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x4E, 0x42, 0x47, 0x49, 0x43, 0x36, 0x00, 0x00, 0xFA, 0xE9, 0x69, 0x00,
				0xF6, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
			};

			const auto accessCode = config["Banapass"]["AccessCode"];
			const auto chipId = config["Banapass"]["Card ID"];
			//const auto accessCode = getProfileString("card", "accessCode", "30764352518498791337", ".\\card.ini");
			//const auto chipId = getProfileString("card", "chipId", "7F5C9744F111111143262C3300040610", ".\\card.ini");
#ifdef _DEBUG
			info("accessCode: %s", accessCode.c_str());
			info("chipId: %s", chipId.c_str());
#endif
			memcpy(rawCardData + 0x50, accessCode.c_str(), accessCode.size() + 1);
			memcpy(rawCardData + 0x2C, chipId.c_str(), chipId.size() + 1);

			//info(true, "Callback from read card");
			callback(0, 0, rawCardData, cardStuctPtr);
			readerActive = false;
		}
	}
}

ULONGLONG BngRwAttach(UINT a1, char* a2, int a3, int a4, long (*callback)(long, long, long*), long* some_struct_ptr) {
#ifdef _DEBUG
	info("BngRwAttach(%i, %s, %d, %d, %p, %p)\n", a1, a2, a3, a4, callback, some_struct_ptr);
#endif
	createCard();

	std::thread t(StartAttachThread, callback, some_struct_ptr);
	t.detach();
	return 1;
}

long BngRwInit() {
#ifdef _DEBUG
	info("BngRwInit()\n");
#endif
	return 0;
}

ULONGLONG BngRwReqSetLedPower() {
#ifdef _DEBUG
	info("BngRwSetLedPower()\n");
#endif
	return 0;
}

int BngRwDevReset(UINT a, long (*callback)(int, int, long*), long* some_struct_ptr) {
#ifdef _DEBUG
	info("BngRwDevReset(%i, %p, %p)\n", a, callback, some_struct_ptr);
#endif

	std::thread t(StartResetThread, callback, some_struct_ptr);
	t.detach();
	return 1;
}

ULONGLONG BngRwExReadMifareAllBlock() {
#ifdef _DEBUG
	info("BngRwExReadMifareAllBlock()\n");
#endif
	return 0xffffff9c;
}

// Finalise?
void BngRwFin() {
#ifdef _DEBUG
	info("BngRwFin()\n");
#endif
}

UINT BngRwGetFwVersion(UINT a) {
#ifdef _DEBUG
	info("BngRwGetFwVersion(%i)\n", a);
#endif
	return 0;
}

UINT BngRwGetStationID(UINT a) {
#ifdef _DEBUG
	info("BngRwGetStationID(%i)\n", a);
#endif
	return 0;
}

const char* BngRwGetVersion() {
#ifdef _DEBUG
	info("BngRwGetVersion()\n");
#endif
	return BANA_API_VERSION;
}

ULONGLONG BngRwIsCmdExec(UINT a) {
#ifdef _DEBUG
	info("BngRwIsCmdExec(%i)\n", a);
#endif
	return 0xFFFFFFFF;
}

UINT BngRwGetTotalRetryCount(UINT a) {
#ifdef _DEBUG
	info("BngRwGetTotalRetryCount(%i)\n", a);
#endif
	return 0;
}

int BngRwReqLed(UINT a, UINT b, ULONGLONG c, ULONGLONG d) {
#ifdef _DEBUG
	info("BngRwReqLed(%i, %i, %llu, %llu)\n", a, b, c, d);
	info("\a");
#endif
	return 1;
}

int BngRwReqAction(UINT a, UINT b, void (*callback)(long, int, long*), long* some_struct_ptr) {
#ifdef _DEBUG
	info("BngRwReqAction(%i, %i, %p, %p)\n", a, b, callback, some_struct_ptr);
#endif

	std::thread t(StartReqActionThread, callback, some_struct_ptr);
	t.detach();
	if (b == 0)
	{
		return 1;
	}
	return -1;
}

int BngRwReqAiccAuth(UINT a, int b, UINT c, int* d, ULONGLONG e, ULONGLONG f, ULONGLONG* g) {
#ifdef _DEBUG
	info("BngRwReqAiccAuth(%i, %d, %i, %p, %llu, %llu, %p)\n", a, b, c, d, e, f, g);
#endif
	return 1;
}

int BngRwReqBeep(UINT a, UINT b, ULONGLONG c, ULONGLONG d) {
#ifdef _DEBUG
	info("BngRwReqBeep(%i, %i, %llu, %llu)\n", a, b, c, d);
#endif
	return 1;
}

int BngRwReqCancel(UINT a) {
#ifdef _DEBUG
	info("BngRwReqCancel(%i)\n", a);
#endif
	if (7 < a)
	{
		return -100;
	}
	return 1;
}

int BngRwReqFwCleanup(UINT a, ULONGLONG b, ULONGLONG c) {
#ifdef _DEBUG
	info("BngRwReqFwCleanup(%i, %llu, %llu)\n", a, b, c);
#endif
	return 1;
}

int BngRwReqFwVersionup(UINT a, ULONGLONG b, ULONGLONG c, ULONGLONG d) {
#ifdef _DEBUG
	info("BngRwReqFwVersionup(%i, %llu, %llu, %llu)\n", a, b, c, d);
#endif
	return 1;
}

int BngRwReqLatchID(UINT a, ULONGLONG b, ULONGLONG c) {
#ifdef _DEBUG
	info("BngRwReqLatchId(%i, %llu, %llu)\n", a, b, c);
#endif
	if (a < 8)
	{
		return -100;
	}
	return 1;
}

int BngRwReqSendMailTo(UINT a, int b, UINT c, int* d,
	char* e, char* f, char* g, char* h, ULONGLONG i, ULONGLONG j) {
#ifdef _DEBUG
	info("BngRwReqSendMailTo(%i, %d, %i, %p, %s, %s, %s, %s, %llu, %llu)\n", a, b, c, d, e, f, g, h, i, j);
#endif
	if (7 < a)
	{
		return -100;
	}
	if (!e)
	{
		return -100;
	}
	return 1;
}

int BngRwReqSendUrlTo(UINT a, int b, UINT c, int* d,
	char* e, char* f, ULONGLONG g, ULONGLONG h) {
#ifdef _DEBUG
	info("BngRwReqSendUrlTo(%i, %d, %i, %p, %s, %s, %llu, %llu)\n", a, b, c, d, e, f, g, h);
#endif
	if (7 < a)
	{
		return -100;
	}
	if (!e)
	{
		return -100;
	}
	return 1;
}

int BngRwReqWaitTouch(UINT a, int maxIntSomehow, UINT c, void (*callback)(int, int, void*, void*), void* card_struct_ptr) {
#ifdef _DEBUG
	info("BngRwReqWaitTouch(%i, %d, %i, %p, %p)\n", a, maxIntSomehow, c, callback, card_struct_ptr);
#endif

	// Hack to make sure previous threads have exited
	readerActive = false;
	//using namespace std::chrono_literals;
	//std::this_thread::sleep_for(250ms);

	readerActive = true;

	std::thread t(StartReadThread, callback, card_struct_ptr);
	t.detach();
	if (isReading) {
		return 1;
	}
	else
	{
		return -1;
	}
}

void init_BanapassEmu()
{
	MH_Initialize();
	MH_CreateHookApi(L"bngrw.dll", "BngRwReqWaitTouch", BngRwReqWaitTouch, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwReqSendUrlTo", BngRwReqSendUrlTo, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwReqSendMailTo", BngRwReqSendMailTo, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwReqLatchID", BngRwReqLatchID, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwReqFwVersionup", BngRwReqFwVersionup, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwReqFwCleanup", BngRwReqFwCleanup, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwReqCancel", BngRwReqCancel, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwReqBeep", BngRwReqBeep, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwReqAiccAuth", BngRwReqAiccAuth, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwReqAction", BngRwReqAction, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwReqLed", BngRwReqLed, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwGetTotalRetryCount", BngRwGetTotalRetryCount, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwIsCmdExec", BngRwIsCmdExec, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwGetVersion", BngRwGetVersion, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwGetStationID", BngRwGetStationID, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwGetFwVersion", BngRwGetFwVersion, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwFin", BngRwFin, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwExReadMifareAllBlock", BngRwExReadMifareAllBlock, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwDevReset", BngRwDevReset, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwReqSetLedPower", BngRwReqSetLedPower, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwInit", BngRwInit, NULL);
	MH_CreateHookApi(L"bngrw.dll", "BngRwAttach", BngRwAttach, NULL);
	MH_EnableHook(MH_ALL_HOOKS);
}
#pragma optimize("", on)
