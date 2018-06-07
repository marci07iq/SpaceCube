#pragma once

#include "../Shaders/Shader.h"

extern guid_t userGUID;
extern NetBinder* user;
extern NetworkC* Connection;

extern int frameTime; //33ms;

void glut_timer_CB(int a);


extern Shader chunkShader;
extern Shader entityShader;

extern GLuint textures;
extern GLuint texSampler;

namespace MainGameCanvas {
  extern int mxold;
  extern int myold;

  extern OpenGLData view;

  extern bool lockMouse;

  void normalizeAngles();
  int renderManager(int ax, int ay, int bx, int by, set<key_location>& down);
  int resizeManager(int x, int y);
  int mouseEntryManager(int state);
  int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down);
  int guiEventManager(gui_event evt, int mx, int my, set<key_location>& down, Canvas* me);
}

void bindGameScreenLabels();