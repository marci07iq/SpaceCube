#include "GameCanvas.h"

Shader chunkShader;
Shader entityShader;


GLuint textures;
GLuint texSampler;

bool MainGameCanvas::lockMouse = 0;

OpenGLData MainGameCanvas::view;

uint64_t firstFrameTime;

guid_t userGUID = 0x0123456789abcdef;
NetPlayer* user;
NetworkC* Connection;

int frameTime = 10; //33ms;


void MainGameCanvas::normalizeAngles() {

}
void MainGameCanvas::renderManager(Canvas* c, int ax, int ay, int bx, int by, set<key_location>& down) {
  uint64_t thisFrameTime = chrono::duration_cast< chrono::milliseconds >(
    chrono::system_clock::now().time_since_epoch()).count();

  sVec3 userMove = { user->getLook().x, user->getLook().y, 0};
  mpsVec3 resVel = mpsVec3(0);
  if(userMove.sqrlen() > 0) {
    userMove.norm();
    if (isDown(down, key_location(key('W', key::type_key)))) {
      resVel += (userMove * user->getSpeed());
    }
    if (isDown(down, key_location(key('D', key::type_key)))) {
      resVel += (crs(userMove, {0,0,1}) * user->getSpeed());
    }
    if (isDown(down, key_location(key('S', key::type_key)))) {
      resVel += (-userMove * user->getSpeed());
    }
    if (isDown(down, key_location(key('A', key::type_key)))) {
      resVel += (-crs(userMove, { 0,0,1 }) * user->getSpeed());
    }
    if (isDown(down, key_location(key(GLFW_KEY_LEFT_CONTROL, key::type_key)))) {
      resVel *= 3;
    } else {
      resVel *= 1.5;
    }
    if (isDown(down, key_location(key(' ', key::type_key)))) {
      if (user->_inWorld) { //Jump from ground
        resVel += mpsVec3(0, 0, sqrt(-2*1.5*G.z) / SC_SUBTICK_TIME);
      }
      /*if(user->_inWorld) { //Jump from ground
        user->setVel(user->getVelocity() + sqrtVec<acc_type_mperss>(-G*2) * user->_friction);
      }*/
    }
    /*if (isDown(down, key_location(key(GLFW_KEY_LEFT_SHIFT, key::type_key)))) {
      resVel += (G/10 * user->getSpeed());
    }*/
  }

  if (!user->_inWorld) {
    resVel = 0;
  }

  user->_selfAccel = resVel;

  doTicks(size_t((thisFrameTime - firstFrameTime) / 1000.0 / SC_SUBTICK_TIME));

  glDepthRange(0.01, 256);
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  // Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //glAlphaFunc(GL_GREATER, 0.1);

  chunkShader.bind();

  float cameraM[16];

  Transpose camview;
  camview.createLook(user->getHead(), user->getLook());
  camview.project(CONS_PI/2, (bx-ax)*1.0f/(by-ay), 256, 0.01);
  camview.transpose();
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
    glUniform1f(loc, (thisFrameTime - firstFrameTime) / 1000.0f );
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

  //glFlush();

  Graphics::resetViewport();

  mVec3 head = user->getFeetCenter();
  renderBitmapString(10,10,"Pos: " + to_string(head.x) + " " + to_string(head.y) + " " + to_string(head.z), 0xffff00ff, false);

  setColor(0xffffffff);
  Gll::gllBegin(Gll::GLL_QUADS);
  Gll::gllVertex((ax + bx) / 2.0 - 10, (ay + by) / 2.0 - 1);
  Gll::gllVertex((ax + bx) / 2.0 - 10, (ay + by) / 2.0 + 1);
  Gll::gllVertex((ax + bx) / 2.0 + 10, (ay + by) / 2.0 - 1);
  Gll::gllVertex((ax + bx) / 2.0 + 10, (ay + by) / 2.0 + 1);
  Gll::gllVertex((ax + bx) / 2.0 + 1, (ay + by) / 2.0 - 10);
  Gll::gllVertex((ax + bx) / 2.0 - 1, (ay + by) / 2.0 - 10);
  Gll::gllVertex((ax + bx) / 2.0 - 1, (ay + by) / 2.0 + 10);
  Gll::gllVertex((ax + bx) / 2.0 + 1, (ay + by) / 2.0 + 10);
  Gll::gllEnd();

  glFlush();
}

int MainGameCanvas::resizeManager(Canvas* c, int x, int y) {
  return 0;
}
int MainGameCanvas::mouseEntryManager(Canvas* c, int state) {
  //mousebuttons = 0;
  return 0;
}
int MainGameCanvas::mouseMoveManager(Canvas* c, int x, int y, int ox, int oy, set<key_location>& down) {
  int mrsx = Graphics::current->width / 2, mrsy = Graphics::current->height / 2;

  if (x != mrsx || y != mrsy) {
    int dx = x - mrsx;
    int dy = y - mrsy;
    //glutWarpPointer(100, glutGet(GLUT_WINDOW_HEIGHT) - 100);
    glfwSetCursorPos(Graphics::current->rawHwnd, Graphics::current->width / 2, Graphics::current->height / 2);

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

int MainGameCanvas::guiEventManager(Canvas* c, gui_event& evt, int mx, int my, set<key_location>& down) {
  bool in = c->isIn(mx, my);
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
    if (evt._type == evt.evt_down && evt._key._keycode == GLFW_MOUSE_BUTTON_LEFT && evt._key._type == evt._key.type_mouse) {
      auto it = res.begin();
      bool go = true;
      while(it != res.end() && go) {
        bool success;
        BlockPos pos;
        pos = getBlock(it->x, it->y, it->z, 0, success);
        if (success && pos.b->_ID != 0) {
          go = false;
          trysetBlock(*it, 0, Block(0));
        }
        ++it;
      }
    }
    if (evt._type == evt.evt_down && evt._key._keycode == GLFW_MOUSE_BUTTON_RIGHT && evt._key._type == evt._key.type_mouse) {
      auto it = res.begin();
      bool go = true;
      iVec3 placeTo = *it;
      while ((++it) != res.end() && go) {
        bool success;
        BlockPos pos;
        pos = getBlock(it->x, it->y, it->z, 0, success);
        if (success && pos.b->_ID != 0) {
          go = false;
          trysetBlock(placeTo, 0, Block(9));
        }
        placeTo = *it;
      }
    }
  }
  return 0;
}

void bindGameScreenLabels() {

  Transpose iden;
  iden.setIdentity();
  iden.read(MainGameCanvas::view.model_view);

  firstFrameTime = chrono::duration_cast< chrono::milliseconds >(
    chrono::system_clock::now().time_since_epoch()).count();
}
