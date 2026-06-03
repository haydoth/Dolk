@echo off

call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

if not exist build mkdir build

cl /nologo /W4 /EHsc /MT ^
  /I include /I src ^
  /DDOLK_SLOW ^
  src\glad.c src\opengl_dolk.c src\win32_arena.c src\win32_file_io.c src\dolk.c ^
  /Fe:build\dolk.exe ^
  /link ^
  /LIBPATH:lib ^
  glfw3.lib opengl32.lib user32.lib gdi32.lib shell32.lib kernel32.lib
