
SET HEADER_FILE=header_info.ini
REM ªÒ»°∞Ê±æ∫≈
for /f "tokens=2,3" %%i in (../shrd100_app/src/cfg/version.h) do (
	if "%%i" == "EMBED_SOFTWARE_PS_VERSION_STR" (
		SET VERSION=%%j
   ) 
)
echo %VERSION%
SET filepath=../shrd100_app_system/_ide/bootimage/%VERSION:~1,-1%.bin
echo %filepath%

python local_packaging.py %HEADER_FILE% %filepath% %VERSION:~1,-1%

pause



