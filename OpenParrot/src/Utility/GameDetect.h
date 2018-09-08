#include "GameID.h"
#include "Functions/Nesica_Libs/CryptoPipe.h"
#include "Functions/Types.h"

class GameDetect
{
public:
	static GameID currentGame;
	static void DetectCurrentGame();
	static X2Type X2Type;
	static bool IsTypeX();
	static bool IsNesicaGame();
	static NesicaKey NesicaKey;
	static std::string GetGameName();
	static std::string GameDetect::GetGameName(GameID game);

private:
	static bool isNesica;
};
