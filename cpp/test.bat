@echo off
echo [*] Compiling dC2 Poller...

:: Added HTTPClient.cpp and -lwinhttp
g++ src/main.cpp src/rpcengine.cpp src/HTTPClient.cpp -I include -static -static-libgcc -static-libstdc++ -lwinhttp -o poller.exe

if %errorlevel% neq 0 (
    echo [-] Build failed. Check your syntax.
    exit /b %errorlevel%
)

echo [+] Build successful! Executing poller.exe...
echo ------------------------------------------------
.\poller.exe
echo ------------------------------------------------