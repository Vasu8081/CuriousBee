@echo off
setlocal enabledelayedexpansion

REM === SETUP ===
set "SCRIPT_DIR=%~dp0"
cd /d "%SCRIPT_DIR%"

REM === Load CF_API_TOKEN from .env ===
set "CF_API_TOKEN="
for /f "usebackq tokens=1,* delims==" %%A in (".env") do (
    if "%%A"=="CF_API_TOKEN" set "CF_API_TOKEN=%%B"
)

if "%CF_API_TOKEN%"=="" (
    echo [ERROR] CF_API_TOKEN not found in .env
    exit /b 1
)

REM === Fetch domain and hostname ===
for /f "delims=" %%A in ('jq -r ".domain" config.json') do set "DOMAIN=%%A"
for /f "delims=" %%A in ('hostname') do set "HOSTNAME=%%A"

echo [INFO] Hostname: %HOSTNAME%
echo [INFO] Domain: %DOMAIN%

REM === Get host JSON ===
set "TMP_HOST=%TEMP%\host_json.txt"
del "%TMP_HOST%" >nul 2>&1
for /f "delims=" %%A in ('jq -c --arg name "%HOSTNAME%" ".hosts[] | select(.name == $name)" config.json') do (
    echo %%A>"%TMP_HOST%"
)

if not exist "%TMP_HOST%" (
    echo [ERROR] Host not found in config.json
    exit /b 1
)

REM === Get alias list ===
set i=0
for /f "delims=" %%A in ('jq -r ".aliases[]" "%TMP_HOST%"') do (
    set "ALIAS[!i!]=%%A"
    echo [DEBUG] Found alias: %%A
    set /a i+=1
)

REM === Get current IPv6 ===
for /f "delims=" %%A in ('curl -6 -s --max-time 10 https://ifconfig.co') do set "CURRENT_IP=%%A"
if "%CURRENT_IP%"=="" (
    echo [ERROR] Could not fetch IPv6
    exit /b 1
)
echo [INFO] Current IPv6: %CURRENT_IP%

REM === Get zone ID ===
for /f "delims=" %%A in (
    'curl -s -H "Authorization: Bearer %CF_API_TOKEN%" -H "Content-Type: application/json" ^
     "https://api.cloudflare.com/client/v4/zones?name=%DOMAIN%" ^| jq -r ".result[0].id"'
) do set "ZONE_ID=%%A"

if "%ZONE_ID%"=="" (
    echo [ERROR] Could not retrieve zone ID
    exit /b 1
)
echo [INFO] Zone ID: %ZONE_ID%

REM === Update DNS Records ===
set j=0
:loop
if defined ALIAS[%j%] (
    set "ALIAS_NAME=!ALIAS[%j%]!"
    set "FULL_NAME=!ALIAS_NAME!.%DOMAIN%"

    echo [INFO] Processing: !FULL_NAME!

    REM Get record ID
    del tmp_dns.json >nul 2>&1
    curl -s -H "Authorization: Bearer %CF_API_TOKEN%" ^
         -H "Content-Type: application/json" ^
         "https://api.cloudflare.com/client/v4/zones/%ZONE_ID%/dns_records?type=AAAA&name=!FULL_NAME!" > tmp_dns.json

    set "RECORD_ID="
    for /f "delims=" %%R in ('jq -r ".result[0].id" tmp_dns.json') do set "RECORD_ID=%%R"

    if "!RECORD_ID!"=="" (
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