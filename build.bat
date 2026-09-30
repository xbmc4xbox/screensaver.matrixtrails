@echo off
setlocal
pushd "%~dp0"
set "CORE=%~1"
if not defined CORE goto usage

SET ADDON_NAME=MatrixTrails
SET ADDON_NAME_ID=screensaver.matrixtrails
SET VS_SOLUTION=%ADDON_NAME%.sln
SET DLL_ADDON=%ADDON_NAME%.xbs

ECHO Cleaning project...
"%VS71COMNTOOLS%\..\IDE\devenv.com" .\src\%VS_SOLUTION% /clean Release

ECHO Installing prerequests...
xcopy /E /I /Y "%CORE%\xbmc\addons\kodi-dev-kit\include\kodi" "src\include\kodi\" >nul
if errorlevel 1 goto failed
xcopy /E /I /Y "%CORE%\lib\boost\boost" "src\boost\" >nul
if errorlevel 1 goto failed
copy /y "%CORE%\xbmc\platform\xbox\stdint.h" src\include\ >nul
if errorlevel 1 goto failed

ECHO Compiling addon...
"%VS71COMNTOOLS%\..\IDE\devenv.com" src\%VS_SOLUTION% /rebuild Release
if errorlevel 1 goto failed
if not exist src\Release\%DLL_ADDON% goto failed

ECHO Building addon...
copy /y src\Release\%DLL_ADDON% %ADDON_NAME_ID%\ >nul
if errorlevel 1 goto failed

REM TODO: parse from addon.xml
SET "VERSION=2.0.0"

ECHO Compressing addon...
7z a -tzip "%ADDON_NAME_ID%-%VERSION%.zip" "%ADDON_NAME_ID%\*"

ECHO Finished!
popd
exit /b 0

:usage
echo Usage: build.bat "C:\path\to\xodi"

:failed
popd
exit /b 1
