@echo off

set PS3SDK=/c/usr/PSDK3v2
set PSL1GHT=/c/usr/PSDK3v2/psl1ght
set PS3DEV=%PS3SDK%/ps3dev
set WIN_PS3SDK=C:\usr\PSDK3v2
set usr=C:\usr
set PYTHON_PATH=%PS3SDK%/MinGW/Python27
set PATH=%WIN_PS3SDK%/mingw/msys/1.0/bin;%PYTHON_PATH%;%WIN_PS3SDK%/mingw/bin;%PS3DEV%/ppu/bin;%usr%\local\cell\host-win32\spu\bin;%usr%\local\cell\host-win32\ppu\bin;%usr%\local\cell\host-win32\sn\bin;%usr%\local\cell\host-win32\bin;%usr%\local\cell\host-win32\Cg\bin
cls

title PS3 SDK *DANiO*

bash