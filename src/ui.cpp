#include "ui.h"

UIHandler::UIHandler() {}

int UIHandler::init() {
  if (gw.init())
    return 1;

  scenes.emplace_back(SceneManager(&gw));
  scenes.emplace_back(SceneManager(&gw));
  SceneManager* scene = &scenes[0];
  for (int i = 0; i < 10; ++i) {
    MeshConstructor mc;
    std::vector<float> s = { -16, -16, 16, 16 };
    mc.add_rectangle(s, 0xffa000ff);
    ManagedObject* mo = &scene->meshes.at(scene->add(mc));
    mo->pos[0] = irand(-500, 500);
    mo->pos[1] = irand(-500, 500);
  }
  cs_index = 1;
  new_panel({ -200, -200, 200, 200 }, 20, 0x00000080, 0xff4040ff);
  gw.add_text("text text text text text", -200, 0, 1, 0xffffffff);
  
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
  for (auto& [vo, mo] : scenes[0].meshes) {
    mo.angle[2] += 0.03;
    mo.update_vo();
  }
  double x, y;
  glfwGetCursorPos(gw.window, &x, &y);
  int width, height;
  glfwGetWindowSize(gw.window, &width, &height);
  for (auto& [vo, mo] : scenes[1].meshes) {
    mo.pos[0] = x / width * 2000 - 1000;
    mo.pos[1] = -y / height * 2000 + 1000;
    mo.update_vo();
  }
}

void UIHandler::proc() {
  frame_time_update();
  handler();
  gw.window_proc();
  frame_limiter();
}