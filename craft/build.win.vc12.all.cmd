@echo off

echo ^=^= building x64 ^=^=
start %~dp0build.win.vc12.x64.cmd

echo ^=^= building x64.debug ^=^=
start %~dp0build.win.vc12.x64.debug.cmd

echo ^=^= building x86 ^=^=
start %~dp0build.win.vc12.x86.cmd

echo ^=^= building x86.debug ^=^=
start %~dp0build.win.vc12.x86.debug.cmd