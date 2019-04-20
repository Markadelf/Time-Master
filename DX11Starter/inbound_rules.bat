ECHO OFF
set LOCALPORT=30000-30001
set RULE_NAME="TimeMasters"
set PROTOCOL="UDP"
set DIR="IN"
set ACTION="ALLOW"


echo netsh advfirewall add rule name =%RULE_NAME% dir=%DIR% localport=%LOCALPORT% action=%ACTION% protocol=%PROTOCOL%