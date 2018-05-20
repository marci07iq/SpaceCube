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

  sVec3 userMove = { user->getLook().x, user->getLook().y, 0};
  mpsVec3 resVel = mpsVec3(0);
  if(userMove.sqrlen() > 0) {
    userMove.norm();
    if (isDown(down, key_location(key('w', 0)))) {
      resVel += (userMove * user->getSpeed());
    }
    if (isDown(down, key_location(key('d', 0)))) {
      resVel += (crs(userMove, {0,0,1}) * user->getSpeed());
    }
    if (isDown(down, key_location(key('s', 0)))) {
      resVel += (-userMove * user->getSpeed());
    }
    if (isDown(down, key_location(key('a', 0)))) {
      resVel += (-crs(userMove, { 0,0,1 }) * user->getSpeed());
    }
    if (isDown(down, key_location(key(' ', 0)))) {
      resVel += (fVec3(0,0,1) * user->getSpeed());
    }
    if (isDown(down, key_location(key(112, 1)))) {
      resVel += (-fVec3(0,0,1) * user->getSpeed());
    }
  }

  user->setVel(resVel);

  tickPhysics((frameTime - lastFrameTime) / 1000.0);

  glDepthRange(0.01, 256);
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  chunkShader.bind();
  
  float cameraM[16];

  Transpose camview;
  camview.createLook(user->getHead(), user->getLook());
  camview.project(PI/2, (bx-ax)*1.0f/(by-ay), 256, 0.01);
  camview.read(view.projection);
  camview.read(cameraM);


  view.cameraEye = user->getHead();
  view.viewport[0] = ax;
  view.viewport[1] = ay;
  view.viewport[2] = bx-ax;
  view.viewport[3] = by-ay;

  GLint loc = glGetUniformLocation(chunkShader._pID, "transform");
  if (loc != -1) {
    glUniformMatrix4fv(loc, 1, false, cameraM);
  }
  loc = -1;
  loc = glGetUniformLocation(chunkShader._pID, "frame_time");
  if (loc != -1) {
    glUniform1f(loc, (lastFrameTime - firstFrameTime) / 1000.0f );
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
            if (abs(cc->_ccx - floor(user->getCenter().x / BLOCK_PER_CHUNK)) > CHUNK_UNLOAD_RADIUS || abs(cc->_ccy - floor(user->getCenter().y / BLOCK_PER_CHUNK)) > CHUNK_UNLOAD_RADIUS) {
              delete cc;
            } else {
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
  }

  chunkShader.unbind();

  glFlush();

  Graphics::resetViewport();

  mVec3 head = user->getHead();
  renderBitmapString(10,10,"Pos: " + to_string(head.x) + " " + to_string(head.y) + " " + to_string(head.z), 0xffff00ff, false);

  setColor(0xffffffff);
  glBegin(GL_QUADS);
  glVertex2d((ax + bx) / 2.0 - 10, (ay + by) / 2.0 + 1);
  glVertex2d((ax + bx) / 2.0 - 10, (ay + by) / 2.0 - 1);
  glVertex2d((ax + bx) / 2.0 + 10, (ay + by) / 2.0 - 1);
  glVertex2d((ax + bx) / 2.0 + 10, (ay + by) / 2.0 + 1);
  glVertex2d((ax + bx) / 2.0 + 1, (ay + by) / 2.0 - 10);
  glVertex2d((ax + bx) / 2.0 - 1, (ay + by) / 2.0 - 10);
  glVertex2d((ax + bx) / 2.0 - 1, (ay + by) / 2.0 + 10);
  glVertex2d((ax + bx) / 2.0 + 1, (ay + by) / 2.0 + 10);
  glEnd();

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
    pos.fromCartesian(user->getLook());

    pos.phi -= 0.01*dx;
    pos.theta -= 0.01*dy;
    pos.r = 1;
 
    pos.reNormalize();

    sVec3 dir = pos.toCartesian();

    user->EntityTryLook(dir);
  }
  return 0;
}

int MainGameCanvas::guiEventManager(gui_event evt, int mx, int my, set<key_location>& down, Canvas* me) {
  bool in = me->isIn(mx, my);
  if(evt._type == evt.evt_down && evt._key._keycode == 0 && evt._key._type == evt._key.type_mouse) {
    GLdouble pos3D_ax = 0, pos3D_ay = 0, pos3D_az = 0;

    // get 3D coordinates based on window coordinates

    gluUnProject((view.viewport[0] + view.viewport[2])/2, (view.viewport[1] + view.viewport[3]) / 2, 0,
      view.model_view, view.projection, view.viewport,
      &pos3D_ax, &pos3D_ay, &pos3D_az);

    vec3<double> rayori = { pos3D_ax, pos3D_ay, pos3D_az };
    vec3<double> raydir = rayori - view.cameraEye;

    if(raydir.sqrlen() > 0) {
      raydir = raydir.norm() * 5;
      list<iVec3> res = find_voxels(rayori, rayori + raydir);
      for (auto&& it : res) {
        setBlock(it, 0, {0,0});
      }
    }
  }
  return 0;
}

void bindGameScreenLabels() {

  Transpose iden;
  iden.setIdentity();
  iden.read(MainGameCanvas::view.model_view);

  firstFrameTime = lastFrameTime = chrono::duration_cast< chrono::milliseconds >(
    chrono::system_clock::now().time_since_epoch()).count();

  /*for (int cx = 0; cx < 16; cx++) {
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
  }*/
}