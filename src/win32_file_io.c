#include "file_io.h"

// Win32
#include <windows.h>


#define DWORD_MAX_SIZE 4294967295 

bool
WriteEntireFile(char* path, void* buffer, u32 bufferSize)
{
  // NOTE: I am unsure what exactly the behavior of CREATE_NEW is compared to CREATE_ALWAYS
  // or if it is possible to do this in 1 call to CreateFileA. However this should suffice for now.
  
  HANDLE fileHandle = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
				  0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
  
  if(fileHandle == INVALID_HANDLE_VALUE) return false;

  fileHandle = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
				  0, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

  if(fileHandle == INVALID_HANDLE_VALUE) return false;

  DWORD bytesWritten;
  if(!WriteFile(fileHandle, buffer, bufferSize, &bytesWritten, 0)) return false;

  return true;
}


file_buffer
ReadEntireFile(char* path, arena* a) {
  // Get handle
  HANDLE fileHandle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
				  0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

  if(fileHandle == INVALID_HANDLE_VALUE) return (file_buffer) {0};

  // Get size of file
  LARGE_INTEGER fileSize;
  if (!GetFileSizeEx(fileHandle, &fileSize))
  {
      CloseHandle(fileHandle);
      return (file_buffer) {0}; 
  }

  u8* buffer = arena_push(a, fileSize.QuadPart);
  DWORD bytesRead = 0;

  // Read file
  if(fileSize.QuadPart >= DWORD_MAX_SIZE) return (file_buffer) {0};
  if(!ReadFile(fileHandle, buffer, (u32)fileSize.QuadPart, &bytesRead, 0)) return (file_buffer) {0};

  CloseHandle(fileHandle);
  return (file_buffer) {buffer, bytesRead};
}
