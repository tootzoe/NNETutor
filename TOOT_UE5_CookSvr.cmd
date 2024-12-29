@echo off
setlocal

::  Change this UE ROOT folder according to your environment
SET UE_FOLDER=C:\UnrealEngine


echo "++++++++=============================++++++++++++++"
echo  Current Dir:  %~dp0
echo  UE_FOLDER Dir:  %UE_FOLDER%
echo "++++++++=============================++++++++++++++"




FOR /f %%F in ('dir /b *.uproject') do SET PRJNAMEPATH=%%~dpnxF


echo %PRJNAMEPATH%



echo "++++==== UE Cook Server for:  %PRJNAMEPATH%   ====++++"


call %UE_FOLDER%/Engine/Binaries/Win64/UnrealEditor-Cmd.exe "%PRJNAMEPATH%" -run=cook -targetplatform=Windows -cookonthefly


if %ERRORLEVEL% NEQ 0   pause

