@echo off

if not exist bin mkdir bin

g++ handle\program.cpp -o bin\chat
bin\chat.exe