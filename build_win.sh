##
##  Created by Matt Hartley on 05/06/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

set -e
mkdir -p ./build

files="main.c ../core/core/sysvideo_win32.c"
options="-g -luser32 -lgdi32 -lopengl32 -lglu32"

gcc -o ./build/majorjam $files \
	-I../core \
	$options \

# -Wl,-rpath,@executable_path/Frameworks \
