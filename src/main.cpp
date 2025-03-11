#include "ui.h"
#include "entity.h"

int main() {
  srand(time(0));

  UIHandler uih;
  CEntity::ui = &uih;
  if (uih.init())
    return 1;
  while (!glfwWindowShouldClose(uih.gw.window)) {
    uih.proc();
  }
  uih.gw.clear();

  return 0;
}