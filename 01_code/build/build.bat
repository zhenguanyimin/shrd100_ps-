@echo off
cd ..
SET SRC_WORKSPACE=%cd:\=/%
SET VITIS_HOME=%VITIS_HOME:\=/%
SET QEMUDATA=%VITIS_HOME%/data/emulation/platforms/zynqmp/sw/a53_standalone/qemu
SET SPR=%SRC_WORKSPACE%/shrd100_plt/platform.spr
SET HEADER_FILE=%SRC_WORKSPACE%/build/header_info.ini
SET XSA_DIR=%cd%\hw_spec

REM ɾ��releaseĿ¼�������ļ�����������
if exist build\release rd /S /Q build\release
	mkdir build\release

REM ��ȡvitis workspace
for /f "tokens=1,2 delims==" %%i in (build/conf.properties) do (
	set VITIS_WORKSPACE=%%j
)

REM ��ȡ�汾��
for /f "tokens=2,3" %%i in (shrd100_app/src/cfg/version.h) do (
	if "%%i" == "EMBED_SOFTWARE_PS_VERSION_STR" (
		SET VERSION=%%j
   ) 
)

REM ��ȡXSA�ļ���
for /f "delims=:" %%a in ('dir /b %XSA_DIR%\*.xsa ^| findstr /n .*') do SET count=%%a
	if %count% equ 1 (
		for /f "delims=" %%i in ('dir /s /b %XSA_DIR%\*.xsa') do SET XSA_FILE=%%~nxi
	) else (
		color cf & echo ��⵽���xsa�ļ���ϵͳֻ����һ��xsa�ļ���
		pause
		exit 1
	)

REM ��ȡbif�ļ���
for /f "delims=:" %%a in ('dir /b %XSA_DIR%\*.bit ^| findstr /n .*') do SET count=%%a
	if %count% equ 1 (
		for /f "delims=" %%i in ('dir /s /b %XSA_DIR%\*.bit') do SET BIF_FILE=%%~nxi
	) else (
		color cf & echo ��⵽���bit�ļ���ϵͳֻ����һ��bit�ļ���
		pause
		exit 1
	)

REM ��xsaͬ����ƽ̨
if not exist %cd%\shrd100_plt\hw\%XSA_FILE% (
	cp %XSA_DIR%\%XSA_FILE% %cd%\shrd100_plt\hw\
)else (
	echo shrd100_plt\hw\%XSA_FILE%�Ѵ���
)

@echo ********�޸�platform.spr********
python build/build_shrd100.py %SPR% %QEMUDATA% %SRC_WORKSPACE%/hw_spec/%XSA_FILE% "<platformDir>/hw"/%XSA_FILE%

@echo ********����shrd100_app_system.bif�ļ�********
echo the_ROM_image:  >build/release/shrd100_app_system.bif
echo {>>build/release/shrd100_app_system.bif
echo     [bootloader, destination_cpu=a53-0] %SRC_WORKSPACE%/shrd100_plt/export/shrd100_plt/sw/shrd100_plt/boot/fsbl.elf>>build/release/shrd100_app_system.bif
echo     [destination_device=pl] %SRC_WORKSPACE%/hw_spec/%BIF_FILE%>>build/release/shrd100_app_system.bif
echo     [destination_cpu=a53-0] %SRC_WORKSPACE%/shrd100_app/Debug/shrd100_app.elf>>build/release/shrd100_app_system.bif
echo }>>build/release/shrd100_app_system.bif

@echo ********���ڱ�����,���Եȼ�����********
call xsct build/shrd100.tcl %VITIS_WORKSPACE% %SRC_WORKSPACE%/hw_spec/%XSA_FILE%

if not exist shrd100_app/Debug/shrd100_app.elf (
    color cf & echo ********������������鿴����********
    pause
    exit 1
)else (
 	call build/build_shrd100.bat %VERSION%
)

if not exist build/release/*.bin (
   echo ********BIN�ļ�����ʧ��********
   exit 1
)else (
	@echo ******** BIN�ļ����ɳɹ� *******
	@echo ******** ��ʼ����OTA�ļ� *******
	@echo ******** %HEADER_FILE%  build/release/%VERSION:~1,-1%.bin  %VERSION:~1,-1% *******
	python build/build_shrd100_OTA.py %HEADER_FILE% build/release/%VERSION:~1,-1%.bin %VERSION:~1,-1%
	if not exist build/release/%VERSION%_OTA.bin (
		echo ********OTA�ļ�����ʧ��********
		pause
		exit 1

	)else (
		@echo ******** OTA�ļ����ɳɹ� *******
		@echo ********��������********
REM		cp tool/flash_prog/fsbl_autel.elf build/release/fsbl_autel.elf
		cp %SRC_WORKSPACE%/hw_spec/%BIF_FILE% build/release/%BIF_FILE%
		cp shrd100_app/Debug/shrd100_app.elf build/release/shrd100_app.elf
		cp shrd100_plt/export/shrd100_plt/sw/shrd100_plt/boot/fsbl.elf  build/release/fsbl.elf
	)
)
pause
@echo *******��ԭplatform.spr�ļ�*******
if exist shrd100_plt\platform.spr_bak (
   del /q shrd100_plt\platform.spr
   ren shrd100_plt\platform.spr_bak platform.spr
)
pause
cd build
