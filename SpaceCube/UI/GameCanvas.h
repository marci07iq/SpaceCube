#pragma once

#include "../UI/Transpose.h";

extern guid_t userGUID;
extern NetPlayer* user;
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
  void renderManager(Canvas* c, int ax, int ay, int bx, int by, set<key_location>& down);
  int resizeManager(Canvas* c, int x, int y);
  int mouseEntryManager(Canvas* c, int state);
  int mouseMoveManager(Canvas* c, int x, int y, int ox, int oy, set<key_location>& down);
  int guiEventManager(Canvas* c, gui_event& evt, int mx, int my, set<key_location>& down);
}

void bindGameScreenLabels();
