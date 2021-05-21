@echo off
bossac -i -d --port=COM10 -U true -i -e -w -v ./firmware.bin -R
pause