#include "ui.h"

void u(GWindow* gw) {}

int main() {
  srand(time(0));

  GWindow gw;
  window_pre_init(&gw);
  if (gw.init())
    return 1;
  window_post_init(&gw);
  gw.window_proc(window_update, u);
  gw.clear();

  return 0;
}