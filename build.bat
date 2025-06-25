@echo off
setlocal

set CC=gcc
set CFLAGS=-Wall -Wextra -Iinclude
set SRC=src\packer.c src\flags.c src\logger.c
set OUT=SNPACK.exe

%CC% %CFLAGS% -o %OUT% %SRC%
if %errorlevel% neq 0 (
    echo Build failed!
    exit /b 1
)
echo Build succeeded!