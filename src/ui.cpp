#include "ui.h"

UIHandler::UIHandler() {}

int UIHandler::init() {
  if (gw.init())
    return 1;

  for (int i = 0; i < 2; ++i)
    scenes.emplace_back(SceneManager(&gw));

  return 0;
}

VObject* UIHandler::new_rectangle(const std::vector<float>& rectangle, uint32_t color) {
  MeshConstructor mc;
  mc.add_rectangle(rectangle, color);
  return scenes[cs_index].add(mc);
}

VObject* UIHandler::new_panel(const std::vector<float>& rectangle, float frame_width, uint32_t inner_color, uint32_t frame_color) {
  std::vector<float> inner_rectangle = { rectangle[0] + frame_width, rectangle[1] + frame_width, rectangle[2] - frame_width, rectangle[3] - frame_width };
  MeshConstructor mc;
  mc.add_rectangle(inner_rectangle, inner_color);
  mc.add_poly4({
    rectangle[0], rectangle[1],
    inner_rectangle[0], inner_rectangle[1],
    inner_rectangle[2], inner_rectangle[1],
    rectangle[2], rectangle[1]
    }, frame_color);
  mc.add_poly4({
    inner_rectangle[2], inner_rectangle[1],
    rectangle[2], rectangle[1],
    rectangle[2], rectangle[3],
    inner_rectangle[2], inner_rectangle[3]
    }, frame_color);
  mc.add_poly4({
    rectangle[2], rectangle[3],
    inner_rectangle[2], inner_rectangle[3],
    inner_rectangle[0], inner_rectangle[3],
    rectangle[0], rectangle[3]
    }, frame_color);
  mc.add_poly4({
    inner_rectangle[0], inner_rectangle[3],
    rectangle[0], rectangle[3],
    rectangle[0], rectangle[1],
    inner_rectangle[0], inner_rectangle[1],
    }, frame_color);
  return scenes[cs_index].add(mc);
}

void UIHandler::handler() {
  
}

void UIHandler::proc() {
  handler();
  gw.window_proc();
}