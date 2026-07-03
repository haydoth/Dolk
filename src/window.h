#pragma once
#include "types.h"

typedef struct { int X, Y; } WindowPosition;
typedef struct { int Width, Height; } WindowSize;

void SetClearColor(float r, float g, float b);
void SetClearColorV(float* v);

void SetWindowTitle(WindowState* window, const char* title);

void SetWindowPosition(WindowState* window, int x, int y);
WindowPosition GetWindowPosition(WindowState* window);

void SetWindowSize(WindowState* window, int width, int height);
WindowSize GetWindowSize(WindowState* window);

int CreateWindow(WindowState* state, const char* windowTitle, int windowWidth, int windowHeight);

double GetElapsedTime(void);
