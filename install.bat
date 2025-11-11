@echo off
chcp 65001 >nul
echo Starting installation...

g++ --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: g++ not found. Install MinGW!
    pause
    exit /b 1
)

set "APP_DIR=Encryption_Algorithm_RGR"
mkdir "%APP_DIR%" 2>nul
mkdir "%APP_DIR%\dlls" 2>nul

echo Compiling libraries...
g++ -shared -o "%APP_DIR%\dlls\atbash.dll" ciphers\atbash.cpp
g++ -shared -o "%APP_DIR%\dlls\hill.dll" ciphers\hill.cpp
g++ -shared -o "%APP_DIR%\dlls\permutations.dll" ciphers\permutations.cpp

echo Compiling main program...
g++ -o "%APP_DIR%\Encryption_Algorithm_RGR.exe" src\main.cpp src\cipher_manager.cpp src\encrypt_decrypt_keygen.cpp utils\file_utils.cpp "%APP_DIR%\dlls\atbash.dll" "%APP_DIR%\dlls\hill.dll" "%APP_DIR%\dlls\permutations.dll"

if errorlevel 1 (
    echo COMPILATION FAILED!
    pause
    exit /b 1
)

echo Encryption_Algorithm_RGR > "%APP_DIR%\README.txt"
echo. >> "%APP_DIR%\README.txt"
echo Algorithms: >> "%APP_DIR%\README.txt"
echo 1. Atbash - no key >> "%APP_DIR%\README.txt"
echo 2. Hill - 4 numbers key >> "%APP_DIR%\README.txt"
echo 3. Permutation - block cipher >> "%APP_DIR%\README.txt"

echo.
echo Installation complete!
dir "%APP_DIR%\"
pause