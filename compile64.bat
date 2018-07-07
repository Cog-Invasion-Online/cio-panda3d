@echo off

set pythondir=win-python-x64
set MAKEPANDA_THIRDPARTY=thirdparty-x64

thirdparty-x64\win-python-x64\python makepanda/makepanda.py --everything --no-contrib --no-fmodex --no-physx --no-bullet --no-pview --no-pandatool --no-swscale --no-swresample --no-speedtree --no-vrpn --no-artoolkit --no-opencv --no-directcam --no-vision --no-rocket --no-awesomium --no-deploytools --no-skel --no-ffmpeg --static %*

