#ifndef G_DRAW_UTILS
#define G_DRAW_UTILS

#include "g-utils.h"

class ManagedObject {
public:
  VObject* vo = nullptr;
  float
    angle[3] = { 0, 0, 0 },
    scale[3] = { 1, 1, 1 },
    pos[3] = { 0, 0, 0 };

  ManagedObject();

  ManagedObject(VObject* vo);

  void update_vo();
};

class MeshConstructor {
public:
  std::vector<float> pos;
  std::vector<uint32_t> colors;

  static std::vector<std::vector<float>> base_triangle, base_square;

  MeshConstructor();

  void set_base_meshes();

  // Adds right triangle at position with size(scale)
  void add_triangle(float x, float y, float scale, uint32_t color);

  // Adds triangle. pos = {x1, y1, x2, y2, x3, y3}
  void add_triangle(const std::vector<float>& pos, uint32_t color);

  // Adds square at position with size(scale)
  void add_square(float x, float y, float scale, uint32_t color);

  // Adds rectangle. pos = {x1, y1, x2, y2}
  void add_rectangle(const std::vector<float>& pos, uint32_t color);

  // Adds rectangle. pos = {x1, y1, x2, y2, x3, y3, x4, y4}
  void add_poly4(const std::vector<float>& pos, uint32_t color);
};

class SceneManager {
public:
  std::unordered_map<VObject*, ManagedObject> meshes;
  GWindow* gw = nullptr;

  SceneManager();

  SceneManager(GWindow* gw);

  void add(VObject* vo);

  VObject* add(MeshConstructor& mc);

  void remove(VObject* vo);
};

// Counts frames and updates related data, depends on frame_limiter()
void frame_time_update();

// Limits amount of frames per second, depends on frame_time_update()
void frame_limiter();

#endif