@echo on
make clean
make re

@echo off
setlocal

:: Define the base directory and archive file name
set BASE_DIR=luma\plugins
set ARCHIVE_FILE=luma.zip

:: Define the subfolders
set FOLDERS=0004000000055D00 0004000000055E00 000400000011C400 000400000011C500

:: Create the base directory
if not exist "%BASE_DIR%" (
    mkdir "%BASE_DIR%"
)

:: Create each subfolder
for %%F in (%FOLDERS%) do (
    mkdir "%BASE_DIR%\%%F"
)

:: Copy .3gx files into each subfolder
for %%F in (%FOLDERS%) do (
    copy *.3gx "%BASE_DIR%\%%F\"
)

:: Zip the luma directory
echo Creating archive %ARCHIVE_FILE%...
7z a "%ARCHIVE_FILE%" "%BASE_DIR%"

:: Check if the zip was created successfully
if exist "%ARCHIVE_FILE%" (
    echo Deleting the luma folder...
    rd /s /q "luma"
) else (
    echo Failed to create the zip file.
)

echo Done.
pause
endlocal