echo OFF
set LOCALPORT=30000-30001
set RULE_NAME="TimeMasters"
set PROTOCOL=udp
set DIR=in
set ACTION=allow


netsh advfirewall firewall show rule name=%RULE_NAME%

if ERRORLEVEL 1 (
netsh advfirewall firewall add rule name=%RULE_NAME% dir=%dir% protocol=%PROTOCOL% localport=%LOCALPORT% action=%ACTION%
) else (
    echo Rule is already present
)