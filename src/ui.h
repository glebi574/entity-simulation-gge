#ifndef G_UI_UTILS
#define G_UI_UTILS

#include "draw-utils.h"

class UIHandler {
public:
  GWindow gw;
  std::vector<SceneManager> scenes;
  // Index of current scene
  int cs_index = 0;

  UIHandler();

  int init();

  void handler();

  void proc();

  VObject* new_rectangle(const std::vector<float>& rectangle, uint32_t color);

  VObject* new_panel(const std::vector<float>& rectangle, float frame_width, uint32_t inner_color, uint32_t frame_color);
};

#endif