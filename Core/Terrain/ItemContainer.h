#pragma once

#include "../Blocks/Block_Types.h"

struct Item {
  Block block;
  //Meta
  size_t count;
};

struct ContainerSlot {
  iVec2 location;
  Item contents;
};

#ifdef M_CLIENT
class ContainerGUI : public GUIElement {
#else
class ContainerGUI {
#endif
public:
  vector<ContainerSlot> elements;
  iVec2 containerSize;
  string containerName;

  int locateField(iVec2 at);
#ifdef M_CLIENT
  ContainerGUI(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor) :
    GUIElement(lname, llocation, lbg, lactive, ltextColor, NULL) {
  }
  /*int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event evt, int mx, int my, set<key_location>& down);
  void render(set<key_location>& down);*/
  ~ContainerGUI();
#endif
};