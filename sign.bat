@echo off
if "%cenc%"=="" goto end
%comspec% /k "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
IF %PLATFORM%==Win32 (
     signtool sign /t http://timestamp.globalsign.com/scripts/timstamp.dll /f "cert" /p %cenc% /d .\build\bin\release\output\OpenParrot.dll .\build\bin\release\output\OpenParrot.dll
     signtool sign /t http://timestamp.globalsign.com/scripts/timstamp.dll /f "cert" /p %cenc% /d .\build\bin\release\output\OpenParrotLoader.exe .\build\bin\release\output\OpenParrotLoader.exe
     signtool sign /t http://timestamp.globalsign.com/scripts/timstamp.dll /f "cert" /p %cenc% /d .\build\bin\release\output\OpenParrotKonamiLoader.exe .\build\bin\release\output\OpenParrotKonamiLoader.exe) ELSE (IF %PLATFORM%==x64 (
      signtool sign /t http://timestamp.globalsign.com/scripts/timstamp.dll /f "cert" /p %cenc% /d .\build\bin\release\output\OpenParrot64.dll .\build\bin\release\output\OpenParrot64.dll
      signtool sign /t http://timestamp.globalsign.com/scripts/timstamp.dll /f "cert" /p %cenc% /d .\build\bin\release\output\OpenParrotLoader64.exe .\build\bin\release\output\OpenParrotLoader64.exe
    )
	)
exit
	
:end
echo Pull Request
exit