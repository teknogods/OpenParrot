#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "MinHook.h"
#include <Utility/Hooking.Patterns.h>
#include <thread>
#ifdef _M_AMD64
#pragma optimize("", off)
#pragma comment(lib, "Ws2_32.lib")

extern LPCSTR hookPort;
static uintptr_t imageBase;
static unsigned char hasp_buffer[0xD40];
static bool isFreePlay;
static bool isEventMode2P;
static bool isEventMode4P;
static bool ForceFullTune;
static bool ForceNeon;
static bool CarTuneNeonThread;
static const char* ipaddr;
static DWORD mileageValue = 0;
static int NeonColour;

// Data for IC card, Force Feedback etc OFF. 
static unsigned char settingData[405] = {
	0x1F, 0x8B, 0x08, 0x08, 0x53, 0x6A, 0x8B, 0x5A, 0x00, 0x00, 0x73, 0x65, 
	0x74, 0x74, 0x69, 0x6E, 0x67, 0x2E, 0x6C, 0x75, 0x61, 0x00, 0x85, 0x93, 
	0xC9, 0x6E, 0xC2, 0x30, 0x10, 0x86, 0xEF, 0x79, 0x0A, 0x5E, 0x80, 0x8A, 
	0x90, 0x80, 0xE8, 0xA1, 0x07, 0x08, 0xA4, 0x20, 0x11, 0x81, 0x20, 0x2A, 
	0x52, 0x6F, 0xC6, 0x19, 0x88, 0x85, 0x17, 0xE4, 0xD8, 0xAD, 0x78, 0xFB, 
	0xDA, 0x59, 0x1D, 0xB5, 0x2A, 0x39, 0x44, 0xF9, 0xBF, 0x59, 0x32, 0x8B, 
	0x3D, 0x1C, 0xFE, 0xFF, 0x78, 0xF6, 0x35, 0x28, 0x40, 0x29, 0xC2, 0xAF, 
	0x2F, 0x54, 0x23, 0xEF, 0x49, 0xC0, 0xD0, 0xF3, 0x58, 0x84, 0x28, 0x39, 
	0xAF, 0x11, 0xCF, 0x28, 0x44, 0xC0, 0x15, 0xC8, 0xC1, 0xDB, 0x20, 0x08, 
	0x27, 0xD3, 0x51, 0x6D, 0x9A, 0x63, 0x0C, 0xB4, 0xB5, 0x34, 0x74, 0x21, 
	0xD1, 0x0D, 0x7E, 0xD1, 0x44, 0x28, 0x21, 0x5B, 0x3A, 0xF1, 0xFD, 0x9A, 
	0xA7, 0x42, 0xE3, 0x7C, 0x0B, 0x17, 0x65, 0xE8, 0x78, 0x14, 0xCE, 0x5C, 
	0x7C, 0x20, 0xD7, 0xDC, 0x72, 0x3F, 0x0C, 0x82, 0xA9, 0x6B, 0x48, 0xC5, 
	0xFD, 0x2F, 0xBC, 0x10, 0x4A, 0x09, 0xD6, 0x25, 0x12, 0x84, 0x47, 0xB9, 
	0x56, 0x60, 0x7D, 0x3D, 0xB6, 0xD0, 0x8F, 0x08, 0xC9, 0x2C, 0x12, 0x85, 
	0xCD, 0x19, 0x78, 0xEC, 0x1D, 0x31, 0xA8, 0xD5, 0xD8, 0x7A, 0x73, 0x33,
	0x1B, 0xED, 0x90, 0x58, 0x53, 0x1A, 0x09, 0x2D, 0x8B, 0x86, 0x85, 0x86,
	0x49, 0x80, 0x3D, 0x45, 0x8F, 0x2A, 0xE5, 0x1E, 0x9F, 0x37, 0x59, 0xD5, 
	0xE4, 0x06, 0xDB, 0xE4, 0x87, 0x6F, 0x57, 0x7D, 0x00, 0xCF, 0x9A, 0x21, 
	0x24, 0x57, 0xD7, 0x1E, 0x0B, 0x89, 0x21, 0x06, 0xC8, 0xCE, 0x08, 0xDF, 
	0x2A, 0x74, 0x22, 0xBC, 0x98, 0xF3, 0xEC, 0x00, 0x0C, 0x99, 0xAF, 0x2A, 
	0xFF, 0xEA, 0xCB, 0x0C, 0x2C, 0x11, 0x19, 0x54, 0x2E, 0xAD, 0x5C, 0x92, 
	0xB2, 0x1E, 0x17, 0x99, 0x42, 0x79, 0x5D, 0x63, 0x44, 0x45, 0x01, 0xE9, 
	0xE3, 0x0E, 0x75, 0x63, 0x56, 0x1E, 0x35, 0x37, 0xEA, 0x75, 0x5A, 0xCB, 
	0x44, 0xF4, 0x64, 0xAA, 0xC1, 0x95, 0x27, 0xC8, 0x7A, 0xD6, 0x5C, 0xBB, 
	0x32, 0x96, 0xC4, 0x95, 0x47, 0xA4, 0x5C, 0xB9, 0x2C, 0x67, 0x63, 0x65, 
	0xB9, 0x92, 0x3D, 0xE2, 0x40, 0xAB, 0x52, 0xED, 0xB8, 0x3F, 0x84, 0x15, 
	0xBE, 0x51, 0x73, 0xA5, 0x24, 0xC2, 0xAA, 0x03, 0xBB, 0xCB, 0x85, 0x12, 
	0x0E, 0x5D, 0xB7, 0x26, 0x1D, 0xBE, 0x19, 0x6A, 0x0E, 0x6D, 0x05, 0x52, 
	0xC2, 0xE0, 0x53, 0xF0, 0xA6, 0x35, 0xBB, 0x7B, 0x8B, 0xCC, 0x1F, 0xB7, 
	0xF5, 0x41, 0x71, 0x9C, 0xD6, 0x66, 0x71, 0x6D, 0xF0, 0xAC, 0xE3, 0x09, 
	0xE1, 0x6E, 0xCE, 0xA3, 0x66, 0x0C, 0xA4, 0x35, 0xF6, 0x02, 0x7A, 0x96, 
	0x7E, 0xC8, 0xD3, 0x7B, 0x53, 0xDE, 0xB4, 0xD5, 0x2E, 0x7E, 0xEE, 0xF9, 
	0x03, 0x44, 0x94, 0xFB, 0x8E, 0xB5, 0x03, 0x00, 0x00
};

