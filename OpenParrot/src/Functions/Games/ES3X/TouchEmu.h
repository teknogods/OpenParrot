#pragma once
#include <Windows.h>

static HANDLE touchDevice;

void mt6SetTouchData(LPARAM lParam, BOOL down, BOOL isTouchScreen);
void mt6SetDisplayParams(HWND hwnd);
void mt6SerialTouchInit();