#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include <filesystem>
#include <iostream>
#include <cstdint>
#include <fstream>
#include "MinHook.h"
#include <Utility/Hooking.Patterns.h>
#include <chrono>
#include <thread>
#ifdef _M_AMD64
#pragma optimize("", off)
#pragma comment(lib, "Ws2_32.lib")

extern LPCSTR hookPort;
uintptr_t imageBasedxplus;
static unsigned char hasp_buffer[0xD40];
static bool isFreePlay;
static bool isEventMode2P;
static bool isEventMode4P;
const char *ipaddrdxplus;

// MUST DISABLE IC CARD, FFB MANUALLY N MT5DX+

// FOR FREEPLAY
unsigned char dxpterminalPackage1_Free[79] = {
	0x01, 0x04, 0x4B, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x02, 0x5A, 0x00, 0x2A, 0x12, 0x08, 0x12, 0x12, 0x0C,
	0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39, 0x39, 0x30, 0x30, 0x30, 0x32,
	0x18, 0x00, 0x30, 0x03, 0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00,
	0x20, 0x00, 0x52, 0x0B, 0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20,
	0x00, 0x28, 0x00, 0xEC, 0x72, 0x00, 0x41
};

unsigned char dxpterminalPackage2_Free[139] = {
	0x01, 0x04, 0x87, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x02, 0x5A, 0x00, 0x2A, 0x12, 0x08, 0x14, 0x12, 0x0C,
	0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39, 0x39, 0x30, 0x30, 0x30, 0x32,
	0x18, 0x00, 0x30, 0x03, 0x42, 0x3A, 0x08, 0x01, 0x10, 0x03, 0x18, 0x02,
	0x20, 0x02, 0x28, 0x04, 0x30, 0x01, 0x38, 0x01, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x02, 0x58, 0x60, 0x60, 0x60, 0x68, 0x60, 0x70, 0x60, 0x78, 0x60,
	0x80, 0x01, 0x60, 0x88, 0x01, 0x60, 0x90, 0x01, 0x60, 0x98, 0x01, 0x00,
	0xA0, 0x01, 0xE2, 0xBA, 0xAC, 0xD4, 0x05, 0xA8, 0x01, 0x04, 0xB0, 0x01,
	0x24, 0xB8, 0x01, 0x00, 0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00,
	0x20, 0x00, 0x52, 0x0B, 0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20,
	0x00, 0x28, 0x00, 0x99, 0x4E, 0xC6, 0x14
};

unsigned char dxpterminalPackage3_Free[79] = {
	0x01, 0x04, 0x4B, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x02, 0x5A, 0x00, 0x2A, 0x12, 0x08, 0x19, 0x12, 0x0C,
	0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39, 0x39, 0x30, 0x30, 0x30, 0x32,
	0x18, 0x00, 0x30, 0x03, 0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00,
	0x20, 0x00, 0x52, 0x0B, 0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20,
	0x00, 0x28, 0x00, 0x89, 0x93, 0x3A, 0x22
};

unsigned char dxpterminalPackage4_Free[139] = {
	0x01, 0x04, 0x87, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x02, 0x5A, 0x00, 0x2A, 0x12, 0x08, 0x2E, 0x12, 0x0C,
	0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39, 0x39, 0x30, 0x30, 0x30, 0x32,
	0x18, 0x00, 0x30, 0x03, 0x42, 0x3A, 0x08, 0x01, 0x10, 0x03, 0x18, 0x02,
	0x20, 0x02, 0x28, 0x04, 0x30, 0x01, 0x38, 0x01, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x02, 0x58, 0x60, 0x60, 0x60, 0x68, 0x60, 0x70, 0x60, 0x78, 0x60,
	0x80, 0x01, 0x60, 0x88, 0x01, 0x60, 0x90, 0x01, 0x60, 0x98, 0x01, 0x00,
	0xA0, 0x01, 0xF0, 0xBA, 0xAC, 0xD4, 0x05, 0xA8, 0x01, 0x04, 0xB0, 0x01,
	0x24, 0xB8, 0x01, 0x00, 0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00,
	0x20, 0x00, 0x52, 0x0B, 0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20,
	0x00, 0x28, 0x00, 0x55, 0x42, 0x47, 0xD5
};

unsigned char dxpterminalPackage5_Free[79] = {
	0x01, 0x04, 0x4B, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x02, 0x5A, 0x00, 0x2A, 0x12, 0x08, 0x2F, 0x12, 0x0C,
	0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39, 0x39, 0x30, 0x30, 0x30, 0x32,
	0x18, 0x00, 0x30, 0x03, 0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00,
	0x20, 0x00, 0x52, 0x0B, 0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20,
	0x00, 0x28, 0x00, 0x9C, 0xC9, 0xE0, 0x73
};

unsigned char dxpterminalPackage6_Free[139] = {
	0x01, 0x04, 0x87, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x02, 0x5A, 0x00, 0x2A, 0x12, 0x08, 0x6A, 0x12, 0x0C,
	0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39, 0x39, 0x30, 0x30, 0x30, 0x32,
	0x18, 0x00, 0x30, 0x03, 0x42, 0x3A, 0x08, 0x01, 0x10, 0x03, 0x18, 0x02,
	0x20, 0x02, 0x28, 0x04, 0x30, 0x01, 0x38, 0x01, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x02, 0x58, 0x60, 0x60, 0x60, 0x68, 0x60, 0x70, 0x60, 0x78, 0x60,
	0x80, 0x01, 0x60, 0x88, 0x01, 0x60, 0x90, 0x01, 0x60, 0x98, 0x01, 0x00,
	0xA0, 0x01, 0xF1, 0xBA, 0xAC, 0xD4, 0x05, 0xA8, 0x01, 0x04, 0xB0, 0x01,
	0x24, 0xB8, 0x01, 0x00, 0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00,
	0x20, 0x00, 0x52, 0x0B, 0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20,
	0x00, 0x28, 0x00, 0x26, 0xB7, 0x89, 0xD0
};

// FOR COIN ENTRY!
unsigned char dxpterminalPackage1_Coin[75] = {
	0x01, 0x04, 0x47, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x00, 0x2A, 0x12, 0x08, 0x0B, 0x12, 0x0C, 0x32, 0x37,
	0x32, 0x32, 0x31, 0x31, 0x39, 0x39, 0x30, 0x30, 0x30, 0x32, 0x18, 0x00,
	0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0B,
	0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0x09,
	0x06, 0x41, 0x0B
};

unsigned char dxpterminalPackage2_Coin[135] = {
	0x01, 0x04, 0x83, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x00, 0x2A, 0x12, 0x08, 0x39, 0x12, 0x0C, 0x32, 0x37,
	0x32, 0x32, 0x31, 0x31, 0x39, 0x39, 0x30, 0x30, 0x30, 0x32, 0x18, 0x00,
	0x42, 0x3A, 0x08, 0x01, 0x10, 0x03, 0x18, 0x02, 0x20, 0x02, 0x28, 0x04,
	0x30, 0x00, 0x38, 0x01, 0x40, 0x00, 0x48, 0x00, 0x50, 0x02, 0x58, 0x60,
	0x60, 0x60, 0x68, 0x60, 0x70, 0x60, 0x78, 0x60, 0x80, 0x01, 0x60, 0x88,
	0x01, 0x60, 0x90, 0x01, 0x60, 0x98, 0x01, 0x00, 0xA0, 0x01, 0xD5, 0xBE,
	0x8F, 0xD2, 0x05, 0xA8, 0x01, 0x04, 0xB0, 0x01, 0x24, 0xB8, 0x01, 0x00,
	0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0B,
	0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0xF5,
	0xF1, 0x0D, 0xB2
};

unsigned char dxpterminalPackage3_Coin[75] = {
	0x01, 0x04, 0x47, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x00, 0x2A, 0x12, 0x08, 0x3A, 0x12, 0x0C, 0x32, 0x37,
	0x32, 0x32, 0x31, 0x31, 0x39, 0x39, 0x30, 0x30, 0x30, 0x32, 0x18, 0x00,
	0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0B,
	0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0x22,
	0x25, 0x31, 0x0D
};

unsigned char dxpterminalPackage4_Coin[135] = {
	0x01, 0x04, 0x83, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x00, 0x2A, 0x12, 0x08, 0x57, 0x12, 0x0C, 0x32, 0x37,
	0x32, 0x32, 0x31, 0x31, 0x39, 0x39, 0x30, 0x30, 0x30, 0x32, 0x18, 0x00,
	0x42, 0x3A, 0x08, 0x01, 0x10, 0x03, 0x18, 0x02, 0x20, 0x02, 0x28, 0x04,
	0x30, 0x00, 0x38, 0x01, 0x40, 0x00, 0x48, 0x00, 0x50, 0x02, 0x58, 0x60,
	0x60, 0x60, 0x68, 0x60, 0x70, 0x60, 0x78, 0x60, 0x80, 0x01, 0x60, 0x88,
	0x01, 0x60, 0x90, 0x01, 0x60, 0x98, 0x01, 0x00, 0xA0, 0x01, 0xD6, 0xBE,
	0x8F, 0xD2, 0x05, 0xA8, 0x01, 0x04, 0xB0, 0x01, 0x24, 0xB8, 0x01, 0x00,
	0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0B,
	0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0xCA,
	0x8B, 0x15, 0xCB
};

unsigned char dxpterminalPackage5_Coin[79] = {
	0x01, 0x04, 0x4B, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x02, 0x5A, 0x00, 0x2A, 0x12, 0x08, 0x58, 0x12, 0x0C,
	0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39, 0x39, 0x30, 0x30, 0x30, 0x32,
	0x18, 0x00, 0x30, 0x03, 0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00,
	0x20, 0x00, 0x52, 0x0B, 0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20,
	0x00, 0x28, 0x00, 0x3E, 0xB1, 0xB7, 0x22
};

