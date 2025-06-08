##
##  Created by Matt Hartley on 05/06/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

set -e
mkdir -p ./build

# ../core/core/sysaudio_win32_xaudio.cpp
files="main.c libs.c ../core/core/sysvideo_win32.c ../core/core/sysaudio_win32_xaudio.cpp"
options="-luser32 -lgdi32 -lopengl32 -lglu32 -lole32 -lxaudio2_9"

gcc -o ./build/majorjam $files \
	-I../core \
	-static -static-libgcc -static-libstdc++\
	-mwindows\
	-O2 -s\
	$options \

# -Wl,-rpath,@executable_path/Frameworks \
