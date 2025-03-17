#ifndef G_UI_UTILS
#define G_UI_UTILS

#include "draw-utils.h"

#define PANEL1_LEFT -950
#define PANEL1_RIGHT 500
#define PANEL1_BOTTOM -550
#define PANEL1_TOP 550

class UIHandler {
public:
  GWindow gw;
  std::vector<SceneManager> scenes;
  // Index of current scene
  int cs_index = 0;
  // Coordinates
  static const int left = -1000, right = 1000, bottom = -563, top = 563,
    panel_left = PANEL1_LEFT, panel_right = PANEL1_RIGHT, panel_bottom = PANEL1_BOTTOM, panel_top = PANEL1_TOP,
    panel2_left = PANEL1_RIGHT + 25, panel2_right = 950, panel2_bottom = PANEL1_BOTTOM, panel2_top = PANEL1_TOP;
  static const uint32_t panel_inner_color = 0, panel_frame_color = 0x804040ff;

  UIHandler();

  int init();

  void handler();

  void proc();

  VObject* new_rectangle(const std::vector<float>& rectangle, uint32_t color);

  VObject* new_panel(const std::vector<float>& rectangle, float frame_width, uint32_t inner_color, uint32_t frame_color);
};

#endif