unsigned char dxpterminalPackage6_Coin[139] = {
	0x01, 0x04, 0x87, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x02, 0x5A, 0x00, 0x2A, 0x12, 0x08, 0x77, 0x12, 0x0C,
	0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39, 0x39, 0x30, 0x30, 0x30, 0x32,
	0x18, 0x00, 0x30, 0x03, 0x42, 0x3A, 0x08, 0x01, 0x10, 0x03, 0x18, 0x02,
	0x20, 0x02, 0x28, 0x04, 0x30, 0x00, 0x38, 0x01, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x02, 0x58, 0x60, 0x60, 0x60, 0x68, 0x60, 0x70, 0x60, 0x78, 0x60,
	0x80, 0x01, 0x60, 0x88, 0x01, 0x60, 0x90, 0x01, 0x60, 0x98, 0x01, 0x00,
	0xA0, 0x01, 0xD7, 0xBE, 0x8F, 0xD2, 0x05, 0xA8, 0x01, 0x04, 0xB0, 0x01,
	0x24, 0xB8, 0x01, 0x00, 0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00,
	0x20, 0x00, 0x52, 0x0B, 0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20,
	0x00, 0x28, 0x00, 0xBD, 0x07, 0xCF, 0xDC
};

//Event mode 2P
unsigned char dxpterminalPackage1_Event4P[79] = {
	0x01, 0x04, 0x44, 0x00, 0x12, 0x0e, 0x0a, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x1a, 0x00, 0x2a, 0x13,
	0x08, 0xd1, 0x0b, 0x12, 0x0c, 0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39,
	0x39, 0x30, 0x30, 0x30, 0x32, 0x18, 0x00, 0x30, 0x00, 0x4a, 0x08, 0x08, 
	0x03, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0b, 0x08, 0x64, 0x10,
	0xde, 0x0f, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0xc1, 0x96, 0xc9, 0x2e
};

unsigned char dxpterminalPackage2_Event4P[139] = {
	0x01, 0x04, 0x80, 0x00, 0x12, 0x0e, 0x0a, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x1a, 0x00, 0x2a, 0x13,
	0x08, 0xd2, 0x0b, 0x12, 0x0c, 0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39,
	0x39, 0x30, 0x30, 0x30, 0x32, 0x18, 0x00, 0x30, 0x00, 0x42, 0x3a, 0x08,
	0x01, 0x10, 0x03, 0x18, 0x02, 0x20, 0x02, 0x28, 0x04, 0x30, 0x01, 0x38,
	0x01, 0x40, 0x01, 0x48, 0x00, 0x50, 0x02, 0x58, 0x60, 0x60, 0x60, 0x68,
	0x60, 0x70, 0x60, 0x78, 0x60, 0x80, 0x01, 0x60, 0x88, 0x01, 0x60, 0x90,
	0x01, 0x60, 0x98, 0x01, 0x00, 0xa0, 0x01, 0xd8, 0xc3, 0xd6, 0xe1, 0x05,
	0xa8, 0x01, 0x04, 0xb0, 0x01, 0x24, 0xb8, 0x01, 0x00, 0x4a, 0x08, 0x08,
	0x03, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0b, 0x08, 0x64, 0x10,
	0xde, 0x0f, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0x91, 0x74, 0xca, 0x1e

};

unsigned char dxpterminalPackage3_Event4P[79] = {
	0x01, 0x04, 0x44, 0x00, 0x12, 0x0e, 0x0a, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x1a, 0x00, 0x2a, 0x13,
	0x08, 0x8d, 0x0c, 0x12, 0x0c, 0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39,
	0x39, 0x30, 0x30, 0x30, 0x32, 0x18, 0x00, 0x30, 0x00, 0x4a, 0x08, 0x08,
	0x03, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0b, 0x08, 0x64, 0x10,
	0xde, 0x0f, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0x86, 0xb1, 0x27, 0x9e
};

unsigned char dxpterminalPackage4_Event4P[139] = {
	0x01, 0x04, 0x80, 0x00, 0x12, 0x0e, 0x0a, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x1a, 0x00, 0x2a, 0x13,
	0x08, 0x8e, 0x0c, 0x12, 0x0c, 0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39,
	0x39, 0x30, 0x30, 0x30, 0x32, 0x18, 0x00, 0x30, 0x00, 0x42, 0x3a, 0x08,
	0x01, 0x10, 0x03, 0x18, 0x02, 0x20, 0x02, 0x28, 0x04, 0x30, 0x01, 0x38,
	0x01, 0x40, 0x01, 0x48, 0x00, 0x50, 0x02, 0x58, 0x60, 0x60, 0x60, 0x68,
	0x60, 0x70, 0x60, 0x78, 0x60, 0x80, 0x01, 0x60, 0x88, 0x01, 0x60, 0x90,
	0x01, 0x60, 0x98, 0x01, 0x00, 0xa0, 0x01, 0xd9, 0xc3, 0xd6, 0xe1, 0x05,
	0xa8, 0x01, 0x04, 0xb0, 0x01, 0x24, 0xb8, 0x01, 0x00, 0x4a, 0x08, 0x08,
	0x03, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0b, 0x08, 0x64, 0x10,
	0xde, 0x0f, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0xc2, 0x11, 0x2a, 0x66

};

unsigned char dxpterminalPackage5_Event4P[79] = {
	0x01, 0x04, 0x44, 0x00, 0x12, 0x0e, 0x0a, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x1a, 0x00, 0x2a, 0x13,
	0x08, 0xc9, 0x0c, 0x12, 0x0c, 0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39,
	0x39, 0x30, 0x30, 0x30, 0x32, 0x18, 0x00, 0x30, 0x00, 0x4a, 0x08, 0x08,
	0x03, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0b, 0x08, 0x64, 0x10,
	0xde, 0x0f, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0x5d, 0x49, 0x01, 0x1e
};

unsigned char dxpterminalPackage6_Event4P[139] = {
	0x01, 0x04, 0x80, 0x00, 0x12, 0x0e, 0x0a, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x1a, 0x00, 0x2a, 0x13,
	0x08, 0xca, 0x0c, 0x12, 0x0c, 0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39,
	0x39, 0x30, 0x30, 0x30, 0x32, 0x18, 0x00, 0x30, 0x00, 0x42, 0x3a, 0x08,
	0x01, 0x10, 0x03, 0x18, 0x02, 0x20, 0x02, 0x28, 0x04, 0x30, 0x01, 0x38,
	0x01, 0x40, 0x01, 0x48, 0x00, 0x50, 0x02, 0x58, 0x60, 0x60, 0x60, 0x68,
	0x60, 0x70, 0x60, 0x78, 0x60, 0x80, 0x01, 0x60, 0x88, 0x01, 0x60, 0x90,
	0x01, 0x60, 0x98, 0x01, 0x00, 0xa0, 0x01, 0xda, 0xc3, 0xd6, 0xe1, 0x05,
	0xa8, 0x01, 0x04, 0xb0, 0x01, 0x24, 0xb8, 0x01, 0x00, 0x4a, 0x08, 0x08,
	0x03, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0b, 0x08, 0x64, 0x10,
	0xde, 0x0f, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0xd4, 0x80, 0x16, 0xc2
};


//Event mode 2P
unsigned char dxpterminalPackage1_Event2P[79] = {
	0x01, 0x04, 0x44, 0x00, 0x12, 0x0e, 0x0a, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x1a, 0x00, 0x2a, 0x13,
	0x08, 0xfe, 0x0e, 0x12, 0x0c, 0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39,
	0x39, 0x30, 0x30, 0x30, 0x32, 0x18, 0x00, 0x30, 0x00, 0x4a, 0x08, 0x08,
	0x03, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0b, 0x08, 0x64, 0x10,
	0xde, 0x0f, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0xaf, 0xa1, 0x42, 0x3d
};

unsigned char dxpterminalPackage2_Event2P[139] = {
	0x01, 0x04, 0x80, 0x00, 0x12, 0x0e, 0x0a, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x1a, 0x00, 0x2a, 0x13,
	0x08, 0xff, 0x0e, 0x12, 0x0c, 0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39,
	0x39, 0x30, 0x30, 0x30, 0x32, 0x18, 0x00, 0x30, 0x00, 0x42, 0x3a, 0x08,
	0x01, 0x10, 0x03, 0x18, 0x02, 0x20, 0x02, 0x28, 0x04, 0x30, 0x01, 0x38,
	0x01, 0x40, 0x02, 0x48, 0x00, 0x50, 0x02, 0x58, 0x60, 0x60, 0x60, 0x68,
	0x60, 0x70, 0x60, 0x78, 0x60, 0x80, 0x01, 0x60, 0x88, 0x01, 0x60, 0x90,
	0x01, 0x60, 0x98, 0x01, 0x00, 0xa0, 0x01, 0xa7, 0xc2, 0xa5, 0xe1, 0x05,
	0xa8, 0x01, 0x02, 0xb0, 0x01, 0x24, 0xb8, 0x01, 0x00, 0x4a, 0x08, 0x08,
	0x03, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0b, 0x08, 0x64, 0x10,
	0xde, 0x0f, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0xe8, 0x94, 0x41, 0x46

};

unsigned char dxpterminalPackage3_Event2P[79] = {
	0x01, 0x04, 0x44, 0x00, 0x12, 0x0e, 0x0a, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x1a, 0x00, 0x2a, 0x13,
	0x08, 0x80, 0x0f, 0x12, 0x0c, 0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39,
	0x39, 0x30, 0x30, 0x30, 0x32, 0x18, 0x00, 0x30, 0x00, 0x4a, 0x08, 0x08,
	0x03, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0b, 0x08, 0x64, 0x10,
	0xde, 0x0f, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0xa3, 0x94, 0x12, 0x9b
};

