@echo off
echo [*] Compiling dC2 Poller...

:: The exact command that worked for you goes here
g++ .\src\main.cpp .\src\rpcengine.cpp -static -static-libgcc -static-libstdc++ -o poller.exe

:: Check if the compilation failed
if %errorlevel% neq 0 (
    echo [-] Build failed. Check your syntax.
    exit /b %errorlevel%
)

echo [+] Build successful! Executing poller.exe...
echo ------------------------------------------------
.\poller.exe
echo ------------------------------------------------