// FOR FREEPLAY
static unsigned char terminalPackage1_Free[79] = {
	0x01, 0x04, 0x4B, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x02, 0x5A, 0x00, 0x2A, 0x12, 0x08, 0x12, 0x12, 0x0C,
	0x32, 0x38, 0x30, 0x38, 0x31, 0x31, 0x34, 0x30, 0x31, 0x35, 0x33, 0x32,
	0x18, 0x00, 0x30, 0x03, 0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00,
	0x20, 0x00, 0x52, 0x0B, 0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20,
	0x00, 0x28, 0x00, 0xEC, 0x72, 0x00, 0x41
};

static unsigned char terminalPackage2_Free[139] = {
	0x01, 0x04, 0x87, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x02, 0x5A, 0x00, 0x2A, 0x12, 0x08, 0x14, 0x12, 0x0C,
	0x32, 0x38, 0x30, 0x38, 0x31, 0x31, 0x34, 0x30, 0x31, 0x35, 0x33, 0x32,
	0x18, 0x00, 0x30, 0x03, 0x42, 0x3A, 0x08, 0x01, 0x10, 0x03, 0x18, 0x02,
	0x20, 0x02, 0x28, 0x04, 0x30, 0x01, 0x38, 0x01, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x02, 0x58, 0x60, 0x60, 0x60, 0x68, 0x60, 0x70, 0x60, 0x78, 0x60,
	0x80, 0x01, 0x60, 0x88, 0x01, 0x60, 0x90, 0x01, 0x60, 0x98, 0x01, 0x00,
	0xA0, 0x01, 0xE2, 0xBA, 0xAC, 0xD4, 0x05, 0xA8, 0x01, 0x04, 0xB0, 0x01,
	0x24, 0xB8, 0x01, 0x00, 0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00,
	0x20, 0x00, 0x52, 0x0B, 0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20,
	0x00, 0x28, 0x00, 0x99, 0x4E, 0xC6, 0x14
};

static unsigned char terminalPackage3_Free[79] = {
	0x01, 0x04, 0x4B, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x02, 0x5A, 0x00, 0x2A, 0x12, 0x08, 0x19, 0x12, 0x0C,
	0x32, 0x38, 0x30, 0x38, 0x31, 0x31, 0x34, 0x30, 0x31, 0x35, 0x33, 0x32,
	0x18, 0x00, 0x30, 0x03, 0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00,
	0x20, 0x00, 0x52, 0x0B, 0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20,
	0x00, 0x28, 0x00, 0x89, 0x93, 0x3A, 0x22
};

static unsigned char terminalPackage4_Free[139] = {
	0x01, 0x04, 0x87, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x02, 0x5A, 0x00, 0x2A, 0x12, 0x08, 0x2E, 0x12, 0x0C,
	0x32, 0x38, 0x30, 0x38, 0x31, 0x31, 0x34, 0x30, 0x31, 0x35, 0x33, 0x32,
	0x18, 0x00, 0x30, 0x03, 0x42, 0x3A, 0x08, 0x01, 0x10, 0x03, 0x18, 0x02,
	0x20, 0x02, 0x28, 0x04, 0x30, 0x01, 0x38, 0x01, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x02, 0x58, 0x60, 0x60, 0x60, 0x68, 0x60, 0x70, 0x60, 0x78, 0x60,
	0x80, 0x01, 0x60, 0x88, 0x01, 0x60, 0x90, 0x01, 0x60, 0x98, 0x01, 0x00,
	0xA0, 0x01, 0xF0, 0xBA, 0xAC, 0xD4, 0x05, 0xA8, 0x01, 0x04, 0xB0, 0x01,
	0x24, 0xB8, 0x01, 0x00, 0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00,
	0x20, 0x00, 0x52, 0x0B, 0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20,
	0x00, 0x28, 0x00, 0x55, 0x42, 0x47, 0xD5
};

static unsigned char terminalPackage5_Free[79] = {
	0x01, 0x04, 0x4B, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x02, 0x5A, 0x00, 0x2A, 0x12, 0x08, 0x2F, 0x12, 0x0C,
	0x32, 0x38, 0x30, 0x38, 0x31, 0x31, 0x34, 0x30, 0x31, 0x35, 0x33, 0x32,
	0x18, 0x00, 0x30, 0x03, 0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00,
	0x20, 0x00, 0x52, 0x0B, 0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20,
	0x00, 0x28, 0x00, 0x9C, 0xC9, 0xE0, 0x73
};

