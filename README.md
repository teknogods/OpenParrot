# OpenParrot

REQUIRED FOR THIS PROJECT: https://github.com/teknogods/TeknoParrotUI

HOW TO COMPILE (VISUAL STUDIO):
1. Execute premake.bat for vs2017, for older versions: please edit the bat accordingly.
2. Open in Visual Studio via .sln and compile. (x86 or x64) 

How to easiest work on new titles: (For devs)
1. Run TeknoParrotUi with command line with some profile that is compatible with your game or create a new XML with according buttons / jvs modes. (TeknoParrotUi.exe --profile=ggxx.xml very good for stick games)
2. Compile OpenParrot with #define DEVMODE uncommented in StdInc.h
3. Copy compiled OpenParrot / OpenParrot64 dll to the game directory.
4. Open game executable in x64dbg ( https://x64dbg.com/#start )
5. Write loadlib OpenParrot / loadlib OpenParrot64 in x64dbg command textbox.
6. The dll should now be loaded and you can work on the game.
7. Remember to add signature of the game to the GameDetect.cpp (Either PE CRC or byte pattern check. Check already existing ones)