@echo off

call vcvars64

if not exist build mkdir build
if not exist build\obj mkdir build\obj

cl /Zi /nologo /W4 /EHsc /MT ^
  /I include /I src ^
  /D DOLK_DO_ASSERT=1 /D DOLK_DO_LOG=1 ^
  /D AL_LIBTYPE_STATIC=1 ^
  /D AL_BUILD_LIBRARY=0 ^
  /D AL_ALEXT_PROTOTYPES ^
  src\glad.c ^
  src\opengl_46_dolk.c ^
  src\opengl_46_buffer.c ^
  src\opengl_46_vertex_array.c ^
  src\opengl_46_shader.c ^
  src\opengl_46_renderer.c ^
  src\opengl_46_window.c ^
  src\opengl_46_input.c ^
  src\win32_arena.c ^
  src\win32_file_io.c ^
  src\dolk.c ^
  src\audio.c ^
  src\wav.c ^
  src\sv.c ^
  src\obj.c ^
  src\da.c ^
  src\sb.c ^
  src\render_state.c ^
  src\material.c ^
  /Fe:build\dolk.exe ^
  /Fo:build\obj\ ^
  /link ^
  /LIBPATH:lib ^
  /LTCG:OFF ^
  glfw3.lib opengl32.lib user32.lib gdi32.lib shell32.lib kernel32.lib OpenAL32.lib winmm.lib dsound.lib ole32.lib avrt.lib ^
  
