@echo off

set "projname=dolk"

if not exist build mkdir build
zig cc -target x86_64-windows-msvc  -Wall -Wextra -o build/%projname%.exe ^
 src/glad.c src/main.c src/win32_arena.c src/win32_file_io.c ^
 -lc -lopengl32 -luser32 -lgdi32 -lshell32 -Llib -lglfw3 -Iinclude -Isrc -DDOLK_SLOW 