static unsigned char terminalPackage6_Free[139] = {
	0x01, 0x04, 0x87, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x02, 0x5A, 0x00, 0x2A, 0x12, 0x08, 0x6A, 0x12, 0x0C,
	0x32, 0x38, 0x30, 0x38, 0x31, 0x31, 0x34, 0x30, 0x31, 0x35, 0x33, 0x32,
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
static unsigned char terminalPackage1_Coin[75] = {
	0x01, 0x04, 0x47, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x00, 0x2A, 0x12, 0x08, 0x0B, 0x12, 0x0C, 0x32, 0x38, 
	0x30, 0x38, 0x31, 0x31, 0x34, 0x30, 0x31, 0x35, 0x33, 0x32, 0x18, 0x00,
	0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0B,
	0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0x09,
	0x06, 0x41, 0x0B
};

static unsigned char terminalPackage2_Coin[135] = {
	0x01, 0x04, 0x83, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x00, 0x2A, 0x12, 0x08, 0x39, 0x12, 0x0C, 0x32, 0x38, 
	0x30, 0x38, 0x31, 0x31, 0x34, 0x30, 0x31, 0x35, 0x33, 0x32, 0x18, 0x00,
	0x42, 0x3A, 0x08, 0x01, 0x10, 0x03, 0x18, 0x02, 0x20, 0x02, 0x28, 0x04,
	0x30, 0x00, 0x38, 0x01, 0x40, 0x00, 0x48, 0x00, 0x50, 0x02, 0x58, 0x60,
	0x60, 0x60, 0x68, 0x60, 0x70, 0x60, 0x78, 0x60, 0x80, 0x01, 0x60, 0x88,
	0x01, 0x60, 0x90, 0x01, 0x60, 0x98, 0x01, 0x00, 0xA0, 0x01, 0xD5, 0xBE,
	0x8F, 0xD2, 0x05, 0xA8, 0x01, 0x04, 0xB0, 0x01, 0x24, 0xB8, 0x01, 0x00,
	0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0B,
	0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0xF5,
	0xF1, 0x0D, 0xB2
};

static unsigned char terminalPackage3_Coin[75] = {
	0x01, 0x04, 0x47, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x00, 0x2A, 0x12, 0x08, 0x3A, 0x12, 0x0C, 0x32, 0x38, 
	0x30, 0x38, 0x31, 0x31, 0x34, 0x30, 0x31, 0x35, 0x33, 0x32, 0x18, 0x00,
	0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0B,
	0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0x22,
	0x25, 0x31, 0x0D
};

static unsigned char terminalPackage4_Coin[135] = {
	0x01, 0x04, 0x83, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x00, 0x2A, 0x12, 0x08, 0x57, 0x12, 0x0C, 0x32, 0x38, 
	0x30, 0x38, 0x31, 0x31, 0x34, 0x30, 0x31, 0x35, 0x33, 0x32, 0x18, 0x00,
	0x42, 0x3A, 0x08, 0x01, 0x10, 0x03, 0x18, 0x02, 0x20, 0x02, 0x28, 0x04,
	0x30, 0x00, 0x38, 0x01, 0x40, 0x00, 0x48, 0x00, 0x50, 0x02, 0x58, 0x60,
	0x60, 0x60, 0x68, 0x60, 0x70, 0x60, 0x78, 0x60, 0x80, 0x01, 0x60, 0x88,
	0x01, 0x60, 0x90, 0x01, 0x60, 0x98, 0x01, 0x00, 0xA0, 0x01, 0xD6, 0xBE,
	0x8F, 0xD2, 0x05, 0xA8, 0x01, 0x04, 0xB0, 0x01, 0x24, 0xB8, 0x01, 0x00,
	0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00, 0x20, 0x00, 0x52, 0x0B,
	0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20, 0x00, 0x28, 0x00, 0xCA,
	0x8B, 0x15, 0xCB
};

static unsigned char terminalPackage5_Coin[79] = {
	0x01, 0x04, 0x4B, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x02, 0x5A, 0x00, 0x2A, 0x12, 0x08, 0x58, 0x12, 0x0C,
	0x32, 0x38, 0x30, 0x38, 0x31, 0x31, 0x34, 0x30, 0x31, 0x35, 0x33, 0x32,
	0x18, 0x00, 0x30, 0x03, 0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00,
	0x20, 0x00, 0x52, 0x0B, 0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20,
	0x00, 0x28, 0x00, 0x3E, 0xB1, 0xB7, 0x22
};

static unsigned char terminalPackage6_Coin[139] = {
	0x01, 0x04, 0x87, 0x00, 0x12, 0x14, 0x0A, 0x00, 0x10, 0x04, 0x18, 0x00,
	0x20, 0x00, 0x28, 0x00, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x00, 0x1A, 0x02, 0x5A, 0x00, 0x2A, 0x12, 0x08, 0x77, 0x12, 0x0C,
	0x32, 0x38, 0x30, 0x38, 0x31, 0x31, 0x34, 0x30, 0x31, 0x35, 0x33, 0x32,
	0x18, 0x00, 0x30, 0x03, 0x42, 0x3A, 0x08, 0x01, 0x10, 0x03, 0x18, 0x02,
	0x20, 0x02, 0x28, 0x04, 0x30, 0x00, 0x38, 0x01, 0x40, 0x00, 0x48, 0x00,
	0x50, 0x02, 0x58, 0x60, 0x60, 0x60, 0x68, 0x60, 0x70, 0x60, 0x78, 0x60,
	0x80, 0x01, 0x60, 0x88, 0x01, 0x60, 0x90, 0x01, 0x60, 0x98, 0x01, 0x00,
	0xA0, 0x01, 0xD7, 0xBE, 0x8F, 0xD2, 0x05, 0xA8, 0x01, 0x04, 0xB0, 0x01,
	0x24, 0xB8, 0x01, 0x00, 0x4A, 0x08, 0x08, 0x01, 0x10, 0x01, 0x18, 0x00,
	0x20, 0x00, 0x52, 0x0B, 0x08, 0x64, 0x10, 0xDE, 0x0F, 0x18, 0x05, 0x20,
	0x00, 0x28, 0x00, 0xBD, 0x07, 0xCF, 0xDC
};

#define HASP_STATUS_OK 0
static unsigned int Hook_hasp_login(int feature_id, void* vendor_code, int hasp_handle) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_login\n");
#endif
	return HASP_STATUS_OK;
}

static unsigned int Hook_hasp_logout(int hasp_handle) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_logout\n");
#endif
	return HASP_STATUS_OK;
}

static unsigned int Hook_hasp_encrypt(int hasp_handle, unsigned char* buffer, unsigned int buffer_size) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_encrypt\n");
#endif
	return HASP_STATUS_OK;
}

static unsigned int Hook_hasp_decrypt(int hasp_handle, unsigned char* buffer, unsigned int buffer_size) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_decrypt\n");
#endif
	return HASP_STATUS_OK;
}

static unsigned int Hook_hasp_get_size(int hasp_handle, int hasp_fileid, unsigned int* hasp_size) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_get_size\n");
#endif
	* hasp_size = 0xD40; // Max addressable size by the game... absmax is 4k
	return HASP_STATUS_OK;
}

static unsigned int Hook_hasp_read(int hasp_handle, int hasp_fileid, unsigned int offset, unsigned int length, unsigned char* buffer) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_read\n");
#endif
	memcpy(buffer, hasp_buffer + offset, length);
	return HASP_STATUS_OK;
}

static unsigned int Hook_hasp_write(int hasp_handle, int hasp_fileid, unsigned int offset, unsigned int length, unsigned char* buffer) {
	return HASP_STATUS_OK;
}

typedef int (WINAPI* BIND)(SOCKET, CONST SOCKADDR*, INT);
static BIND pbind = NULL;

