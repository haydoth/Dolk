#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define global static
#define internal static
#define local_persist static

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define i8  int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

#define f32 float
#define f64 double

#define Kilobytes(x) ((x) * 1024)
#define Megabytes(x) (Kilobytes(x) * 1024)
#define Gigabytes(x) (Megabytes(x) * 1024)

#ifdef DOLK_SLOW 
#define ASSERT(expr) do {if(!(expr)) {abort();}} while(0)
#else
#define ASSERT(expr) 
#endif

#ifdef DOLK_DO_LOG

#define DOLK_LOG(...)     printf(__VA_ARGS__)
#define DOLK_INFO(...)    DOLK_LOG("[INFO] "__VA_ARGS__)
#define DOLK_WARNING(...) DOLK_LOG("[WARNING] "__VA_ARGS__)
#define DOLK_ERROR(...)   DOLK_LOG("[ERROR] "__VA_ARGS__)

#else

#define DOLK_LOG(...)     
#define DOLK_INFO(...)    
#define DOLK_WARNING(...) 
#define DOLK_ERROR(...)   

#endif

#define UNUSED(x) (void)(x)

