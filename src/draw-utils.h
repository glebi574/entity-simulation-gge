#ifndef G_DRAW_UTILS
#define G_DRAW_UTILS

#include "g-utils.h"

class MObject {
public:
  VObject* vo;
  float
    angle[3] = { 0, 0, 0 },
    scale[3] = { 1, 1, 1 },
    pos[3] = { 0, 0, 0 };
  float acceleration = 0, dx = 0, dy = 0;

  MObject(VObject* vo);

  void update_vo();
};

void window_pre_init(GWindow* gw);

void window_post_init(GWindow* gw);

void additional_draw_function(GWindow* gw);

#endif