static unsigned int WINAPI Hook_bind(SOCKET s, const sockaddr* addr, int namelen) {
	sockaddr_in bindAddr = { 0 };
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_addr.s_addr = inet_addr("192.168.96.20");
	bindAddr.sin_port = htons(50765);
	if (addr == (sockaddr*)& bindAddr) {
		sockaddr_in bindAddr2 = { 0 };
		bindAddr2.sin_family = AF_INET;
		bindAddr2.sin_addr.s_addr = inet_addr(ipaddr);
		bindAddr2.sin_port = htons(50765);
		return pbind(s, (sockaddr*)& bindAddr2, namelen);
	}
	else {
		return pbind(s, addr, namelen);

	}
}

static unsigned char saveData[0x2000];
static bool saveOk = false;
static unsigned char carData[0xFF];

static int SaveOk()
{
	saveOk = true;
	return 1;
}

static char carFileName[0xFF];
static bool loadOk = false;
static bool customCar = false;

static uintptr_t SaveLocation = 0x1E183F8;

static int SaveGameData()
{
	if (!saveOk)
		return 1;

	memset(saveData, 0, 0x2000);
	uintptr_t value = *(uintptr_t*)(imageBase + SaveLocation);
	value = *(uintptr_t*)(value + 0x178);
	memcpy(saveData, (void*)value, 0x340);
	FILE* file = fopen("openprogress.sav", "wb");
	fwrite(saveData, 1, 0x2000, file);
	fclose(file);

	// Car Profile saving
	memset(carData, 0, 0xFF);
	memset(carFileName, 0, 0xFF);
	memcpy(carData, (void*) * (uintptr_t*)(*(uintptr_t*)(imageBase + SaveLocation) + 0x2D8), 0xE0);
	CreateDirectoryA("OpenParrot_Cars", nullptr);
	if (customCar)
	{
		sprintf(carFileName, ".\\OpenParrot_Cars\\custom.car");
	}
	else
	{
		sprintf(carFileName, ".\\OpenParrot_Cars\\%08X.car", *(DWORD*)(*(uintptr_t*)(*(uintptr_t*)(imageBase + SaveLocation) + 0x2D8) + 0x34));
	}
	FILE* carSave = fopen(carFileName, "wb");
	fwrite(carData, 1, 0xE0, file);
	fclose(carSave);

	FILE* fmileage = fopen("mileage.sav", "wb");
	fwrite(&mileageValue, 1, 0x4, fmileage);
	fclose(fmileage);

	saveOk = false;
	return 1;
}

static uintptr_t saveGameOffset;

static int LoadGameData()
{
	saveOk = false;
	memset(saveData, 0x0, 0x2000);
	FILE* file = fopen("openprogress.sav", "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		int fsize = ftell(file);
		if (fsize == 0x2000)
		{
			fseek(file, 0, SEEK_SET);
			fread(saveData, fsize, 1, file);
			uintptr_t value = *(uintptr_t*)(imageBase + SaveLocation);
			value = *(uintptr_t*)(value + 0x178);

			*(DWORD_PTR*)(value + 0x08) = *(DWORD_PTR*)(saveData + 0x08);
			*(DWORD_PTR*)(value + 0x18) = *(DWORD_PTR*)(saveData + 0x18);
			*(DWORD_PTR*)(value + 0x28) = *(DWORD_PTR*)(saveData + 0x28);
			*(DWORD_PTR*)(value + 0x38) = *(DWORD_PTR*)(saveData + 0x38);
			*(DWORD_PTR*)(value + 0x48) = *(DWORD_PTR*)(saveData + 0x48);
			*(DWORD_PTR*)(value + 0x60) = *(DWORD_PTR*)(saveData + 0x60);
			*(DWORD*)(value + 0x70) = *(DWORD*)(saveData + 0x70);
			*(DWORD_PTR*)(value + 0x74) = *(DWORD_PTR*)(saveData + 0x74);
			*(DWORD_PTR*)(value + 0x78) = *(DWORD_PTR*)(saveData + 0x78);
			*(DWORD_PTR*)(value + 0x80) = *(DWORD_PTR*)(saveData + 0x80);
			*(DWORD_PTR*)(value + 0x88) = *(DWORD_PTR*)(saveData + 0x88);
			*(DWORD_PTR*)(value + 0x98) = *(DWORD_PTR*)(saveData + 0x98);
			*(DWORD_PTR*)(value + 0xA8) = *(DWORD_PTR*)(saveData + 0xA8);
			*(DWORD_PTR*)(value + 0xB8) = *(DWORD_PTR*)(saveData + 0xB8);
			*(DWORD_PTR*)(value + 0xD8) = *(DWORD_PTR*)(saveData + 0xD8);
			*(DWORD_PTR*)(value + 0xE8) = *(DWORD_PTR*)(saveData + 0xE8);
			*(DWORD_PTR*)(value + 0xF0) = *(DWORD_PTR*)(saveData + 0xF0);
			*(DWORD_PTR*)(value + 0xF8) = *(DWORD_PTR*)(saveData + 0xF8);
			*(DWORD_PTR*)(value + 0x108) = *(DWORD_PTR*)(saveData + 0x108);
			*(DWORD_PTR*)(value + 0x110) = *(DWORD_PTR*)(saveData + 0x110);
			*(DWORD_PTR*)(value + 0x118) = *(DWORD_PTR*)(saveData + 0x118);
			*(DWORD_PTR*)(value + 0x128) = *(DWORD_PTR*)(saveData + 0x128);
			*(DWORD_PTR*)(value + 0x130) = *(DWORD_PTR*)(saveData + 0x130);
			*(DWORD_PTR*)(value + 0x138) = *(DWORD_PTR*)(saveData + 0x138);
			*(DWORD_PTR*)(value + 0x140) = *(DWORD_PTR*)(saveData + 0x140);
			*(DWORD_PTR*)(value + 0x148) = *(DWORD_PTR*)(saveData + 0x148);
			*(DWORD_PTR*)(value + 0x158) = *(DWORD_PTR*)(saveData + 0x158);
			*(DWORD_PTR*)(value + 0x160) = *(DWORD_PTR*)(saveData + 0x160);
			*(DWORD_PTR*)(value + 0x168) = *(DWORD_PTR*)(saveData + 0x168);
			*(DWORD_PTR*)(value + 0x170) = *(DWORD_PTR*)(saveData + 0x170);
			*(DWORD_PTR*)(value + 0x180) = *(DWORD_PTR*)(saveData + 0x180);
			*(DWORD_PTR*)(value + 0x188) = *(DWORD_PTR*)(saveData + 0x188);
			*(DWORD_PTR*)(value + 0x198) = *(DWORD_PTR*)(saveData + 0x198);
			*(DWORD_PTR*)(value + 0x1A8) = *(DWORD_PTR*)(saveData + 0x1A8);
			*(DWORD_PTR*)(value + 0x1B0) = *(DWORD_PTR*)(saveData + 0x1B0);
			*(DWORD_PTR*)(value + 0x1C0) = *(DWORD_PTR*)(saveData + 0x1C0);
			*(DWORD_PTR*)(value + 0x1D0) = *(DWORD_PTR*)(saveData + 0x1D0);
			*(DWORD_PTR*)(value + 0x1D8) = *(DWORD_PTR*)(saveData + 0x1D8);
			*(DWORD_PTR*)(value + 0x1E8) = *(DWORD_PTR*)(saveData + 0x1E8);
			*(DWORD_PTR*)(value + 0x1F8) = *(DWORD_PTR*)(saveData + 0x1F8);
			*(DWORD_PTR*)(value + 0x200) = *(DWORD_PTR*)(saveData + 0x200);
			*(DWORD_PTR*)(value + 0x210) = *(DWORD_PTR*)(saveData + 0x210);
			*(DWORD_PTR*)(value + 0x220) = *(DWORD_PTR*)(saveData + 0x220);
			*(DWORD_PTR*)(value + 0x228) = *(DWORD_PTR*)(saveData + 0x228);
			*(DWORD_PTR*)(value + 0x238) = *(DWORD_PTR*)(saveData + 0x238);
			*(DWORD_PTR*)(value + 0x248) = *(DWORD_PTR*)(saveData + 0x248);
			*(DWORD_PTR*)(value + 0x250) = *(DWORD_PTR*)(saveData + 0x250);
			*(DWORD_PTR*)(value + 0x260) = *(DWORD_PTR*)(saveData + 0x260);
			*(DWORD_PTR*)(value + 0x278) = *(DWORD_PTR*)(saveData + 0x278);
			*(DWORD_PTR*)(value + 0x288) = *(DWORD_PTR*)(saveData + 0x288);
			*(DWORD_PTR*)(value + 0x298) = *(DWORD_PTR*)(saveData + 0x298);
			*(DWORD_PTR*)(value + 0x2A8) = *(DWORD_PTR*)(saveData + 0x2A8);

			loadOk = true;
		}
		fclose(file);
	}

	FILE* fmileage = fopen("mileage.sav", "rb");
	if (fmileage)
	{
		fseek(fmileage, 0, SEEK_END);
		int fsize = ftell(fmileage);
		if (fsize == 4)
		{
			fseek(fmileage, 0, SEEK_SET);
			fread(&mileageValue, 4, 1, fmileage);
		}
		fclose(fmileage);
	}
	return 1;
}

