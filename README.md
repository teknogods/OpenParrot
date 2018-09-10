# OpenParrot

REQUIRED FOR THIS PROJECT: https://github.com/teknogods/TeknoParrotUI

HOW TO COMPILE (VISUAL STUDIO):
1. Execute premake.bat for vs2017, for older versions: please edit the bat accordingly.
2. Open in Visual Studio via .sln and compile. (x86 or x64) 

How to easiest work on new titles: (For devs) (See STREAM: https://www.twitch.tv/videos/308359681 )
1. Run TeknoParrotUi with command line with some profile that is compatible with your game or create a new XML with according buttons / jvs modes. (TeknoParrotUi.exe --profile=ggxx.xml very good for stick games)
2. Open game executable in x64dbg ( https://x64dbg.com/#start )
3. Write loadlib OpenParrot / loadlib OpenParrot64 in x64dbg command textbox with full directory to OpenParrot. (See STREAM: https://www.twitch.tv/videos/308359681 )
6. The dll should now be loaded and you can work on the game.
7. Remember to add signature of the game to the GameDetect.cpp (Either PE CRC or byte pattern check. Check already existing ones)