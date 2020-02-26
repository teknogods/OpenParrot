#include "GameID.h"
#include "Functions/Nesica_Libs/CryptoPipe.h"
#include "Functions/Types.h"

class GameDetect
{
public:
	static GameID currentGame;
	static void DetectCurrentGame(bool konami = false);
	static X2Type X2Type;
	static KonamiGame KonamiGame;
	static bool IsTypeX();
	static bool IsNesicaGame();
	static NesicaKey NesicaKey;
	static std::string GetGameName();
	static std::string GameDetect::GetGameName(GameID game);
	static bool enableNesysEmu;
	static bool file_exists(const std::string& name);

private:
	static bool isNesica;
};
