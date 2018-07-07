@echo off
set PATH=%systemroot%\System32;%PATH%
thirdparty\win-python-x64\python makepanda/makepanda.py --everything --no-touchinput --verbose --distributor="Cog Invasion Online" --threads=6 --optimize=3 --arch=x64 --windows-sdk=10 --msvc-version=14.1 --no-maya2015 --no-maya2017
pause