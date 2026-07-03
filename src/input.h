#pragma once
#include "types.h"

void Input_EndFrame(InputState* state);

int KeyIsPressed(InputState* state, int key);
int KeyWasJustReleased(InputState* state, int key);
int KeyWasJustPressed(InputState* state, int key);

void ToggleMouseSeizure(void* windowHandle);
int MouseIsSeized(void* windowHandle);

typedef struct {float X, Y;} MouseDelta;
MouseDelta GetMouseDelta(void);

typedef struct {float X, Y;} MousePosition;
MousePosition GetMousePosition(void);

