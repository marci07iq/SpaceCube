#pragma once

#include "../UI/Transpose.h";

extern guid_t userGUID;
extern NetPlayer* user;
extern NetworkC* Connection;

extern int frameTime; //33ms;

void glut_timer_CB(int a);


extern NGin::Graphics::Shader chunkShader;
extern NGin::Graphics::Shader entityShader;

extern GLuint textures;
extern GLuint texSampler;

namespace MainGameCanvas {
  extern int mxold;
  extern int myold;

  extern NGin::Graphics::OpenGLData view;

  extern bool lockMouse;

  void normalizeAngles();
  void renderManager(NGin::Graphics::CanvasHwnd c, int ax, int ay, int bx, int by, set<NGin::Graphics::key_location>& down);
  int resizeManager(NGin::Graphics::CanvasHwnd c, int x, int y);
  int mouseEntryManager(NGin::Graphics::CanvasHwnd c, int state);
  int mouseMoveManager(NGin::Graphics::CanvasHwnd c, int x, int y, int ox, int oy, set<NGin::Graphics::key_location>& down);
  int guiEventManager(NGin::Graphics::CanvasHwnd c, NGin::Graphics::gui_event& evt, int mx, int my, set<NGin::Graphics::key_location>& down);
}

void bindGameScreenLabels();
