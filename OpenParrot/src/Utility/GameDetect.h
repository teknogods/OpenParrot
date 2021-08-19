#include "GameID.h"
#include "Functions/Nesica_Libs/CryptoPipe.h"
#include "Functions/Types.h"

class GameDetect
{
public:
	static GameID currentGame;
	static void DetectCurrentGame();
	static void DetectCurrentLinuxGame();
	static X2Type X2Type;
	static bool IsTypeX();
	static bool IsNesicaGame();
	static void SetGameId(GameID gameId, char* name);
	static NesicaKey NesicaKey;
	static std::string GetGameName();
	static std::string GameDetect::GetGameName(GameID game);
	static bool enableNesysEmu;

private:
	static bool isNesica;
};
