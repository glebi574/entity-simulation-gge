#include "ui.h"

int main() {
  srand(time(0));

  UIHandler h;
  if (h.init())
    return 1;
  while (!glfwWindowShouldClose(h.gw.window)) {
    h.proc();
  }
  h.gw.clear();

  return 0;
}