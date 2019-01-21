@echo off
set PATH=%systemroot%\System32;%PATH%
move built_x64_static\libp3pystub.lib built_x64_static\lib\libp3pystub.lib
thirdparty\win-python-x64\python makepanda/makepanda.py --everything --no-touchinput --verbose --distributor="Cog Invasion Online" --threads=3 --optimize=3 --arch=x64 --windows-sdk=10 --msvc-version=14.1 --no-maya2015 --no-maya2017 --outputdir built_x64_static --static %*
move built_x64_static\lib\libp3pystub.lib built_x64_static\libp3pystub.lib
pause
