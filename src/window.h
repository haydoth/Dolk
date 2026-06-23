#pragma once

typedef struct { int X, Y; } WindowPosition;
typedef struct { int Width, Height; } WindowSize;

void SetClearColor(float r, float g, float b);
void SetClearColorV(float* v);

void SetWindowTitle(const char* title);

void SetWindowPosition(int x, int y);
WindowPosition GetWindowPosition();

void SetWindowSize(int width, int height);
WindowSize GetWindowSize();

int CreateWindow(const char* windowTitle, int windowWidth, int windowHeight);
void* GetWindowHandle();

double GetElapsedTime();
