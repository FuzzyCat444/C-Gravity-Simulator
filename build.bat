del CGravitySim.exe
"mingw64/bin/gcc" -o CGravitySim.exe -O3 *.c glad/src/gl.c lodepng/src/lodepng.c -I"glad/include" -I"lodepng/include" -I"linmath/include" -I"glfw-3.3.8/include" -L"glfw-3.3.8/lib-mingw-w64" -lglfw3 -lglfw3dll
start CGravitySim.exe