# OpenParrot

Open Source version of TeknoParrot by NTA, avail and Reaver. Works in collaboration with [TeknoParrotUI](https://github.com/teknogods/TeknoParrotUI).

[TeknoParrot Discord](https://discord.gg/kmWgGDe), development discussion is in the ``#openparrot-dev`` channel.

[Bounties](https://docs.google.com/spreadsheets/d/1gC2vCqWq93Blmg6XsbVYxEsMU3y1p6WPzbnkbtrg_k0/edit?usp=sharing)

# Compiling (VS 2019)

1. Execute ``premake.bat``, for older versions, please edit the bat accordingly.
2. Open in Visual Studio via .sln and compile. (x86 or x64)

# Adding support for new games

See STREAM: https://www.twitch.tv/videos/308359681 

1. Run TeknoParrotUi with command line with some profile that is compatible with your game or create a new GameProfile with according buttons / JVS modes. (``TeknoParrotUi.exe --profile=ggxx.xml`` is very good for stick games)
2. Open game executable in [x64dbg](https://x64dbg.com/#start)
3. Write ``loadlib OpenParrot`` (or ``OpenParrot64``) in x64dbg command textbox with full directory to OpenParrot.
6. The dll should now be loaded and you can work on the game.
7. Remember to add signature of the game to ``GameDetect.cpp`` (Either PE CRC or byte pattern check. Check already existing ones)
