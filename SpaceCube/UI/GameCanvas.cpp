#include "GameCanvas.h"

NetworkC* Connection;
guid_t userGUID = 0x0123456789abcdef;

int frameTime = 33; //33ms;

void glut_timer_CB(int a) {
  glutTimerFunc(frameTime, glut_timer_CB, 0);
  glutPostRedisplay();
}

Player* user;

Shader chunkShader;

GLuint textures;
GLuint texSampler;

bool MainGameCanvas::lockMouse = 0;

OpenGLData MainGameCanvas::view;

uint64_t firstFrameTime;

uint64_t lastFrameTime;

void MainGameCanvas::normalizeAngles() {

}
int MainGameCanvas::renderManager(int ax, int ay, int bx, int by, set<key_location>& down) {
  uint64_t frameTime = chrono::duration_cast< chrono::milliseconds >(
    chrono::system_clock::now().time_since_epoch()).count();

  sVec3 userMove = {user->_lookDir.x, user->_lookDir.y, 0};
  if(userMove.sqrlen() > 0) {
    userMove.norm();
    if (isDown(down, key_location(key('w', 0)))) {
      user->_pos += userMove * user->_speed * (frameTime - lastFrameTime) / 1000.0;
    }
    if (isDown(down, key_location(key('d', 0)))) {
      user->_pos += crs(userMove, {0,0,1}) * user->_speed * (frameTime - lastFrameTime) / 1000.0;
    }
    if (isDown(down, key_location(key('s', 0)))) {
      user->_pos -= userMove * user->_speed * (frameTime - lastFrameTime) / 1000.0;
    }
    if (isDown(down, key_location(key('a', 0)))) {
      user->_pos -= crs(userMove, { 0,0,1 }) * user->_speed * (frameTime - lastFrameTime) / 1000.0;
    }
    if (isDown(down, key_location(key(' ', 0)))) {
      user->_pos += fVec3({ 0,0,1 }) * user->_speed * (frameTime - lastFrameTime) / 1000.0;
    }
    if (isDown(down, key_location(key(112, 1)))) {
      user->_pos -= fVec3({ 0,0,1 }) * user->_speed * (frameTime - lastFrameTime) / 1000.0;
    }
    cout << "Pos: " << user->_pos << '\t' << user->_lookDir << '\r';
  }

  glDepthRange(0.1, 100);
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  chunkShader.bind();

  float transform[16];
  
  Transpose view;
  view.createLook(user->_pos + user->_headOffset, user->_lookDir);
  view.project(PI/2, (bx-ax)*1.0f/(by-ay), 100, 0.1);
  view.read(transform);

  GLint loc = glGetUniformLocation(chunkShader._pID, "transform");
  if (loc != -1) {
    glUniformMatrix4fv(loc, 1, false, transform);
  }
  loc = -1;
  loc = glGetUniformLocation(chunkShader._pID, "frame_time");
  if (loc != -1) {
    glUniform1f(loc, (lastFrameTime - firstFrameTime) / 100000.0f );
  }

  //glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures);
  glUniform1i(glGetUniformLocation(chunkShader._pID, "myTexture"), 0); // set it manually


  for (auto&& it : _fragments) {
    if(it.second) {
      for (int i = 0; i < COLUMN_PER_FRAGMENT; i++) {
        for (int j = 0; j < COLUMN_PER_FRAGMENT; j++) {
          ChunkCol* cc = it.second->getChunkCol(i,j);
          if(cc != NULL) {
            for (int k = 0; k < CHUNK_PER_COLUMN; k++) {
              Chunk* c = cc->getChunk(k);

              if(c != NULL) {
                if (c->_state == Chunk_ToRender) {
                  c->buildChunk();
                }
                if(c->_state == Chunk_Rendered && c->_quads) {
                  glBindVertexArray(c->_chunk_vao);
                  glDrawArrays(GL_TRIANGLES, 0, c->_quads);
                }
              }
            }
          }
        }
      }
    }
  }

  glFlush();

  Graphics::resetViewport();

  lastFrameTime = frameTime;

  return 0;
}
int MainGameCanvas::resizeManager(int x, int y) {
  return 0;
}
int MainGameCanvas::mouseEntryManager(int state) {
  //mousebuttons = 0;
  return 0;
}
int MainGameCanvas::mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down) {
  if (x != 100 || y != 100) {
    int dx = x - 100;
    int dy = y - 100;
    glutWarpPointer(100, glutGet(GLUT_WINDOW_HEIGHT) - 100);

    polar_vec3 pos;
    pos.fromCartesian(user->_lookDir);
    user->_lookDir = pos.toCartesian();

    pos.phi -= 0.01*dx;
    pos.theta -= 0.01*dy;
    pos.r = 1;
 
    pos.reNormalize();

    user->_lookDir = pos.toCartesian();
  }
  return 0;
}

int MainGameCanvas::guiEventManager(gui_event evt, int mx, int my, set<key_location>& down, Canvas* me) {
  bool in = me->isIn(mx, my);
  
  return 0;
}

void bindGameScreenLabels() {
  firstFrameTime = lastFrameTime = chrono::duration_cast< chrono::milliseconds >(
    chrono::system_clock::now().time_since_epoch()).count();

  for (int cx = 0; cx < 16; cx++) {
    for (int cy = 0; cy < 16; cy++) {

      DataElement* res = new DataElement();
      DataElement* nd;
      nd = new DataElement();
      vGFunc(cx, nd);
      res->addChild(nd);
      nd = new DataElement();
      vGFunc(cy, nd);
      res->addChild(nd);
      
      Connection->SendData(res, PacketChunk);
    }
  }
}