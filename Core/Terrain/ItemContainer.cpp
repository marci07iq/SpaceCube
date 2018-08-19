#include "ItemContainer.h"

#ifdef M_CLIENT
int ContainerGUI::locateField(iVec2 at) {
  for (int i = 0; i < elements.size(); i++) {
    if (band(elements[i].location == at)) {
      return i;
    }
  }
  return -1;
}
#endif