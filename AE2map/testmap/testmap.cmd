@echo off

:: set method to 1 for Midp2Exe
:: set method to 2 for Java + mpowerplayer (recommended if Java is installed)
set method=1

:: set filename for jar file.
:: Warning: do not specify the extension neither the path.
:: Also, do not use space on the filename!
set ae2temp=temp

set AE2title=Ancient Empires II - Map tester batch
title %AE2title% [%1]

echo.
echo.
echo batch file from byblo, for AE2 map editor. 200x
echo http://go.to/byblo
echo http://byblo.proboards.com
echo.
echo.

::go to batch's drive and folder
%~d0
cd %~dp0

::check if required files are correctly present
if not exist bin\AE2lang.exe goto error_AE2lang_exe
if not exist bin\AE2pak.exe goto error_AE2pak_exe
if not exist bin\unzip.exe goto error_unzip_exe
if not exist bin\zip.exe goto error_zip_exe

::check if Midp2Exe's files are present if method is set (method=1)
if %method%==1 if not exist bin\Midp2Exe.exe goto error_Midp2Exe_exe
if %method%==1 if not exist bin\MidpRuntimeDLL.dll goto error_MidpRuntimeDLL_dll

::check if mpowerplayer jar files are present if method is set (method=2)
if %method%==2 if not exist bin\adapter.jar goto error_adapter_jar
if %method%==2 if not exist bin\cldc.jar goto error_cldc_jar
if %method%==2 if not exist bin\midp.jar goto error_midp_jar
if %method%==2 if not exist bin\player.jar goto error_player_jar

::make a temp\ex\ folder if do not exist yet
if not exist temp\ex\ echo Creating temp\ex\ folder.
if not exist temp\ex\ mkdir temp\ex\
::check if correctly created
if not exist temp\ex\ goto error_mkdir_temp_ex

::check if a temporary version of the game is present
if not exist temp\%ae2temp%.jar goto error_temp_ae2temp_jar

::extract the game archive content to the temp\ folder (only the files which do not exist yet)
bin\unzip.exe -n temp\%ae2temp%.jar -d temp\

::unpack 1.pak if not already done
if exist temp\1.pak if not exist temp\ex\_filelist.txt bin\ae2pak.exe temp\1.pak -e temp\ex\

::if not paratemer specified, simply run the emulator as its actual state.
if {%1}=={} goto step2

::if parameter %1 specified but file cannot be found, give error message
if not exist %~dp0%1 goto error_p1_not_found

::copy the map to the temp\ex\ folder
if exist %~dp0%1 move /Y %~dp0%1 %~dp0temp\ex\

::check if the map have been correctly moved
if exist %~dp0%1 goto error_p1_could_not_been_moved_to_ex



::***************************************************************************************************
::   step2 = jump here if parameter %1 was not specified
::***************************************************************************************************
:step2

::repack ex\ to 1.pak if _filelist.txt present
if exist temp\ex\_filelist.txt bin\AE2pak.exe temp\1.pak -p "temp\ex\_filelist.txt"
if exist temp\ex\_filelist.txt echo ------------------------------------------------

::convert lang.txt if present to lang.dat
if exist bin\lang.txt bin\AE2lang.exe bin\lang.txt temp\lang.dat
if exist bin\lang.txt echo ------------------------------------------------

::rezip everything which need to be updated to the jar file
bin\zip.exe -u -j temp\%ae2temp%.jar temp\*.class temp\gameicon.png temp\lang.dat temp\1.pak
echo ------------------------------------------------


::goto the choosen method on beginning this batch, see: set method=X
goto method_%method%


::***************************************************************************************************
::  method 1: Midp2Exe
::***************************************************************************************************
:method_1
::test by removing actual executable game to ensure it isn't currently running
del temp\%ae2temp%.exe
::if cannot be removed, maybe it is still running, then give error message
if exist temp\%ae2temp%.exe goto ae2temp_exe_cannot_del

::compile .jar to .exe (this will permit to keep the game's setting into the ae2temp-properties\ folder created by executable)
bin\midp2exe.exe -jar temp\%ae2temp%.jar -o temp\%ae2temp%.exe
echo ------------------------------------------------

::copy to the temp folder the needed dll for the exe
if not exist temp\MidpRuntimeDLL.dll xcopy bin\MidpRuntimeDLL.dll temp\

::check if correctly created then launch exe
if not exist temp\%ae2temp%.exe goto ae2temp_exe_cannot_create
temp\%ae2temp%.exe
goto end_no_pause



::***************************************************************************************************
::  method 2: Java + mpowerplayer
::***************************************************************************************************
:method_2

::run game
java.exe -jar bin\player.jar temp\%ae2temp%.jar

::in case, check if Java was present then give a hint if not
if exist "%SystemRoot%\system32\java.exe" goto end_no_pause
echo.
echo Warning: maybe Java isn't installed on your computer.
echo          If you have problem to run the emulator,
echo          then try to download the Java runtime from the official URL:
echo          http://java.com
goto end


::***************************************************************************************************
::   error messages list
::***************************************************************************************************
:error_AE2lang_exe
echo.
echo Error: file bin\AE2lang.exe could not be found!
goto end

:error_AE2pak_exe
echo.
echo Error: file bin\AE2pak.exe could not be found!
goto end

:error_Midp2Exe_exe
echo.
echo Error: file bin\Midp2Exe.exe could not be found!
goto end

:error_MidpRuntimeDLL_dll
echo.
echo Error: file bin\MidpRuntimeDLL.dll could not be found!
goto end

:error_unzip_exe
echo.
echo Error: file bin\unzip.exe could not be found!
goto end

:error_zip_exe
echo.
echo Error: file bin\zip.exe could not be found!
goto end

:error_adapter_jar
echo.
echo Error: file bin\adapter.jar could not be found!
goto end

:error_cldc_jar
echo.
echo Error: file bin\cldc.jar could not be found!
goto end

:error_midp_jar
echo.
echo Error: file bin\midp.jar could not be found!
goto end

:error_player_jar
echo.
echo Error: file bin\player.jar could not be found!
goto end

:error_mkdir_temp_ex
echo.
echo Error: Could not create the temporary: temp\ex\ folder!
goto end

:error_temp_ae2temp_jar
echo.
echo Error: file temp\%ae2temp%.jar could not be found!
echo.
echo You will need to test your map a JAR game file,
echo to rename and to place it to the temp\%ae2temp%.jar
goto end

:error_p1_not_found
echo Error: could not find specified file:
echo %~dp0%1
goto end

:error_p1_could_not_been_moved_to_ex
echo Error: could not move correctly the specified file:
echo %~dp0%1
goto end

:ae2temp_exe_cannot_del
echo.
echo Error: could not remove the executable file:
echo "%AE2title%.exe"
echo Please check if a preview session is already running !
goto end

:ae2temp_exe_cannot_create
echo.
echo Error: Executable could not be created for some reason.
echo Please check upper this window the log from Midp2Exe.exe
goto end



::***************************************************************************************************
::   endings list of this batch
::***************************************************************************************************
:end
echo.
pause
exit

:end_no_pause
exit