unsigned char dxpterminalPackage4_Event2P[139] = {
	0x01, 0x04, 0x80, 0x00, 0x12, 0x0e, 0x0a, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x1a, 0x00, 0x2a, 0x13,
	0x08, 0x8d, 0x0f, 0x12, 0x0c, 0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39,
	0x39, 0x30, 0x30, 0x30, 0x32, 0x18, 0x00, 0x30, 0x00, 0x42, 0x3a, 0x08,
	0x01, 0x10, 0x03, 0x18, 0x02, 0x20, 0x02, 0x28, 0x04, 0x30, 0x01, 0x38,
	0x01, 0x40, 0x02, 0x48, 0x00, 0x50, 0x02, 0x58, 0x60, 0x60, 0x60, 0x68,
	0x60, 0x70, 0x60, 0x78, 0x60, 0x80, 0x01, 0x60, 0x88, 0x01, 0x60, 0x90,
	0x01, 0x60, 0x98, 0x01, 0x00, 0xa0, 0x01, 0xa8, 0xc2, 0xa5, 0xe1, 0x05,
	0xa8, 0x01, 0x02, 0xb0, 0x01, 0x24, 0xb8, 0x01, 0x00, 0x4a, 0x08, 0x08,
	0x03, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0b, 0x08, 0x64, 0x10,
	0xde, 0x0f, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0x8b, 0x02, 0xdf, 0xad

};

unsigned char dxpterminalPackage5_Event2P[79] = {
	0x01, 0x04, 0x44, 0x00, 0x12, 0x0e, 0x0a, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x1a, 0x00, 0x2a, 0x13,
	0x08, 0x8e, 0x0f, 0x12, 0x0c, 0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39,
	0x39, 0x30, 0x30, 0x30, 0x32, 0x18, 0x00, 0x30, 0x00, 0x4a, 0x08, 0x08,
	0x03, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0b, 0x08, 0x64, 0x10,
	0xde, 0x0f, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0xa3, 0xc2, 0x27, 0x9c
};

unsigned char dxpterminalPackage6_Event2P[139] = {
	0x01, 0x04, 0x80, 0x00, 0x12, 0x0e, 0x0a, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x1a, 0x00, 0x2a, 0x13,
	0x08, 0xf0, 0x0e, 0x12, 0x0c, 0x32, 0x37, 0x32, 0x32, 0x31, 0x31, 0x39,
	0x39, 0x30, 0x30, 0x30, 0x32, 0x18, 0x00, 0x30, 0x00, 0x42, 0x3a, 0x08,
	0x01, 0x10, 0x03, 0x18, 0x02, 0x20, 0x02, 0x28, 0x04, 0x30, 0x01, 0x38,
	0x01, 0x40, 0x02, 0x48, 0x00, 0x50, 0x02, 0x58, 0x60, 0x60, 0x60, 0x68,
	0x60, 0x70, 0x60, 0x78, 0x60, 0x80, 0x01, 0x60, 0x88, 0x01, 0x60, 0x90,
	0x01, 0x60, 0x98, 0x01, 0x00, 0xa0, 0x01, 0xa6, 0xc2, 0xa5, 0xe1, 0x05,
	0xa8, 0x01, 0x02, 0xb0, 0x01, 0x24, 0xb8, 0x01, 0x00, 0x4a, 0x08, 0x08,
	0x03, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0b, 0x08, 0x64, 0x10,
	0xde, 0x0f, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0x97, 0xd5, 0x79, 0xa6
};


#define HASP_STATUS_OK 0
unsigned int dxpHook_hasp_login(int feature_id, void* vendor_code, int hasp_handle) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_login\n");
#endif
	return HASP_STATUS_OK;
}

unsigned int dxpHook_hasp_logout(int hasp_handle) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_logout\n");
#endif
	return HASP_STATUS_OK;
}

unsigned int dxpHook_hasp_encrypt(int hasp_handle, unsigned char* buffer, unsigned int buffer_size) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_encrypt\n");
#endif
	return HASP_STATUS_OK;
}

unsigned int dxpHook_hasp_decrypt(int hasp_handle, unsigned char* buffer, unsigned int buffer_size) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_decrypt\n");
#endif
	return HASP_STATUS_OK;
}

unsigned int dxpHook_hasp_get_size(int hasp_handle, int hasp_fileid, unsigned int* hasp_size) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_get_size\n");
#endif
	*hasp_size = 0xD40; // Max addressable size by the game... absmax is 4k
	return HASP_STATUS_OK;
}

unsigned int dxpHook_hasp_read(int hasp_handle, int hasp_fileid, unsigned int offset, unsigned int length, unsigned char* buffer) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_read\n");
#endif
	memcpy(buffer, hasp_buffer + offset, length);
	return HASP_STATUS_OK;
}

unsigned int dxpHook_hasp_write(int hasp_handle, int hasp_fileid, unsigned int offset, unsigned int length, unsigned char* buffer) {
	return HASP_STATUS_OK;
}

// Save data dump memory block
unsigned char saveDatadxp[0x2000];

// Mile data dump memory block
unsigned char mileDatadxp[0x08];

//set system date patch by pockywitch
typedef bool (WINAPI* SETSYSTEMTIME)(SYSTEMTIME* in);
SETSYSTEMTIME pSetSystemTime = NULL;

bool WINAPI Hook_SetSystemTime(SYSTEMTIME* in)
{
	return TRUE;
}

// BASE: 0x24E0 
// Campaing honor data: 2998, save 0xB8
// Story Mode Honor data: 25F0, save 0x98
// StoryModeNoLoseHonorData: 2C80, Copy 0,0x10, Copy 0x18,0x28 maybe 8 bytes more
// OtherHonorData: 2A90, Copy 0x60
// CampaignHonorData: 2698, Copy 0x48

//static int SaveCampaingHonorData2()
//{
//	memset(saveData, 0, 0x1000);
//	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
//	uintptr_t value = *(uintptr_t*)(imageBase + 0x1948F10);
//	value += 0x2698;
//	FILE* file = fopen(V("CampaignHonorData.sav"), V("wb"));
//	memcpy(saveData, (void *)value, 0x48);
//	fwrite(saveData, 1, 0x100, file);
//	fclose(file);
//	return 1;
//}
//
//static int SaveOtherHonorData()
//{
//	memset(saveData, 0, 0x1000);
//	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
//	uintptr_t value = *(uintptr_t*)(imageBase + 0x1948F10);
//	value += 0x2A90;
//	FILE* file = fopen(V("OtherHonorData.sav"), V("wb"));
//	memcpy(saveData, (void *)value, 0x60);
//	fwrite(saveData, 1, 0x100, file);
//	fclose(file);
//	return 1;
//}
//
//static int SaveStoryModeNoLoseHonorData()
//{
//	memset(saveData, 0, 0x1000);
//	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
//	uintptr_t value = *(uintptr_t*)(imageBase + 0x1948F10);
//	value += 0x2C80;
//	FILE* file = fopen(V("StoryModeNoLoseHonorData.sav"), V("wb"));
//	memcpy(saveData, (void *)value, 0x10);
//	value += 0x18;
//	memcpy(saveData, (void *)value, 0x28);
//	fwrite(saveData, 1, 0x100, file);
//	fclose(file);
//	return 1;
//}
//
//static int SaveCampaingHonorData()
//{
//	memset(saveData, 0, 0x1000);
//	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
//	uintptr_t value = *(uintptr_t*)(imageBase + 0x1948F10);
//	value += 0x2998;
//	FILE* file = fopen(V("campaing.sav"), V("wb"));
//	memcpy(saveData, (void *)value, 0xB8);
//	fwrite(saveData, 1, 0x100, file);
//	fclose(file);
//	return 1;
//}
//
//static int SaveStoryData()
//{
//	memset(saveData, 0, 0x1000);
//	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
//	uintptr_t value = *(uintptr_t*)(imageBase + 0x1948F10);
//	value += 0x25F0;
//	FILE* file = fopen(V("story.sav"), V("wb"));
//	memcpy(saveData, (void *)value, 0x98);
//	fwrite(saveData, 1, 0x100, file);
//	fclose(file);
//	return 1;
//}

// setFullTune(void): Int
// If the currently loaded car is NOT fully tuned, 
// updates the power and handling values to be fully
// tuned (16 for each). If they are already fully tuned,
// does not change any values.
static int setFullTune()
{
	// Get the memory addresses for the car base save, power and handling values
	auto carSaveBase = (uintptr_t*)(*(uintptr_t*)(imageBasedxplus + 0x01F7D578) + 0x268);
	auto powerAddress = (uintptr_t*)(*(uintptr_t*)(carSaveBase)+0xAC);
	auto handleAddress = (uintptr_t*)(*(uintptr_t*)(carSaveBase)+0xB8);

	// Dereference the power value from the memory address
	auto powerValue = injector::ReadMemory<uint8_t>(powerAddress, true);
	auto handleValue = injector::ReadMemory<uint8_t>(handleAddress, true);

	// If the power and handling values do not add up to fully tuned
	if (powerValue + handleValue < 0x20)
	{
		// Car is not fully tuned, force it to the default full tune
		injector::WriteMemory<uint8_t>(powerAddress, 0x10, true);
		injector::WriteMemory<uint8_t>(handleAddress, 0x10, true);

		// Updated
		return 1;
	}

	// Not updated
	return 0;
}

// forceFullTune(pArguments: void*): DWORD WINAPI
// Function which runs in a secondary thread if the forceFullTune
// option is selected in the menu. If the player's car is not fully
// tuned, it is forcibly set to max tune. If the player's car is already
// fully tuned, it is left alone. 
static DWORD WINAPI forceFullTune(void* pArguments)
{
	// Loops while the program is running
	while (true) {

		// Only runs every 16th frame
		Sleep(16);

		// Run the set full tune process
		setFullTune();
	}
}

// ******************************************** //
// ************ Debug Data Logging ************ //
// ******************************************** //

// ************* Global Variables ************* //

// **** String Variables

// Debugging event log file
std::string logfileDxp = "wmmt5dxp_errors.txt";

