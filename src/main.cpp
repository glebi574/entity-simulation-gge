#include "ui.h"
#include "entity-handler.h"

int main() {
  srand(time(0));

  UIHandler uih;
  CEntity::ui = &uih;
  if (uih.init())
    return 1;
  CellManager::sm = &uih.scenes[1];
  EntityHandler eh;
  eh.initial_spawn();
  while (!glfwWindowShouldClose(uih.gw.window)) {
    frame_time_update();
    uih.proc();
    eh.proc();
    frame_limiter();
  }
  uih.gw.clear();

  return 0;
}