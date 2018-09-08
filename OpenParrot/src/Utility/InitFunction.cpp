#include <StdInc.h>
#include "InitFunction.h"

#include <forward_list>

std::forward_list<InitFunction*>* g_initFunctions;

InitFunction::InitFunction(void(*callback)(), GameID gameID)
	: callback(callback), game(gameID)
{
	if (!g_initFunctions)
	{
		g_initFunctions = new std::forward_list<InitFunction*>();
	}

	g_initFunctions->push_front(this);
}

void InitFunction::RunFunctions(GameID game)
{
	for (auto& it : *g_initFunctions)
	{
		if (it->game == game)
		{
			it->callback();
		}
	}
}