// writeLog(filename: String, message: String): Int
// Given a filename string and a message string, appends
// the message to the given file.
static int writeLog(std::string filename, std::string message)
{
	// Log file to write to
	std::ofstream eventLog;

	// Open the filename provided (append mode)
	eventLog.open(filename, std::ios_base::app);

	// File open success
	if (eventLog.is_open()) 
	{
		// Write the message to the file
		eventLog << message;

		// Close the log file handle
		eventLog.close();

		// Success
		return 0;
	}
	else // File open failed
	{
		// Failure
		return 1;
	}
}

// writeDump(filename: Char*, data: unsigned char *, size: size_t): Int
static int writeDump(char * filename, unsigned char * data, size_t size)
{
	// Open the file with the provided filename
	FILE* file = fopen(filename, "wb");

	// File opened successfully
	if (file)
	{
		// Write the data to the file
		fwrite((void*)data, 1, size, file);

		// Close the file
		fclose(file);

		// Return success status
		return 0;
	}
	else // Failed to open
	{
		// Return failure status
		return 1;
	}
}

// Sets if saving is allowed or not
static bool saveOk = false;

// If custom car is used
bool customCarDxp = false;

// Sets if loading is allowed
bool loadOkDxp = false;

// Car save data reserved memory
unsigned char carDataDxp[0xFF];

// Car filename string
char carFileNameDxp[0xFF];

// SaveOk(void): Void
// Enables saving
static int SaveOk()
{
	saveOk = true;
	return 1;
}

// loadCarFile(filename: char*): Int
// Given a filename, loads the data from
// the car file into memory. 
static int loadCarFile(char* filename)
{
	// Open the file with the filename
	FILE* file = fopen(filename, "rb");

	// File open OK
	if (file)
	{
		// Get the length of the file
		fseek(file, 0, SEEK_END);
		int fsize = ftell(file);

		// If the file has the right size
		if (fsize == 0xFF)
		{
			// Reset to start of the file and read it into the car data variable
			fseek(file, 0, SEEK_SET);
			fread(carDataDxp, fsize, 1, file);

			// Dereference the memory location for the car save data
			uintptr_t carSaveLocation = *(uintptr_t*)((*(uintptr_t*)(imageBasedxplus + 0x1F7D578)) + 0x268);

			// memcpy((void*)(carSaveLocation + 0x00), carDataDxp + 0x00, 8); // ??
			// memcpy((void*)(carSaveLocation + 0x08), carDataDxp + 0x08, 8); // ??
			// memcpy((void*)(carSaveLocation + 0x10), carDataDxp + 0x10, 8); // ??
			// memcpy((void*)(carSaveLocation + 0x18), carDataDxp + 0x18, 8); // ??
			// memcpy((void*)(carSaveLocation + 0x20), carDataDxp + 0x20, 8); // Crash
			memcpy((void*)(carSaveLocation + 0x28), carDataDxp + 0x28, 8); // Region (0x28)
			memcpy((void*)(carSaveLocation + 0x30), carDataDxp + 0x30, 8); // CarID (0x34)
			// memcpy((void*)(carSaveLocation + 0x38), carDataDxp + 0x38, 4); // defaultColor
			memcpy((void*)(carSaveLocation + 0x3C), carDataDxp + 0x3C, 4); // CustomColor (0x3C)
			memcpy((void*)(carSaveLocation + 0x40), carDataDxp + 0x40, 8); // Rims (0x40), Rims Colour (0x44)
			memcpy((void*)(carSaveLocation + 0x48), carDataDxp + 0x48, 8); // Aero (0x48), Hood (0x4C)
			// memcpy((void*)(carSaveLocation + 0x50), carDataDxp + 0x50, 8); // Crash
			memcpy((void*)(carSaveLocation + 0x58), carDataDxp + 0x58, 8); // Wing (0x58), Mirror (0x5C)
			memcpy((void*)(carSaveLocation + 0x60), carDataDxp + 0x60, 8); // Sticker (0x60), Sticker Type (0x64)
			// memcpy((void*)(carSaveLocation + 0x68), carDataDxp + 0x60, 8); // Crash
			memcpy((void*)(carSaveLocation + 0x88), carDataDxp + 0x88, 8); // Roof Sticker (0x88), Roof Sticker Type (0x8C)
			memcpy((void*)(carSaveLocation + 0x90), carDataDxp + 0x90, 8); // Neon (0x90), Trunk (0x94)
			memcpy((void*)(carSaveLocation + 0x98), carDataDxp + 0x98, 8); // Plate Frame (0x98)
			memcpy((void*)(carSaveLocation + 0xA0), carDataDxp + 0xA0, 8); // Plate Number (0xA0), vinyl_body_challenge_prefecture_1~15 (0xA4)
			memcpy((void*)(carSaveLocation + 0xA8), carDataDxp + 0xA8, 8); // vinyl_body_challenge_prefecture (0xA8), Power (0xAC)
			memcpy((void*)(carSaveLocation + 0xB8), carDataDxp + 0xB8, 8); // Handling (0xB8), Rank (0xBC)
			// memcpy((void*)(carSaveLocation + 0xC0), carDataDxp + 0xC0, 8); // ??
			// memcpy((void*)(carSaveLocation + 0xC8), carDataDxp + 0xC8, 8); // ??
			// memcpy((void*)(carSaveLocation + 0xD0), carDataDxp + 0xD0, 8); // Crash
			// memcpy((void*)(carSaveLocation + 0xD8), carDataDxp + 0xD8, 8); // ??
			// memcpy((void*)(carSaveLocation + 0xE0), carDataDxp + 0xE0, 8); // ??
			// memcpy((void*)(carSaveLocation + 0xE8), carDataDxp + 0xE8, 8); // ??
			// memcpy((void*)(carSaveLocation + 0xF0), carDataDxp + 0xF0, 8); // ??
			// memcpy((void*)(carSaveLocation + 0xF8), carDataDxp + 0xF8, 8); // ??
		}

		// Disable loading
		loadOkDxp = false;

		// Close the file
		fclose(file);

		// Success
		return  1;
	}

	// Failed
	return 0;
} 

// loadCarData(filepath: char*): Void
// Given a filepath, attempts to load a 
// car file (either custom.car or specific
// car file) from that folder.
static int loadCarData(char * filepath)
{
	// Sleep for 1 second
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	// Custom car disabled by default
	customCarDxp = false;

	// Miles path string
	char carpath[0xFF];

	// Set the milepath memory to zero
	memset(carpath, 0, 0xFF);

	// Copy the file path to the miles path
	strcpy(carpath, filepath);

	// Append the mileage filename to the string
	strcat(carpath, "\\OpenParrot_Cars");

	// Create the OpenParrot_cars directory at the given filepath
	std::filesystem::create_directories(carpath);

	// Get the path to the custom car file
	sprintf(carFileNameDxp, "%s\\custom.car", carpath);

	// If the custom car file exists
	if (FileExists(carFileNameDxp))
	{
		// Load the custom car file
		loadCarFile(carFileNameDxp);

		// Enable custom car switch
		customCarDxp = true;
	}

	// Empty the car filename string
	memset(carFileNameDxp, 0, 0xFF);

	// Get the path to the specific car file
	sprintf(carFileNameDxp, "%s\\%08X.car", carpath, *(DWORD*)(*(uintptr_t*)(*(uintptr_t*)(imageBasedxplus + 0x1F7D578) + 0x268) + 0x34));
	
	// If the specific car file exists
	if (FileExists(carFileNameDxp))
	{
		// Load the car file
		loadCarFile(carFileNameDxp);
	}

	// If the force full tune switch is set
	if (ToBool(config["Tune"]["Force Full Tune"]))
	{
		// Create the force full tune thread
		// CreateThread(0, 0, forceFullTune, 0, 0, 0);

		// Set the car to be fully tuned
		setFullTune();
	}

	// Success
	return 1;
}

static int saveCarData(char* filepath)
{
	// Car Profile saving
	memset(carDataDxp, 0, 0xFF);
	memset(carFileNameDxp, 0, 0xFF);

	// Address where player save data starts
	uintptr_t saveDataBase = *(uintptr_t*)(imageBasedxplus + 0x1F7D578);

	// Address where car save data starts
	uintptr_t carSaveBase = *(uintptr_t*)(saveDataBase + 0x268);

	// Miles path string
	char carpath[0xFF];

	// Set the milepath memory to zero
	memset(carpath, 0, 0xFF);

	// Copy the file path to the miles path
	strcpy(carpath, filepath);

	// Append the mileage filename to the string
	strcat(carpath, "\\OpenParrot_Cars");

	// CreateDirectoryA(carpath, nullptr);

	// Create the cars path folder
	std::filesystem::create_directories(carpath);

	// Copy the 0xFF bytes from memory to the car data array
	memcpy(carDataDxp + 0x00, (void*)(carSaveBase + 0x0), 0xFF);

	// If custom car is set
	if (customCarDxp)
	{
		// Save the file to custom.car
		sprintf(carpath, "%s\\custom.car", carpath);
	}
	else // Custom car is not set
	{
		// Save the file to the specific car filename
		sprintf(carpath, "%s\\%08X.car", carpath, *(DWORD*)(*(uintptr_t*)(*(uintptr_t*)(imageBasedxplus + 0x1F7D578) + 0x268) + 0x34));
	}

	// Open the file at the given car path
	FILE* carFile = fopen(carpath, "wb");

	// Write the data from the array to the file
	fwrite(carDataDxp, 1, 0xFF, carFile);
	fclose(carFile);

	// Success
	return 1;

	/*
	
	// Legacy MT5 car saving code reference

	memset(saveData, 0, 0x2000);
	uintptr_t value = *(uintptr_t*)(imageBase + 0x1948F10);
	value = *(uintptr_t*)(value + 0x108);
	memcpy(saveData, (void *)value, 0x340);
	FILE* file = fopen("openprogress.sav", "wb");
	fwrite(saveData, 1, 0x2000, file);
	fclose(file);
	
	// Car Profile saving
	memset(carData, 0, 0xFF);
	memset(carFileNamedxp, 0, 0xFF);
	memcpy(carData, (void *)*(uintptr_t*)(*(uintptr_t*)(imageBase + 0x1948F10) + 0x180 + 0xa8 + 0x18), 0xE0);
	CreateDirectoryA("OpenParrot_Cars", nullptr);
	
	if(customCardxp)
	{
		sprintf(carFileNamedxp, ".\\OpenParrot_Cars\\custom.car");
	}
	else
	{
		sprintf(carFileNamedxp, ".\\OpenParrot_Cars\\%08X.car", *(DWORD*)(*(uintptr_t*)(*(uintptr_t*)(imageBase + 0x1948F10) + 0x180 + 0xa8 + 0x18) + 0x2C));
	}
	
	FILE *carSave = fopen(carFileNamedxp, "wb");
	fwrite(carData, 1, 0xE0, file);
	fclose(carSave);
	
	//SaveStoryData();
	//SaveCampaingHonorData();
	//SaveStoryModeNoLoseHonorData();
	//SaveOtherHonorData();
	//SaveCampaingHonorData2();
	
	saveOk = false;
	return 1;
	*/
}

