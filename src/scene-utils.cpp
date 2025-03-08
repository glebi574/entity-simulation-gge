#include "draw-utils.h"

ManagedObject::ManagedObject(VObject* vo) {
  this->vo = vo;
}

void ManagedObject::update_vo() {
  vo->tm.set_angle(angle[0], angle[1], angle[2]);
  vo->tm.set_scale(scale[0], scale[1], scale[2]);
  vo->tm.set_offset(pos[0], pos[1], pos[2]);
}

void SceneManager::set_base_meshes() {
  float multiplier_triangle = TAU / 3, multiplier_square = TAU / 4, offset_square = TAU / 8;
  for (int i = 0; i < 3; ++i) {
    float angle = multiplier_triangle * i;
    base_triangle.emplace_back(std::vector<float>{ cos(angle), sin(angle) });
  }
  for (int i = 0; i < 4; ++i) {
    float angle = multiplier_square * i + offset_square;
    base_square.emplace_back(std::vector<float>{ cos(angle), sin(angle) });
  }
}

SceneManager::SceneManager() {
  if (base_triangle.empty())
    set_base_meshes();
}

SceneManager::SceneManager(GWindow* gw) {
  if (base_triangle.empty())
    set_base_meshes();
  this->gw = gw;
}

void SceneManager::add(VObject* vo) {
  meshes.insert({ vo, ManagedObject(vo) });
}

void SceneManager::remove(VObject* vo) {
  gw->remove_vo(vo);
  meshes.erase(vo);
}

void insert_vertex(std::vector<float>& to, const std::vector<float>& from, float scale) {
  to.emplace_back(from[0] * scale);
  to.emplace_back(from[1] * scale);
  to.emplace_back(0);
}

VObject* SceneManager::new_triangle(float x, float y, float scale, uint32_t color) {
  std::vector<float> positions;
  std::vector<uint32_t> colors;

  for (const std::vector<float>& pos : base_triangle) {
    insert_vertex(positions, pos, scale);
  }

  for (int i = 0; i < 3; ++i)
    colors.emplace_back(color);

  VObject* vo = gw->add_tvo(positions, colors);

  add(vo);
  ManagedObject* mo = &meshes.at(vo);
  mo->pos[0] = x;
  mo->pos[1] = y;
  mo->update_vo();

  return vo;
}

VObject* SceneManager::new_square(float x, float y, float scale, uint32_t color) {
  std::vector<float> positions;
  std::vector<uint32_t> colors;

  for (int i = 0; i < 3; ++i) {
    const std::vector<float>& pos = base_square[i];
    insert_vertex(positions, pos, scale);
  }
  for (int i = 2; i < 5; ++i) {
    const std::vector<float>& pos = base_square[i % 4];
    insert_vertex(positions, pos, scale);
  }

  for (int i = 0; i < 6; ++i)
    colors.emplace_back(color);

  VObject* vo = gw->add_tvo(positions, colors);

  add(vo);
  ManagedObject* mo = &meshes.at(vo);
  mo->pos[0] = x;
  mo->pos[1] = y;
  mo->update_vo();

  return vo;
}

std::vector<std::vector<float>> SceneManager::base_triangle, SceneManager::base_square;