##
##  Created by Matt Hartley on 30/05/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

set -e
mkdir -p ./build

files="main.c libs.c core/sysvideo_osx.m core/sysaudio_osx.m"
options="-g -fno-objc-arc"

clang -o ./build/majorjam $files \
	-I../core \
	-fno-objc-arc \
	-framework Metal \
	-framework OpenGL \
	-framework Cocoa \
	-framework QuartzCore \
	-framework AudioToolbox \
	-framework CoreAudio \
	$options \

# -Wl,-rpath,@executable_path/Frameworks \