// loadStoryData(filepath: char *): Void
// Given a filepath, loads the story data 
// from the file into memory.
static int loadStoryData(char* filepath)
{
	// Zero out the save data array
	memset(saveDatadxp, 0x0, 0x2000);

	// Miles path string
	char storyPath[0xFF];

	// Set the milepath memory to zero
	memset(storyPath, 0, 0xFF);

	// Copy the file path to the miles path
	strcpy(storyPath, filepath);

	// Append the mileage filename to the string
	strcat(storyPath, "\\openprogress.sav");

	// Address where player save data starts
	uintptr_t saveDataBase = *(uintptr_t*)(imageBasedxplus + 0x1F7D578);

	// Open the openprogress file with read privileges	
	FILE* file = fopen(storyPath, "rb");

	// If the file exists
	if (file)
	{
		// Get all of the contents from the file
		fseek(file, 0, SEEK_END);

		// Get the size of the file
		int fsize = ftell(file);

		// Check file is correct size
		if (fsize == 0x2000)
		{
			// Reset seek index to start
			fseek(file, 0, SEEK_SET);

			// Read all of the contents of the file into saveDatadxp
			fread(saveDatadxp, fsize, 1, file);

			// Story save data offset
			uintptr_t saveStoryBase = *(uintptr_t*)(saveDataBase + 0x108);

			// 0x00 - 08 4C - Should be able to use this to figure out what game a save is from

			// (Mostly) discovered story data

			memcpy((void*)(saveStoryBase + 0x48), saveDatadxp + 0x48, 0x8); // Story Bit
			memcpy((void*)(saveStoryBase + 0xE0), saveDatadxp + 0xE0, 0x8); // ??
			memcpy((void*)(saveStoryBase + 0xE8), saveDatadxp + 0xE8, 0x8); // Chapter Progress (0xE8) (Bitmask)
			memcpy((void*)(saveStoryBase + 0xF0), saveDatadxp + 0xF0, 0x8); // Current Chapter (0xF0), Total Wins (0xF4)
			memcpy((void*)(saveStoryBase + 0xF8), saveDatadxp + 0xF8, 0x8); // ??
			memcpy((void*)(saveStoryBase + 0x100), saveDatadxp + 0x100, 0x8); // Win Streak (0x104)
			memcpy((void*)(saveStoryBase + 0x108), saveDatadxp + 0x108, 0x8); // ??
			memcpy((void*)(saveStoryBase + 0x110), saveDatadxp + 0x110, 0x8); // Locked Chapters (0x110) (Bitmask)
			
			// Can't tell if the data past this point does anything
			
			// memcpy((void*)(saveStoryBase + 0x118), saveDatadxp + 0x118, 0x8); // ??
			// memcpy((void*)(saveStoryBase + 0x120), saveDatadxp + 0x120, 0x8); // ??
			// memcpy((void*)(saveStoryBase + 0x128), saveDatadxp + 0x128, 0x8); // ??
			// memcpy((void*)(saveStoryBase + 0x130), saveDatadxp + 0x130, 0x8); // ??
			// memcpy((void*)(saveStoryBase + 0x138), saveDatadxp + 0x138, 0x8); // ??
			// memcpy((void*)(saveStoryBase + 0x140), saveDatadxp + 0x140, 0x8); // ??
			// memcpy((void*)(saveStoryBase + 0x148), saveDatadxp + 0x148, 0x8); // ??
			// memcpy((void*)(saveStoryBase + 0x150), saveDatadxp + 0x150, 0x8); // ??
			// memcpy((void*)(saveStoryBase + 0x158), saveDatadxp + 0x158, 0x8); // ??

			// Save data loaded successfully
			loadOkDxp = true;

			//+ 0x80

			//+ [0x340]

			// [[[[0000000005CE5850] + 108] + 340] + 50] + 50
			// [[[[0000000005CE5850] + 108] + 340] + 50] + 54
			// wmn5r.exe + 1948BF8
			//TA stuff

			//[[[[magic_rva]+108]+340]+50]

			// memcpy((void*)(saveStoryBase + 0x24E0 + 0x16), saveDatadxp + 0x16, 0x1000);

			//value += 0x24E0;
			// First chunk
			//memcpy((void *)(value + 0x16), saveData + 0x16, 0x28);
			////
			//memcpy((void *)(value + 0x40), saveData + 0x40, 0x18);
			////
			//memcpy((void *)(value + 0x60), saveData + 0x60, 0x20);
			////
			//memcpy((void *)(value + 0x90), saveData + 0x90, 0x28);
			////
			//memcpy((void *)(value + 0xC0), saveData + 0xC0, 0x10);
			////
			//memcpy((void *)(value + 0xD8), saveData + 0xD8, 0x28); // OK
			////
			//memcpy((void *)(value + 0x110), saveData + 0x110, 0x98);
			////
			//memcpy((void *)(value + 0x1B8), saveData + 0x1B8, 0x48);
			////
			//memcpy((void *)(value + 0x208), saveData + 0x208, 0x28);
			////
			//memcpy((void *)(value + 0x240), saveData + 0x240, 0x68);
			////
			//memcpy((void *)(value + 0x2B8), saveData + 0x2B8, 0x88);
			//
			//memcpy((void *)(value + 0x370), saveData + 0x370, 0x10);
			////
			//memcpy((void *)(value + 0x388), saveData + 0x388, 0x90);
			////
			//memcpy((void *)(value + 0x420), saveData + 0x420, 0x18);
			////
			//memcpy((void *)(value + 0x440), saveData + 0x440, 0x18);
			////
			//memcpy((void *)(value + 0x460), saveData + 0x460, 0x48);
			////
			//memcpy((void *)(value + 0x4B8), saveData + 0x4B8, 0xB8);
			////
			//memcpy((void *)(value + 0x578), saveData + 0x578, 0x08);
			////
			//memcpy((void *)(value + 0x5A8), saveData + 0x5A8, 0x68);
			////
			//memcpy((void *)(value + 0x628), saveData + 0x628, 0x48);
			////
			//memcpy((void *)(value + 0x688), saveData + 0x688, 0x48);
			////
			//memcpy((void *)(value + 0x6E8), saveData + 0x6E8, 0xA8);
			////
			//memcpy((void *)(value + 0x7A0), saveData + 0x7A0, 0x10);
			////
			//memcpy((void *)(value + 0x7B8), saveData + 0x7B8, 0x28);
			////
			//memcpy((void *)(value + 0x7E8), saveData + 0x7E8, 0x10);
			////
			////memcpy((void *)(value + 0x800), saveData + 0x800, 0x48); // Problem
			//////
			//memcpy((void *)(value + 0x850), saveData + 0x850, 0x08);
			//
			//memcpy((void *)(value + 0x860), saveData + 0x860, 0x08);
			//////
			//memcpy((void *)(value + 0x870), saveData + 0x870, 0x18);
			////
			//memcpy((void *)(value + 0x890), saveData + 0x890, 0x40);
			////
			//memcpy((void *)(value + 0x8E0), saveData + 0x8E0, 0x10);
			////
			//memcpy((void *)(value + 0x8F8), saveData + 0x8F8, 0x28);
			////
			//memcpy((void *)(value + 0x928), saveData + 0x928, 0x10);
			////
			//memcpy((void *)(value + 0x940), saveData + 0x940, 0x48); // Problem
		}
		fclose(file);
	}

	//LoadStoryData();
	//LoadCampaingHonorData();
	//LoadStoryModeNoLoseHonorData();
	//LoadOtherHonorData();
	//LoadCampaingHonorData2();

	// Success status
	return 1;
}

