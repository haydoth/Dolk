#include "common.h"
#include "window.h"
#include "input.h"
#include "renderer.h"
#include "audio.h"

typedef struct {
  float playerSpeed;
} GameState;

typedef struct {

  WindowState window;
  AudioState audio;
  RenderState render;
  GameState game;
  
} AppState;

void Init(AppState* app);
void Update(AppState* app, f64 delta);
