
projname="dolk"

mkdir -p build
cc -Wall -Wextra -g -o build/$projname\
 src/glad.c src/opengl_dolk.c src/linux_arena.c src/linux_file_io.c src/dolk.c\
 -lglfw3 -lc -lm -lX11 -lXrandr -lXi -lXcursor -lXinerama -lXxf86vm -lXext -lGL -lpthread -ldl  -Llib -Iinclude -Isrc -DDOLK_SLOW