//static int LoadCampaingHonorData2()
//{
//	memset(saveData, 0x0, 0x1000);
//	FILE* file = fopen(V("CampaignHonorData.sav"), V("rb"));
//	if (file)
//	{
//		fseek(file, 0, SEEK_END);
//		int fsize = ftell(file);
//		if (fsize == 0x100)
//		{
//			fseek(file, 0, SEEK_SET);
//			fread(saveData, fsize, 1, file);
//			uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
//			uintptr_t value = *(uintptr_t*)(imageBase + 0x1948F10);
//			value += 0x2698;
//			memcpy((void *)value, saveData, 0x48);
//		}
//		fclose(file);
//	}
//	return 1;
//}
//
//static int LoadOtherHonorData()
//{
//	memset(saveData, 0x0, 0x1000);
//	FILE* file = fopen(V("OtherHonorData.sav"), V("rb"));
//	if (file)
//	{
//		fseek(file, 0, SEEK_END);
//		int fsize = ftell(file);
//		if (fsize == 0x100)
//		{
//			fseek(file, 0, SEEK_SET);
//			fread(saveData, fsize, 1, file);
//			uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
//			uintptr_t value = *(uintptr_t*)(imageBase + 0x1948F10);
//			value += 0x2A90;
//			memcpy((void *)value, saveData, 0x60);
//		}
//		fclose(file);
//	}
//	return 1;
//}
//
//static int LoadStoryModeNoLoseHonorData()
//{
//	memset(saveData, 0x0, 0x1000);
//	FILE* file = fopen(V("StoryModeNoLoseHonorData.sav"), V("rb"));
//	if (file)
//	{
//		fseek(file, 0, SEEK_END);
//		int fsize = ftell(file);
//		if (fsize == 0x100)
//		{
//			fseek(file, 0, SEEK_SET);
//			fread(saveData, fsize, 1, file);
//			uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
//			uintptr_t value = *(uintptr_t*)(imageBase + 0x1948F10);
//			value += 0x2C80;
//			//memcpy((void *)value, saveData, 0x10);
//			value += 0x18;
//			memcpy((void *)value, saveData, 0x28);
//		}
//		fclose(file);
//	}
//	return 1;
//}
//
//static int LoadCampaingHonorData()
//{
//	memset(saveData, 0x0, 0x1000);
//	FILE* file = fopen(V("campaing.sav"), V("rb"));
//	if (file)
//	{
//		fseek(file, 0, SEEK_END);
//		int fsize = ftell(file);
//		if (fsize == 0x100)
//		{
//			fseek(file, 0, SEEK_SET);
//			fread(saveData, fsize, 1, file);
//			uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
//			uintptr_t value = *(uintptr_t*)(imageBase + 0x1948F10);
//			value += 0x24E0;
//			memcpy((void *)value, saveData, 0xB8);
//		}
//		fclose(file);
//	}
//	return 1;
//}
//
//static int LoadStoryData()
//{
//	memset(saveData, 0x0, 0x1000);
//	FILE* file = fopen(V("story.sav"), V("rb"));
//	if (file)
//	{
//		fseek(file, 0, SEEK_END);
//		int fsize = ftell(file);
//		if (fsize == 0x100)
//		{
//			fseek(file, 0, SEEK_SET);
//			fread(saveData, fsize, 1, file);
//			uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
//			uintptr_t value = *(uintptr_t*)(imageBase + 0x1948F10);
//			value += 0x25F0;
//			memcpy((void *)value, saveData, 0x98);
//		}
//		fclose(file);
//	}
//	return 1;
//}

static int saveStoryData(char* filepath)
{
	// Miles path string
	char storyPath[0xFF];

	// Set the milepath memory to zero
	memset(storyPath, 0, 0xFF);

	// Copy the file path to the miles path
	strcpy(storyPath, filepath);

	// Append the mileage filename to the string
	strcat(storyPath, "\\openprogress.sav");

	// Save story data

	// Address where player save data starts
	uintptr_t saveDataBase = *(uintptr_t*)(imageBasedxplus + 0x1F7D578);

	// Zero out save data binary
	memset(saveDatadxp, 0, 0x2000);

	// Address where the player story data starts
	uintptr_t storySaveBase = *(uintptr_t*)(saveDataBase + 0x108);

	// Copy to saveDatadxp from the story save data index
	memcpy(saveDatadxp, (void*)storySaveBase, 0x340);

	// Dump the save data to openprogress.sav
	writeDump(storyPath, saveDatadxp, 0x2000);

	// Success
	return 1;
}

static int loadMileData(char* filepath)
{
	// Zero out the mile data memory
	memset(mileDatadxp, 0, 0x08);

	// Miles path string
	char milepath[0xFF];

	// Set the milepath memory to zero
	memset(milepath, 0, 0xFF);

	// Copy the file path to the miles path
	strcpy(milepath, filepath);

	// Append the mileage filename to the string
	strcat(milepath, "\\mileage.dat");

	// Path to the miles file
	FILE* miles = fopen(milepath, "rb");
	
	// File loaded OK
	if (miles)
	{
		// Get the size of the file
		fseek(miles, 0, SEEK_END);
		int mileSize = ftell(miles);

		// If the file size is correct
		if (mileSize == 0x08)
		{
			// Load the content from the file into mileDatadxp
			fseek(miles, 0, SEEK_SET);
			fread(mileDatadxp, mileSize, 1, miles);

			// Get the pointer to the memory location storing the miles
			uintptr_t mileMemory = *(uintptr_t*)(imageBasedxplus + 0x1F7D578);

			// Copy the mile data from the file into the memory location
			memcpy((void*)(mileMemory + 0x280), mileDatadxp + 0x00, 0x04);

			fclose(miles);
		}
	}

	// Success
	return 1;
}

static int saveMileData(char* filepath)
{
	// Get the pointer to the memory location storing the miles
	auto mileageLocation = (uintptr_t*)(*(uintptr_t*)(imageBasedxplus + 0x1F7D578) + 0x280);

	// Miles path string
	char milepath[0xFF];

	// Set the milepath memory to zero
	memset(milepath, 0, 0xFF);

	// Copy the file path to the miles path
	strcpy(milepath, filepath);

	// Append the mileage filename to the string
	strcat(milepath, "\\mileage.dat");

	// Load the miles file
	FILE* tempFile = fopen(milepath, "wb");

	// Write the miles data from memory to the miles file
	fwrite(mileageLocation, 1, sizeof(mileageLocation), tempFile);

	fclose(tempFile);

	// Success
	return 1;
}

static int loadGameData()
{
	// Disable saving
	saveOk = false;

	// Miles path string
	char loadPath[0xFF];

	// Set the milepath memory to zero
	memset(loadPath, 0, 0xFF);

	// Wirte the '.' into the load path
	sprintf(loadPath, ".\\SaveData");

	// Seperate save file / cars per user profile
	if (ToBool(config["Save"]["Save Per Custom Name"]))
	{
		// Get the profile name from the 
		std::string name = config["General"]["CustomName"];

		// Add the c string version of the profile name to the path
		sprintf(loadPath, "%s\\%s", loadPath, name.c_str());
	}

	// Seperate miles / story per car
	if (ToBool(config["Save"]["Save Per Car"]))
	{
		// Need to get the hex code for the selected car

		// Address where player save data starts
		uintptr_t saveDataBase = *(uintptr_t*)(imageBasedxplus + 0x1F7D578);

		// Address where the car save data starts
		uintptr_t carSaveBase = *(uintptr_t*)(saveDataBase + 0x268);

		// If custom car is set
		if (customCarDxp)
		{
			// Add the car id to the save path
			sprintf(loadPath, "%s\\custom", loadPath);
		}
		else // Custom car is not set
		{
			// Add the custom folder to the save path
			sprintf(loadPath, "%s\\%08X", loadPath, *(DWORD*)(*(uintptr_t*)(*(uintptr_t*)(imageBasedxplus + 0x1F7D578) + 0x268) + 0x34));
		}
	}

	// Ensure the directory exists
	std::filesystem::create_directories(loadPath);

	// Load the car save file
	loadCarData(loadPath);

	// Load the openprogress.sav file
	loadStoryData(loadPath);

	// Load the miles save file
	loadMileData(loadPath);

	// Success
	return 1;
}



// SaveGameData(void): Int
// If saving is enabled, loads the 
// player story data 
static int SaveGameData()
{
	// Saving is disabled
	if (!saveOk)
		return 1;

	// Miles path string
	char savePath[0xFF];

	// Set the milepath memory to zero
	memset(savePath, 0, 0xFF);

	// Wirte the '.' into the load path
	sprintf(savePath, ".\\SaveData");

	// Seperate save file / cars per user profile
	if (ToBool(config["Save"]["Save Per Custom Name"]))
	{
		// Get the profile name from the 
		std::string name = config["General"]["CustomName"];

		// Add the c string version of the profile name to the path
		sprintf(savePath, "%s\\%s", savePath, name.c_str());
	}

	// Seperate miles / story per car
	if (ToBool(config["Save"]["Save Per Car"]))
	{
		// Need to get the hex code for the selected car

		// Address where player save data starts
		uintptr_t saveDataBase = *(uintptr_t*)(imageBasedxplus + 0x1F7D578);

		// Address where the car save data starts
		uintptr_t carSaveBase = *(uintptr_t*)(saveDataBase + 0x268);

		// If custom car is set
		if (customCarDxp)
		{
			// Add the car id to the save path
			sprintf(savePath, "%s\\custom", savePath);
		}
		else // Custom car is not set
		{
			// Add the custom folder to the save path
			sprintf(savePath, "%s\\%08X", savePath, *(DWORD*)(*(uintptr_t*)(*(uintptr_t*)(imageBasedxplus + 0x1F7D578) + 0x268) + 0x34));
		}
	}

	// Ensure the directory exists
	std::filesystem::create_directories(savePath);

	// Load the car save file
	saveCarData(savePath);

	// Load the openprogress.sav file
	saveStoryData(savePath);

	// Load the miles save file
	saveMileData(savePath);

	// Disable saving
	saveOk = false;

	// Success
	return 1;
}

static void loadGame()
{
	// Runs after car data is loaded

	// Load story data thread
	std::thread t1(loadGameData);
	t1.detach();
}

static int ReturnTrue()
{
	return 1;
}

void GenerateDongleDataDxp(bool isTerminal)
{
	memset(hasp_buffer, 0, 0xD40);
	hasp_buffer[0] = 0x01;
	hasp_buffer[0x13] = 0x01;
	hasp_buffer[0x17] = 0x0A;
	hasp_buffer[0x1B] = 0x04;
	hasp_buffer[0x1C] = 0x3B;
	hasp_buffer[0x1D] = 0x6B;
	hasp_buffer[0x1E] = 0x40;
	hasp_buffer[0x1F] = 0x87;

	hasp_buffer[0x23] = 0x01;
	hasp_buffer[0x27] = 0x0A;
	hasp_buffer[0x2B] = 0x04;
	hasp_buffer[0x2C] = 0x3B;
	hasp_buffer[0x2D] = 0x6B;
	hasp_buffer[0x2E] = 0x40;
	hasp_buffer[0x2F] = 0x87;

	if(isTerminal)
	{
		memcpy(hasp_buffer + 0xD00, "278311042069", 12); //272211990002
		hasp_buffer[0xD3E] = 0x6B;
		hasp_buffer[0xD3F] = 0x94;
	}
	else
	{
		memcpy(hasp_buffer + 0xD00, "278313042069", 12); //272213990002
		hasp_buffer[0xD3E] = 0x6D;
		hasp_buffer[0xD3F] = 0x92;
	}
}


