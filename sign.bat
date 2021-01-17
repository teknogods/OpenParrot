@echo off
if "%cenc%"=="" goto end
%comspec% /k "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
IF %PLATFORM%==Win32 (
     C:\Program Files (x86)\Windows Kits\10\bin\10.0.18362.0\x64\signtool.exe sign /t http://timestamp.globalsign.com/scripts/timstamp.dll /f "cert" /p %cenc% /d .\build\bin\release\output\OpenParrot.dll .\build\bin\release\output\OpenParrot.dll
     C:\Program Files (x86)\Windows Kits\10\bin\10.0.18362.0\x64\signtool.exe sign /t http://timestamp.globalsign.com/scripts/timstamp.dll /f "cert" /p %cenc% /d .\build\bin\release\output\OpenParrotLoader.exe .\build\bin\release\output\OpenParrotLoader.exe
     C:\Program Files (x86)\Windows Kits\10\bin\10.0.18362.0\x64\signtool.exe sign /t http://timestamp.globalsign.com/scripts/timstamp.dll /f "cert" /p %cenc% /d .\build\bin\release\output\OpenParrotKonamiLoader.exe .\build\bin\release\output\OpenParrotKonamiLoader.exe) ELSE (IF %PLATFORM%==x64 (
      C:\Program Files (x86)\Windows Kits\10\bin\10.0.18362.0\x64\signtool.exe sign /t http://timestamp.globalsign.com/scripts/timstamp.dll /f "cert" /p %cenc% /d .\build\bin\release\output\OpenParrot64.dll .\build\bin\release\output\OpenParrot64.dll
      C:\Program Files (x86)\Windows Kits\10\bin\10.0.18362.0\x64\signtool.exe sign /t http://timestamp.globalsign.com/scripts/timstamp.dll /f "cert" /p %cenc% /d .\build\bin\release\output\OpenParrotLoader64.exe .\build\bin\release\output\OpenParrotLoader64.exe
    )
	)
exit
	
:end
echo Pull Request
exit