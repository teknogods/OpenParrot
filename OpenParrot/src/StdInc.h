#pragma once

// os includes
//#define DEVMODE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <ws2tcpip.h>

// lib includes
#include <stdint.h>

// our includes
#include "injector/injector.hpp"
#include "injector/calling.hpp"
#include "Utility/Utils.h"
#include "ini_parser.hpp"

extern linb::ini config;