static BOOL FileExists(char* szPath)
{
	DWORD dwAttrib = GetFileAttributesA(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

static DWORD WINAPI SpamCarTuneNeons(LPVOID)
{
	while (true)
	{
		Sleep(16);

		uintptr_t carSaveLocation = *(uintptr_t*)((*(uintptr_t*)(imageBase + SaveLocation)) + 0x2D8);

		if (ForceNeon)
		injector::WriteMemory<BYTE>(carSaveLocation + 0x60, NeonColour, true);

		if (ForceFullTune)
		{
			injector::WriteMemory<BYTE>(carSaveLocation + 0x74, 0x11, true);
			injector::WriteMemory<BYTE>(carSaveLocation + 0x80, 0x11, true);
		}	
	}
}

static void LoadWmmt5CarData()
{
	if (!loadOk)
		return;
	customCar = false;
	memset(carData, 0, 0xFF);
	memset(carFileName, 0, 0xFF);
	CreateDirectoryA("OpenParrot_Cars", nullptr);

	// check for custom car
	sprintf(carFileName, ".\\OpenParrot_Cars\\custom.car");
	if (FileExists(carFileName))
	{
		FILE* file = fopen(carFileName, "rb");
		if (file)
		{
			fseek(file, 0, SEEK_END);
			int fsize = ftell(file);
			if (fsize == 0xE0)
			{
				fseek(file, 0, SEEK_SET);
				fread(carData, fsize, 1, file);
				uintptr_t carSaveLocation = *(uintptr_t*)((*(uintptr_t*)(imageBase + SaveLocation)) + 0x2D8);
				memcpy((void*)(carSaveLocation + 0x08), carData + 0x08, 8);
				memcpy((void*)(carSaveLocation + 0x18), carData + 0x18, 8);
				memcpy((void*)(carSaveLocation + 0x28), carData + 0x28, 8);
				memcpy((void*)(carSaveLocation + 0x30), carData + 0x30, 8);
				memcpy((void*)(carSaveLocation + 0x40), carData + 0x40, 8);
				memcpy((void*)(carSaveLocation + 0x48), carData + 0x48, 8);
				memcpy((void*)(carSaveLocation + 0x58), carData + 0x58, 8);
				memcpy((void*)(carSaveLocation + 0x60), carData + 0x60, 8);
				memcpy((void*)(carSaveLocation + 0x68), carData + 0x68, 8);
				memcpy((void*)(carSaveLocation + 0x70), carData + 0x70, 8);
				memcpy((void*)(carSaveLocation + 0x80), carData + 0x80, 8);
				memcpy((void*)(carSaveLocation + 0x90), carData + 0x90, 8);
				memcpy((void*)(carSaveLocation + 0x98), carData + 0x98, 8);
				memcpy((void*)(carSaveLocation + 0xB0), carData + 0xB0, 8);
				memcpy((void*)(carSaveLocation + 0xC8), carData + 0xC8, 8);
				customCar = true;
			}
			loadOk = false;
			fclose(file);
			return;
		}
	}

	memset(carFileName, 0, 0xFF);
	// Load actual car if available
	sprintf(carFileName, ".\\OpenParrot_Cars\\%08X.car", *(DWORD*)(*(uintptr_t*)(*(uintptr_t*)(imageBase + SaveLocation) + 0x2D8) + 0x34));
	if (FileExists(carFileName))
	{
		FILE* file = fopen(carFileName, "rb");
		if (file)
		{
			fseek(file, 0, SEEK_END);
			int fsize = ftell(file);
			if (fsize == 0xE0)
			{
				fseek(file, 0, SEEK_SET);
				fread(carData, fsize, 1, file);
				uintptr_t carSaveLocation = *(uintptr_t*)((*(uintptr_t*)(imageBase + SaveLocation)) + 0x2D8);
				memcpy((void*)(carSaveLocation + 0x08), carData + 0x08, 8);
				memcpy((void*)(carSaveLocation + 0x18), carData + 0x18, 8);
				memcpy((void*)(carSaveLocation + 0x28), carData + 0x28, 8);
				memcpy((void*)(carSaveLocation + 0x30), carData + 0x30, 8);
				memcpy((void*)(carSaveLocation + 0x40), carData + 0x40, 8);
				memcpy((void*)(carSaveLocation + 0x48), carData + 0x48, 8);
				memcpy((void*)(carSaveLocation + 0x58), carData + 0x58, 8);
				memcpy((void*)(carSaveLocation + 0x60), carData + 0x60, 8);
				memcpy((void*)(carSaveLocation + 0x68), carData + 0x68, 8);
				memcpy((void*)(carSaveLocation + 0x70), carData + 0x70, 8);
				memcpy((void*)(carSaveLocation + 0x80), carData + 0x80, 8);
				memcpy((void*)(carSaveLocation + 0x90), carData + 0x90, 8);
				memcpy((void*)(carSaveLocation + 0x98), carData + 0x98, 8);
				memcpy((void*)(carSaveLocation + 0xB0), carData + 0xB0, 8);
				memcpy((void*)(carSaveLocation + 0xC8), carData + 0xC8, 8);
			}
			fclose(file);
		}
	}
	if (ForceFullTune || ForceNeon)
	{
		if (!CarTuneNeonThread)
		{
			CarTuneNeonThread = true;
			CreateThread(0, 0, SpamCarTuneNeons, 0, 0, 0);
		}
	}
	loadOk = false;
}

static int ReturnTrue()
{
	return 1;
}

static BYTE GenerateChecksum(unsigned char *myArray, int index, int length)
{
	BYTE crc = 0;
	for(int i = 0; i < length; i++)
	{
		crc += myArray[index + i];
	}
	return crc & 0xFF;
}

static void GenerateDongleData(bool isTerminal)
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
	if (isTerminal)
	{
		memcpy(hasp_buffer + 0xD00, "280811990002", 12); // not sure these are OK, since its from google lol.
		hasp_buffer[0xD3E] = GenerateChecksum(hasp_buffer, 0xD00, 62);
		hasp_buffer[0xD3F] = hasp_buffer[0xD3E] ^ 0xFF;
	}
	else
	{
		memcpy(hasp_buffer + 0xD00, "280813990002", 12);
		hasp_buffer[0xD3E] = GenerateChecksum(hasp_buffer, 0xD00, 62);
		hasp_buffer[0xD3F] = hasp_buffer[0xD3E] ^ 0xFF;
	}
}

static char customName[256];


static DWORD WINAPI SpamCustomName(LPVOID)
{
	while (true)
	{
		Sleep(50);
		void* value = (void*)(imageBase + 0x1E19EE0);
		memcpy(value, customName, strlen(customName) + 1);
	}
}

static DWORD WINAPI SpamMulticast(LPVOID)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	int ttl = 255;
	setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (char*)& ttl, sizeof(ttl));

	int reuse = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)& reuse, sizeof(reuse));

	setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)& reuse, sizeof(reuse));

	sockaddr_in bindAddr = { 0 };
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_addr.s_addr = inet_addr(ipaddr);
	bindAddr.sin_port = htons(50765);
	bind(sock, (sockaddr*)& bindAddr, sizeof(bindAddr));


	ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr("225.0.0.1");
	mreq.imr_interface.s_addr = inet_addr(ipaddr);

	setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)& mreq, sizeof(mreq));

	const uint8_t* byteSequences_Free[] = {
		terminalPackage1_Free,
		terminalPackage2_Free,
		terminalPackage3_Free,
		terminalPackage4_Free,
		terminalPackage5_Free,
		terminalPackage6_Free,
	};

	const size_t byteSizes_Free[] = {
		sizeof(terminalPackage1_Free),
		sizeof(terminalPackage2_Free),
		sizeof(terminalPackage3_Free),
		sizeof(terminalPackage4_Free),
		sizeof(terminalPackage5_Free),
		sizeof(terminalPackage6_Free),
	};

	const uint8_t* byteSequences_Coin[] = {
		terminalPackage1_Coin,
		terminalPackage2_Coin,
		terminalPackage3_Coin,
		terminalPackage4_Coin,
		terminalPackage5_Coin,
		terminalPackage6_Coin,
	};

	const size_t byteSizes_Coin[] = {
		sizeof(terminalPackage1_Coin),
		sizeof(terminalPackage2_Coin),
		sizeof(terminalPackage3_Coin),
		sizeof(terminalPackage4_Coin),
		sizeof(terminalPackage5_Coin),
		sizeof(terminalPackage6_Coin),
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
		while (true) for (int i = 0; i < _countof(byteSequences_Free); i++)
		{
			sendto(sock, (const char*)byteSequences_Free[i], byteSizes_Free[i], 0, (sockaddr*)&toAddr, sizeof(toAddr));
			Sleep(8);
		}		
	}

	while (true) for (int i = 0; i < _countof(byteSequences_Coin); i++)
	{
		sendto(sock, (const char*)byteSequences_Coin[i], byteSizes_Coin[i], 0, (sockaddr*)& toAddr, sizeof(toAddr));
		Sleep(8);
	}
}

extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;

static __int64(__fastcall* g_origMileageFix)(__int64);

static __int64 __fastcall MileageFix(__int64 a1)
{
	*(DWORD*)(a1 + 224) = mileageValue;
	auto result = g_origMileageFix(a1);
	mileageValue += *(DWORD*)(a1 + 228);
	return result;
}

static InitFunction Wmmt6Func([]()
{
	// folder for path redirections
	CreateDirectoryA(".\\TP", nullptr);

	FILE* fileF = _wfopen(L".\\TP\\setting.lua.gz", L"r");
	if (fileF == NULL)
	{
		FILE* settingsF = _wfopen(L".\\TP\\setting.lua.gz", L"wb");
		fwrite(settingData, 1, sizeof(settingData), settingsF);
		fclose(settingsF);
	}
	else
	{
		fclose(fileF);
	}

	bool isTerminal = false;
	if (ToBool(config["General"]["TerminalMode"]))
	{
		isTerminal = true;
	}

	std::string networkip = config["General"]["NetworkAdapterIP"];
	if (!networkip.empty())
	{
		ipaddr = networkip.c_str();
	}

	hookPort = "COM3";
	imageBase = (uintptr_t)GetModuleHandleA(0);
	MH_Initialize();

	// Hook dongle funcs
	MH_CreateHookApi(L"hasp_windows_x64_28756.dll", "hasp_write", Hook_hasp_write, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_28756.dll", "hasp_read", Hook_hasp_read, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_28756.dll", "hasp_get_size", Hook_hasp_get_size, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_28756.dll", "hasp_decrypt", Hook_hasp_decrypt, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_28756.dll", "hasp_encrypt", Hook_hasp_encrypt, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_28756.dll", "hasp_logout", Hook_hasp_logout, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_28756.dll", "hasp_login", Hook_hasp_login, NULL);
	MH_CreateHookApi(L"WS2_32", "bind", Hook_bind, reinterpret_cast<LPVOID*>(&pbind));
	MH_CreateHook((void*)(imageBase + 0x35AAC0), MileageFix, (void**)&g_origMileageFix);

	GenerateDongleData(isTerminal);

	injector::WriteMemory<uint8_t>(hook::get_pattern("0F 94 C0 84 C0 0F 94 C0 84 C0 75 05 45 32 ? EB", 0x13), 0, true);

	// Skip weird camera init that stucks entire pc on certain brands. TESTED ONLY ON 05!!!!
	if (ToBool(config["General"]["WhiteScreenFix"]))
	{
		injector::WriteMemory<DWORD>(hook::get_pattern("48 8B C4 55 57 41 54 41 55 41 56 48 8D 68 A1 48 81 EC 90 00 00 00 48 C7 45 D7 FE FF FF FF 48 89 58 08 48 89 70 18 45 33 F6 4C 89 75 DF 33 C0 48 89 45 E7", 0), 0x90C3C032, true);
	}

	injector::MakeNOP(hook::get_pattern("45 33 C0 BA 65 09 00 00 48 8D 4D B0 E8 ? ? ? ? 48 8B 08", 12), 5);

	auto location = hook::get_pattern<char>("48 83 EC 28 33 D2 B9 70 00 02 00 E8 ? ? ? ? 85 C0 79 06");
	injector::WriteMemory<uint8_t>(location + 0x12, 0xEB, true);

	// First auth error skip
	injector::WriteMemory<BYTE>(imageBase + 0x6A0077, 0xEB, true);
	
	if (isTerminal)
	{
		safeJMP(hook::get_pattern("0F B6 41 05 2C 30 3C 09 77 04 0F BE C0 C3 83 C8 FF C3"), ReturnTrue);
	}
	else
	{
		injector::WriteMemory<WORD>(imageBase + 0x6A0C87, 0x00D1, true);		
		injector::WriteMemory<BYTE>(imageBase + 0x20B88A, 0x90, true);
		injector::WriteMemory<BYTE>(imageBase + 0x20B88B, 0x90, true);
		injector::WriteMemory<BYTE>(imageBase + 0x20B89B, 0x90, true);
		injector::WriteMemory<BYTE>(imageBase + 0x20B89C, 0x90, true);
		injector::WriteMemory<BYTE>(imageBase + 0x20B8A1, 0x90, true);
		injector::WriteMemory<BYTE>(imageBase + 0x20B8A2, 0x90, true);

		// spam thread
		if (ToBool(config["General"]["TerminalEmulator"]))
		{
			CreateThread(0, 0, SpamMulticast, 0, 0, 0);
		}
	}

	// Enable all print
	injector::WriteMemory<BYTE>(imageBase + 0x8B6B23, 0xEB, true);

	// path fixes
	injector::WriteMemoryRaw(imageBase + 0x12C5248, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x12C5268, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x12C5288, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x12C52A8, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x12C52C8, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x12C52E8, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x12C5308, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x12C5328, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x12C5348, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x12C5360, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A708, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A720, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A738, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A760, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A788, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A7A0, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A7B8, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A7C8, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A7D8, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A7F0, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A808, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A828, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A848, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A858, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A868, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A880, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A898, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A8B0, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A8C8, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A8E0, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A8F8, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A910, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A928, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x135A940, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x1362D48, "TP", 2, true); // F:/contents/
	injector::WriteMemoryRaw(imageBase + 0x13B2890, "TP/contents/", 12, true); // F:contents/
	injector::WriteMemoryRaw(imageBase + 0x13B28A0, "TP/contents/", 12, true);	// G:contents/
	injector::WriteMemoryRaw(imageBase + 0x1401030, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x1401048, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x1401E08, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x1401E20, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x1401E38, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x1401E60, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x1401E88, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x1401EA0, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x14028E0, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x1402900, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x1401DDC, "TP", 2, true); // F:
	injector::WriteMemoryRaw(imageBase + 0x13652B8, "TP", 2, true);
	injector::WriteMemoryRaw(imageBase + 0x1365AC8, "TP", 2, true);

	if (ToBool(config["General"]["SkipMovies"]))
	{
		// Skip movies fuck you wmmt5
		safeJMP(imageBase + 0x9A79D0, ReturnTrue);
	}

	std::string value = config["General"]["CustomName"];
	if (!value.empty())
	{
		if (value.size() > 5)
		{
			memset(customName, 0, 256);
			strcpy(customName, value.c_str());
			CreateThread(0, 0, SpamCustomName, 0, 0, 0);
		}

		injector::WriteMemory<BYTE>(imageBase + 0x10942E8, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10F5428, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B3EB0, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B75A0, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12CE688, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4BF0, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C00, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C10, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10942EA, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10F542A, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B3EB2, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B75A2, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12CE68A, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4BF2, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C02, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C12, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10942EC, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10F542C, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B3EB4, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B75A4, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12CE68C, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4BF4, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C04, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C14, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10942EE, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10F542E, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B3EB6, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B75A6, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12CE68E, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4BF6, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C06, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C16, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10942F0, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10F5430, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B3EB8, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B75A8, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12CE690, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4BF8, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C08, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C18, 0xFF, true);

		char NameChar;
		for (int i = 0; i < value.size(); i++) {
			NameChar = value.at(i) - 0x20;

			switch (i)
			{
			case 0x00:
				injector::WriteMemory<BYTE>(imageBase + 0x10942E8, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x10F5428, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x12B3EB0, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x12B75A0, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x12CE688, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x13C4BF0, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x13C4C00, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x13C4C10, NameChar, true);
				break;
			case 0x01:
				injector::WriteMemory<BYTE>(imageBase + 0x10942EA, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x10F542A, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x12B3EB2, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x12B75A2, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x12CE68A, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x13C4BF2, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x13C4C02, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x13C4C12, NameChar, true);
				break;
			case 0x02:
				injector::WriteMemory<BYTE>(imageBase + 0x10942EC, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x10F542C, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x12B3EB4, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x12B75A4, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x12CE68C, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x13C4BF4, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x13C4C04, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x13C4C14, NameChar, true);
				break;
			case 0x03:
				injector::WriteMemory<BYTE>(imageBase + 0x10942EE, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x10F542E, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x12B3EB6, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x12B75A6, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x12CE68E, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x13C4BF6, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x13C4C06, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x13C4C16, NameChar, true);
				break;
			case 0x04:
				injector::WriteMemory<BYTE>(imageBase + 0x10942F0, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x10F5430, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x12B3EB8, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x12B75A8, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x12CE690, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x13C4BF8, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x13C4C08, NameChar, true);
				injector::WriteMemory<BYTE>(imageBase + 0x13C4C18, NameChar, true);
				break;
			}
		}
		injector::WriteMemory<BYTE>(imageBase + 0x10942E9, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10942EB, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10942ED, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10942EF, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10942F1, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10F5429, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10F542B, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10F542D, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10F542F, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x10F5431, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B3EB1, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B3EB3, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B3EB5, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B3EB7, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B3EB9, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B75A1, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B75A3, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B75A5, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B75A7, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12B75A9, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12CE689, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12CE68B, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12CE68D, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12CE68F, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x12CE691, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4BF1, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4BF3, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4BF5, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4BF7, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4BF9, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C01, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C03, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C05, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C07, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C09, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C11, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C13, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C15, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C17, 0xFF, true);
		injector::WriteMemory<BYTE>(imageBase + 0x13C4C19, 0xFF, true);
	}

	ForceFullTune = (ToBool(config["Tune"]["Force Full Tune"]));
	ForceNeon = (ToBool(config["Tune"]["Force Neon"]));

	if (ForceNeon)
	{
		if (strcmp(config["Tune"]["Select Neon"].c_str(), "Green") == 0)
			NeonColour = 0x01;
		if (strcmp(config["Tune"]["Select Neon"].c_str(), "Blue") == 0)
			NeonColour = 0x02;
		if (strcmp(config["Tune"]["Select Neon"].c_str(), "Red") == 0)
			NeonColour = 0x03;
		if (strcmp(config["Tune"]["Select Neon"].c_str(), "Yellow") == 0)
			NeonColour = 0x04;
		if (strcmp(config["Tune"]["Select Neon"].c_str(), "Purple") == 0)
			NeonColour = 0x05;
		if (strcmp(config["Tune"]["Select Neon"].c_str(), "Green Pattern") == 0)
			NeonColour = 0x06;
		if (strcmp(config["Tune"]["Select Neon"].c_str(), "Blue Pattern") == 0)
			NeonColour = 0x07;
		if (strcmp(config["Tune"]["Select Neon"].c_str(), "Red Pattern") == 0)
			NeonColour = 0x08;
		if (strcmp(config["Tune"]["Select Neon"].c_str(), "Yellow Pattern") == 0)
			NeonColour = 0x09;
		if (strcmp(config["Tune"]["Select Neon"].c_str(), "Purple Pattern") == 0)
			NeonColour = 0x0A;
	}

	// Fix dongle error (can be triggered by various USB hubs, dongles
	injector::MakeNOP(imageBase + 0x8C140F, 2, true);

	// Save story stuff (only 05)
	{
		// skip erasing of temp card data
		injector::WriteMemory<uint8_t>(imageBase + 0xA54F13, 0xEB, true);
		// Skip erasing of temp card
		safeJMP(imageBase + 0x647FB0, LoadGameData);
		safeJMP(imageBase + 0x65ED40, ReturnTrue);
		safeJMP(imageBase + 0x682A00, ReturnTrue);
		safeJMP(imageBase + 0x68CD40, ReturnTrue);

		safeJMP(imageBase + 0xACEA10, ReturnTrue);
		safeJMP(imageBase + 0x65F1F0, ReturnTrue);
		safeJMP(imageBase + 0x6856F0, ReturnTrue);

		// Skip more
		safeJMP(imageBase + 0x641950, ReturnTrue);
		safeJMP(imageBase + 0xACDCE0, ReturnTrue);
		safeJMP(imageBase + 0x6B7030, ReturnTrue);
		safeJMP(imageBase + 0x6C73D0, ReturnTrue);
		safeJMP(imageBase + 0xA85F20, ReturnTrue);
		safeJMP(imageBase + 0x64F600, ReturnTrue);
		safeJMP(imageBase + 0x61BD00, ReturnTrue);

		safeJMP(imageBase + 0x6C8818, LoadWmmt5CarData);

		// Save progress trigger
		injector::WriteMemory<WORD>(imageBase + 0x655154, 0xB848, true);
		injector::WriteMemory<uintptr_t>(imageBase + 0x655154 + 2, (uintptr_t)SaveOk, true);
		injector::WriteMemory<DWORD>(imageBase + 0x655154 + 0xA, 0x9090D0FF, true);

		// Try save later!
		injector::MakeNOP(imageBase + 0x399A56, 0x12);
		injector::WriteMemory<WORD>(imageBase + 0x399A56, 0xB848, true);
		injector::WriteMemory<uintptr_t>(imageBase + 0x399A56 + 2, (uintptr_t)SaveGameData, true);
		injector::WriteMemory<DWORD>(imageBase + 0x399A60, 0x3348D0FF, true);
		injector::WriteMemory<WORD>(imageBase + 0x399A60 + 4, 0x90C0, true);
	}

	MH_EnableHook(MH_ALL_HOOKS);
}, GameID::WMMT6);
#endif
#pragma optimize("", on)