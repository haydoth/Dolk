#include "file_io.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

bool
WriteEntireFile(char* path, void* buffer, u64 bufferSize)
{
  printf("Writing file: %s\n", path);
  int fd = creat(path, S_IRWXU);
  if(fd == -1) return false;

  if(write(fd, buffer, bufferSize) < 0) return false;

  close(fd);
  return true;
}

file_buffer
ReadEntireFile(char* path, arena* a) 
{
  printf("Reading file: %s\n", path);
  int fd = open(path, O_RDONLY);
  if(fd == -1) return (file_buffer) {0};
  
  struct stat st;
  fstat(fd, &st);
  u8* buffer = arena_push(a, st.st_size);
  
  if(read(fd, buffer, st.st_size) == -1) return (file_buffer) {0};
  
  close(fd);
  return (file_buffer) {buffer, st.st_size};
}
