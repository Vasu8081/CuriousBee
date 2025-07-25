@echo off
setlocal enabledelayedexpansion

echo [INFO] Starting Cloudflare AAAA sync...

REM Set working directory to script location
set "SCRIPT_DIR=%~dp0"
cd /d "%SCRIPT_DIR%"

REM Load CF_API_TOKEN from .env
for /f "usebackq tokens=1,* delims==" %%A in (".env") do (
    if "%%A"=="CF_API_TOKEN" set "CF_API_TOKEN=%%B"
)
if "%CF_API_TOKEN%"=="" (
    echo [ERROR] CF_API_TOKEN not found in .env
    exit /b 1
)

REM Get hostname and domain
for /f "delims=" %%A in ('hostname') do set "HOSTNAME=%%A"
for /f "delims=" %%A in ('jq -r ".domain" config.json') do set "DOMAIN=%%A"

echo [INFO] Hostname: %HOSTNAME%
echo [INFO] Domain: %DOMAIN%

REM Get host JSON line
set "HOST_JSON="
for /f "delims=" %%A in ('jq -c --arg name "%HOSTNAME%" ".hosts[] | select(.name == $name)" config.json') do set "HOST_JSON=%%A"
if "%HOST_JSON%"=="" (
    echo [ERROR] Host not found in config.json
    exit /b 1
)

REM Extract aliases
set i=0
for /f "delims=" %%A in ('echo %HOST_JSON% ^| jq -r ".aliases[]"') do (
    set "ALIAS[!i!]=%%A"
    echo [DEBUG] Found alias: %%A
    set /a i+=1
)

REM Get current IPv6
for /f "delims=" %%A in ('curl -6 -s --max-time 10 https://ifconfig.co') do set "CURRENT_IP=%%A"
echo [INFO] Current IPv6: %CURRENT_IP%

REM Get Cloudflare Zone ID
for /f "delims=" %%A in (
    'curl -s -H "Authorization: Bearer %CF_API_TOKEN%" ^
         -H "Content-Type: application/json" ^
         "https://api.cloudflare.com/client/v4/zones?name=%DOMAIN%" ^
         ^| jq -r ".result[0].id"'
) do set "ZONE_ID=%%A"
if "%ZONE_ID%"=="" (
    echo [ERROR] Failed to get Zone ID
    exit /b 1
)
echo [INFO] Zone ID: %ZONE_ID%

REM Loop through aliases
set j=0
:loop
if defined ALIAS[%j%] (
    set "ALIAS_NAME=!ALIAS[%j%]!"

    if "!ALIAS_NAME!"=="%DOMAIN%" (
        set "FULL_NAME=%DOMAIN%"
    ) else (
        set "FULL_NAME=!ALIAS_NAME!.%DOMAIN%"
    )

    echo [INFO] Processing: !FULL_NAME!

    REM Fetch Record ID
    for /f "delims=" %%R in (
        'curl -s -H "Authorization: Bearer %CF_API_TOKEN%" ^
            -H "Content-Type: application/json" ^
            "https://api.cloudflare.com/client/v4/zones/%ZONE_ID%/dns_records?type=AAAA&name=!FULL_NAME!" ^
            ^| jq -r ".result[0].id"'
    ) do set "RECORD_ID=%%R"

    if "!RECORD_ID!"=="" (
        set "RECORD_ID=null"
    )

    if /i "!RECORD_ID!"=="null" (
        echo [INFO] Creating AAAA record for !FULL_NAME!
        curl -s -X POST "https://api.cloudflare.com/client/v4/zones/%ZONE_ID%/dns_records" ^
            -H "Authorization: Bearer %CF_API_TOKEN%" ^
            -H "Content-Type: application/json" ^
            --data "{\"type\":\"AAAA\",\"name\":\"!FULL_NAME!\",\"content\":\"%CURRENT_IP%\",\"ttl\":1,\"proxied\":false}" >nul
    ) else (
        echo [INFO] Updating AAAA record for !FULL_NAME!
        curl -s -X PUT "https://api.cloudflare.com/client/v4/zones/%ZONE_ID%/dns_records/!RECORD_ID!" ^
            -H "Authorization: Bearer %CF_API_TOKEN%" ^
            -H "Content-Type: application/json" ^
            --data "{\"type\":\"AAAA\",\"name\":\"!FULL_NAME!\",\"content\":\"%CURRENT_IP%\",\"ttl\":1,\"proxied\":false}" >nul
    )

    set /a j+=1
    goto :loop
)

echo [DONE] All aliases updated successfully.
pause