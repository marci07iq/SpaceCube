#pragma once

#include "../Blocks/Block_Types.h"

struct MetaBlock {
  Block block;
  void* meta;
};

struct ItemStack {
  MetaBlock block;
  //Meta
  size_t count;
};

struct ContainerSlot {
  iVec2 location;
  ItemStack contents;
};

#ifdef M_CLIENT
class ContainerGUI : public NGin::Graphics::GUIElement {
#else
class ContainerGUI {
#endif
public:
  vector<ContainerSlot> elements;
  string containerName;

  int locateField(iVec2 at);
#ifdef M_CLIENT
  ContainerGUI(string lname, NGin::Graphics::LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor) :
    GUIElement(lname, llocation, lbg, lactive, ltextColor, NULL) {
  }
  /*int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event evt, int mx, int my, set<key_location>& down);
  void render(set<key_location>& down);*/
  ~ContainerGUI();
#endif
};