#pragma once

#include <stdint.h>
#include <stdlib.h>

#define global static
#define internal static
#define local_persist static

#define true 1
#define false 0

#define bool int8_t

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

#ifdef CHISEL_SLOW 
#define ASSERT(expr) if(!(expr)) {abort();}
#else
#define ASSERT(expr) 
#endif

#define UNUSED(x) (void)(x)