char customNamedxp[256];


static DWORD WINAPI SpamcustomNamedxp(LPVOID)
{
	while (true)
	{
		
		Sleep(50);
		void *value = (void *)(imageBasedxplus + 0x1F846F0);
		memcpy(value, customNamedxp, strlen(customNamedxp) + 1);
		
	}
}

static DWORD WINAPI SpamMulticast(LPVOID)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	int ttl = 255;
	setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(ttl));

	int reuse = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));

	setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&reuse, sizeof(reuse));

	sockaddr_in bindAddr = { 0 };
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_addr.s_addr = inet_addr(ipaddrdxplus);
	bindAddr.sin_port = htons(50765);
	bind(sock, (sockaddr*)&bindAddr, sizeof(bindAddr));
	

	ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr("225.0.0.1");
	mreq.imr_interface.s_addr = inet_addr(ipaddrdxplus);

	setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));

	const uint8_t* byteSequences_Free[] = {
		dxpterminalPackage1_Free,
		dxpterminalPackage2_Free,
		dxpterminalPackage3_Free,
		dxpterminalPackage4_Free,
		dxpterminalPackage5_Free,
		dxpterminalPackage6_Free,
	};

	const size_t byteSizes_Free[] = {
		sizeof(dxpterminalPackage1_Free),
		sizeof(dxpterminalPackage2_Free),
		sizeof(dxpterminalPackage3_Free),
		sizeof(dxpterminalPackage4_Free),
		sizeof(dxpterminalPackage5_Free),
		sizeof(dxpterminalPackage6_Free),
	};
	
	const uint8_t* byteSequences_Event2P[] = {
		dxpterminalPackage1_Event2P,
		dxpterminalPackage2_Event2P,
		dxpterminalPackage3_Event2P,
		dxpterminalPackage4_Event2P,
		dxpterminalPackage5_Event2P,
		dxpterminalPackage6_Event2P,
	};

	const size_t byteSizes_Event2P[] = {
		sizeof(dxpterminalPackage1_Event2P),
		sizeof(dxpterminalPackage2_Event2P),
		sizeof(dxpterminalPackage3_Event2P),
		sizeof(dxpterminalPackage4_Event2P),
		sizeof(dxpterminalPackage5_Event2P),
		sizeof(dxpterminalPackage6_Event2P),
	};

	const uint8_t* byteSequences_Event4P[] = {
		dxpterminalPackage1_Event4P,
		dxpterminalPackage2_Event4P,
		dxpterminalPackage3_Event4P,
		dxpterminalPackage4_Event4P,
		dxpterminalPackage5_Event4P,
		dxpterminalPackage6_Event4P,
	};

	const size_t byteSizes_Event4P[] = {
		sizeof(dxpterminalPackage1_Event4P),
		sizeof(dxpterminalPackage2_Event4P),
		sizeof(dxpterminalPackage3_Event4P),
		sizeof(dxpterminalPackage4_Event4P),
		sizeof(dxpterminalPackage5_Event4P),
		sizeof(dxpterminalPackage6_Event4P),
	};

	const uint8_t* byteSequences_Coin[] = {
		dxpterminalPackage1_Coin,
		dxpterminalPackage2_Coin,
		dxpterminalPackage3_Coin,
		dxpterminalPackage4_Coin,
		dxpterminalPackage5_Coin,
		dxpterminalPackage6_Coin,
	};

	const size_t byteSizes_Coin[] = {
		sizeof(dxpterminalPackage1_Coin),
		sizeof(dxpterminalPackage2_Coin),
		sizeof(dxpterminalPackage3_Coin),
		sizeof(dxpterminalPackage4_Coin),
		sizeof(dxpterminalPackage5_Coin),
		sizeof(dxpterminalPackage6_Coin),
	};
	
	sockaddr_in toAddr = { 0 };
	toAddr.sin_family = AF_INET;
	toAddr.sin_addr.s_addr = inet_addr("225.0.0.1");
	toAddr.sin_port = htons(50765);
	
	
	isFreePlay = ToBool(config["General"]["FreePlay"]);
	isEventMode2P = ToBool(config["TerminalEmuConfig"]["2P Event Mode"]);
	isEventMode4P = ToBool(config["TerminalEmuConfig"]["4P Event Mode"]);
	
	
	if (isFreePlay)
	{
		if (isEventMode2P) {
			while (true) for (int i = 0; i < _countof(byteSequences_Event2P); i++)
			{
				sendto(sock, (const char*)byteSequences_Event2P[i], byteSizes_Event2P[i], 0, (sockaddr*)&toAddr, sizeof(toAddr));
				Sleep(8);
			}
		}
		else if (isEventMode4P) {
			while (true) for (int i = 0; i < _countof(byteSequences_Event4P); i++)
			{
				sendto(sock, (const char*)byteSequences_Event4P[i], byteSizes_Event4P[i], 0, (sockaddr*)&toAddr, sizeof(toAddr));
				Sleep(8);
			}
		}
		else {
			while (true) for (int i = 0; i < _countof(byteSequences_Free); i++)
			{
				sendto(sock, (const char*)byteSequences_Free[i], byteSizes_Free[i], 0, (sockaddr*)&toAddr, sizeof(toAddr));
				Sleep(8);
			}
		}
	}
	
	while (true) for (int i = 0; i < _countof(byteSequences_Coin); i++)
	{
		sendto(sock, (const char*)byteSequences_Coin[i], byteSizes_Coin[i], 0, (sockaddr*)&toAddr, sizeof(toAddr));
		Sleep(8);
	}
}

/*
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;
DWORD WINAPI Wmmt5FfbCollector(void* ctx)
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
	while (true)
	{
		*ffbOffset = *(DWORD *)(imageBase + 0x196F188);
		*ffbOffset2 = *(DWORD *)(imageBase + 0x196F18c);
		*ffbOffset3 = *(DWORD *)(imageBase + 0x196F190);
		*ffbOffset4 = *(DWORD *)(imageBase + 0x196F194);
		Sleep(10);
	}
}*/


