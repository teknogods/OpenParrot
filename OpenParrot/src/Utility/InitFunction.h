#pragma once

#include "GameID.h"

class InitFunction
{
public:
	InitFunction(void(*callback)(), GameID game = GameID::Global);

	static void RunFunctions(GameID game);

private:
	void(*callback)();

	GameID game;
};