// Wmmt5Func([]()): InitFunction
// Performs the initial startup tasks for 
// maximum tune 5, including the starting 
// of required subprocesses.
static InitFunction Wmmt5Func([]()
{
	/*
	FILE* fileF = _wfopen(L"Fsetting.lua.gz", L"r");
	if (fileF == NULL)
	{
		FILE* settingsF = _wfopen(L"Fsetting.lua.gz", L"wb");
		fwrite(settingData, 1, sizeof(settingData), settingsF);
		fclose(settingsF);
	}
	else
	{
		fclose(fileF);
	}
	FILE* fileG = _wfopen(L"Gsetting.lua.gz", L"r");
	if (fileG == NULL)
	{
		FILE* settingsG = _wfopen(L"Gsetting.lua.gz", L"wb");
		fwrite(settingData, 1, sizeof(settingData), settingsG);
		fclose(settingsG);
	}
	else
	{
		fclose(fileG);
	}
	*/

	// Records if terminal mode is enabled
	bool isTerminal = false;

	// If terminal mode is set in the general settings
	if (ToBool(config["General"]["TerminalMode"]))
	{
		// Terminal mode is set
		isTerminal = true;
	}
	
	// Get the network adapter ip address from the general settings
	std::string networkip = config["General"]["NetworkAdapterIP"];

	// If the ip address is not blank
	if (!networkip.empty())
	{
		// Overwrite the default ip address
		ipaddrdxplus = networkip.c_str();
	}

	hookPort = "COM3";
	imageBasedxplus = (uintptr_t)GetModuleHandleA(0);

	MH_Initialize();

	// Hook dongle funcs
	MH_CreateHookApi(L"hasp_windows_x64_106482.dll", "hasp_write", dxpHook_hasp_write, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_106482.dll", "hasp_read", dxpHook_hasp_read, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_106482.dll", "hasp_get_size", dxpHook_hasp_get_size, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_106482.dll", "hasp_decrypt", dxpHook_hasp_decrypt, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_106482.dll", "hasp_encrypt", dxpHook_hasp_encrypt, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_106482.dll", "hasp_logout", dxpHook_hasp_logout, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_106482.dll", "hasp_login", dxpHook_hasp_login, NULL);

	GenerateDongleDataDxp(isTerminal);

	// Prevents game from setting time, thanks pockywitch!
	MH_CreateHookApi(L"KERNEL32", "SetSystemTime", Hook_SetSystemTime, reinterpret_cast<LPVOID*>(&pSetSystemTime));

	// Patch some check TEMP DISABLE AS WELL OVER HERE
	// 0F 94 C0 84 C0 0F 94 C0 84 C0 75 05 45 32 E4 EB 03 41 B4 01
	// FOUND ON 21, 10
	// NOT WORKING 1
	// 0F 94 C0 84 C0 0F 94 C0 84 C0 75 05 45 32 ?? EB
	// FOUND ON 1
	//injector::WriteMemory<uint8_t>(imageBase + 0x6286EC, 0, true); 
	injector::WriteMemory<uint8_t>(hook::get_pattern("85 C9 0F 94 C0 84 C0 0F 94 C0 84 C0 75 ? 40 32 F6 EB ?", 0x15), 0, true); //patches out dongle error2 (doomer)

	// Patch some jnz
	// 83 C0 FD 83 F8 01 0F 87 B4 00 00 00 83 BF D0 06 00 00 3C 73 29 48 8D 8D 60 06 00 00
	// FOUND ON 21, 10
	// NOT FOUND: 1
	// 83 C0 FD 83 F8 01 0F 87 B4 00 00 00
	// FOUND ON 1
	//injector::MakeNOP(imageBase + 0x628AE0, 6);
	//THIS injector::MakeNOP(hook::get_pattern("83 C0 FD 83 F8 01 0F 87 B4 00 00 00", 6), 6);
	injector::MakeNOP(hook::get_pattern("83 C0 FD 83 F8 01 76 ? 49 8D ? ? ? ? 00 00"), 6);

	// Patch some shit
	// 83 FA 04 0F 8C 1E 01 00 00 4C 89 44 24 18 4C 89 4C 24 20
	// FOUND ON 21, 10, 1
	// NOT FOUND:
	//injector::WriteMemory<uint8_t>(imageBase + 0x7B9882, 0, true);
	//THIS injector::WriteMemory<uint8_t>(hook::get_pattern("83 FA 04 0F 8C 1E 01 00 00 4C 89 44 24 18 4C 89 4C 24 20", 2), 0, true);
		
	// Skip weird camera init that stucks entire pc on certain brands. TESTED ONLY ON 05!!!!
	if (ToBool(config["General"]["WhiteScreenFix"]))
	{
		injector::WriteMemory<DWORD>(hook::get_pattern("48 8B C4 55 57 41 54 41 55 41 56 48 8D 68 A1 48 81 EC 90 00 00 00 48 C7 45 D7 FE FF FF FF 48 89 58 08 48 89 70 18 45 33 F6 4C 89 75 DF 33 C0 48 89 45 E7", 0), 0x90C3C032, true);
	}

	// Patch some call
	// 45 33 C0 BA 65 09 00 00 48 8D 4D B0 E8 ?? ?? ?? ?? 48 8B 08
	// FOUND ON 21, 10, 1

	{
		// 199AE18 TIME OFFSET RVA temp disable ALL JNZ PATCH

		auto location = hook::get_pattern<char>("41 3B C7 74 0E 48 8D 8F B8 00 00 00 BA F6 01 00 00 EB 6E 48 8D 8F A0 00 00 00");
		
		// Patch some jnz
		// 41 3B C7 74 0E 48 8D 8F B8 00 00 00 BA F6 01 00 00 EB 6E 48 8D 8F A0 00 00 00
		// FOUND ON 21, 10, 1
		injector::WriteMemory<uint8_t>(location + 3, 0xEB, true); //patches content router (doomer)

		// Skip some jnz
		injector::MakeNOP(location + 0x22, 2); //patches ip addr error again (doomer)

		// Skip some jnz
		injector::MakeNOP(location + 0x33, 2); //patches ip aaddr error(doomer)
	}

	// Terminal mode is off
	if (!isTerminal)
	{
		// Disregard terminal scanner stuff.
		// 48 8B 18 48 3B D8 0F 84 88 00 00 00 39 7B 1C 74 60 80 7B 31 00 75 4F 48 8B 43 10 80 78 31 00
		// FOUND ON 21, 10, 1
		//injector::MakeNOP(imageBase + 0x91E1AE, 6);
		//injector::MakeNOP(imageBase + 0x91E1B7, 2);
		//injector::MakeNOP(imageBase + 0x91E1BD, 2);

		/*
		auto location = hook::get_pattern<char>("48 8B 18 48 3B D8 0F 84 8B 00 00 00 0F 1F 80 00 00 00 00 39 73 1C 74 5C 80 7B 31 00");
		// injector::MakeNOP(location + 6, 6); // 6
		injector::MakeNOP(location + 0xF, 2); // 0xF
		// injector::MakeNOP(location + 0x15, 2); // 0x15
		*/

		injector::MakeNOP(imageBasedxplus + 0x9F2BB3, 2);

		// If terminal emulator is enabled
		if (ToBool(config["General"]["TerminalEmulator"]))
		{
			// Start the multicast spam thread
			CreateThread(0, 0, SpamMulticast, 0, 0, 0);
		}
	}
	/*
	else
	{
		// Patch some func to 1
		// 
		// FOUND ON 21, 10, 1
		// NOT FOUND:
		//safeJMP(imageBase + 0x7BE440, ReturnTrue);
		//safeJMP(hook::get_pattern("0F B6 41 05 2C 30 3C 09 77 04 0F BE C0 C3 83 C8 FF C3"), ReturnTrue);
		//safeJMP(imageBase + 0x89D420, ReturnTrue);

		// Patch some func to 1
		// 40 53 48 83 EC 20 48 83 39 00 48 8B D9 75 28 48 8D ?? ?? ?? ?? 00 48 8D ?? ?? ?? ?? 00 41 B8 ?? ?? 00 00 FF 15 ?? ?? ?? ?? 4C 8B 1B 41 0F B6 43 78
		// FOUND ON 21, 10, 1
		//safeJMP(imageBase + 0x7CF8D0, ReturnTrue); 
		//safeJMP(hook::get_pattern("40 53 48 83 EC 20 48 83 39 00 48 8B D9 75 11 48 8B 0D C2"), ReturnTrue);
		//safeJMP(imageBase + 0x8B5190, ReturnTrue); 
	}
	*/

	auto chars = { 'F', 'G' };

	for (auto cha : chars)
	{
		auto patterns = hook::pattern(va("%02X 3A 2F", cha));

		if (patterns.size() > 0)
		{
			for (int i = 0; i < patterns.size(); i++)
			{
				char* text = patterns.get(i).get<char>(0);
				std::string text_str(text);

				std::string to_replace = va("%c:/", cha);
				std::string replace_with = va("./%c", cha);

				std::string replaced = text_str.replace(0, to_replace.length(), replace_with);

				injector::WriteMemoryRaw(text, (char*)replaced.c_str(), replaced.length() + 1, true);
			}
		}
	}

	// Get the custom name specified in the  config file
	std::string value = config["General"]["CustomName"];

	// If a custom name is set
	if (!value.empty())
	{
		// Zero out the custom name variable
		memset(customNamedxp, 0, 256);

		// Copy the custom name to the custom name block
		strcpy(customNamedxp, value.c_str());

		// Create the spam custom name thread
		CreateThread(0, 0, SpamcustomNamedxp, 0, 0, 0);
	}

	// Save story stuff (only 05)
	{
		// Skip erasing of temp card data
		// injector::WriteMemory<uint8_t>(imageBase + 0xA35CA3, 0xEB, true);
		
		/*
		// Skip erasing of temp card
		safeJMP(imageBase + 0x54DCE1, LoadGameData);
		safeJMP(imageBase + 0x5612F0, ReturnTrue);
		safeJMP(imageBase + 0x5753C0, ReturnTrue);
		safeJMP(imageBase + 0x57DF10, ReturnTrue);
		safeJMP(imageBase + 0x92DB20, ReturnTrue);
		safeJMP(imageBase + 0x5628C0, ReturnTrue);
		safeJMP(imageBase + 0x579090, ReturnTrue);
		
		// Skip more
		safeJMP(imageBase + 0x54B0F0, ReturnTrue);
		safeJMP(imageBase + 0x909DB0, ReturnTrue);
		safeJMP(imageBase + 0x59FD90, ReturnTrue);
		safeJMP(imageBase + 0x5A0030, ReturnTrue);
		safeJMP(imageBase + 0x915370, ReturnTrue);
		safeJMP(imageBase + 0x5507A0, ReturnTrue);
		safeJMP(imageBase + 0x561290, ReturnTrue);
		safeJMP(imageBase + 0x5A0AE8, LoadWmmt5CarData);
		
		// crash fix
		//safeJMP(imageBase + 0xAD6F28, WmmtOperatorDelete);
		//safeJMP(imageBase + 0xAD6F4C, WmmtMemset);
		
		// Save progress trigger
		injector::WriteMemory<WORD>(imageBase + 0x556CE3, 0xB848, true);
		injector::WriteMemory<uintptr_t>(imageBase + 0x556CE3 + 2, (uintptr_t)SaveOk, true);
		injector::WriteMemory<DWORD>(imageBase + 0x556CED, 0x9090D0FF, true);
		
		// Try save later!
		injector::MakeNOP(imageBase + 0x308546, 0x12);
		injector::WriteMemory<WORD>(imageBase + 0x308546, 0xB848, true);
		injector::WriteMemory<uintptr_t>(imageBase + 0x308546 + 2, (uintptr_t)SaveGameData, true);
		injector::WriteMemory<DWORD>(imageBase + 0x308550, 0x3348D0FF, true);
		injector::WriteMemory<WORD>(imageBase + 0x308550 + 4, 0x90C0, true);
		CreateThread(0, 0, Wmmt5FfbCollector, 0, 0, 0);
		*/

		// Enable all print
		injector::MakeNOP(imageBasedxplus + 0x898BD3, 6);

		// Load car and story data at once
		safeJMP(imageBasedxplus + 0x72AB90, loadGame);

		// Save car trigger
		// injector::WriteMemory<uintptr_t>(imageBase + 0x376F80 + 2, (uintptr_t)SaveGameData, true);
		// safeJMP(imageBase + 0x376F76, SaveGameData);

		// Save car trigger
		injector::MakeNOP(imageBasedxplus + 0x376F76, 0x12);
		injector::WriteMemory<WORD>(imageBasedxplus + 0x376F76, 0xB848, true);
		injector::WriteMemory<uintptr_t>(imageBasedxplus + 0x376F76 + 2, (uintptr_t)SaveGameData, true);
		injector::WriteMemory<DWORD>(imageBasedxplus + 0x376F80, 0x3348D0FF, true);
		injector::WriteMemory<WORD>(imageBasedxplus + 0x376F80 + 4, 0x90C0, true);

		// Prevents startup saving
		// injector::MakeNOP(imageBase + 0x6B908C, 0x0D);
		// safeJMP(imageBase + 0x6B908C, SaveOk);
		injector::WriteMemory<WORD>(imageBasedxplus + 0x6B909A, 0xB848, true);
		injector::WriteMemory<uintptr_t>(imageBasedxplus + 0x6B909A + 2, (uintptr_t)SaveOk, true);
		injector::WriteMemory<DWORD>(imageBasedxplus + 0x6B90A4, 0x9090D0FF, true);
	}

	MH_EnableHook(MH_ALL_HOOKS);

}, GameID::WMMT5DXPlus);
#endif
#pragma optimize("", on)