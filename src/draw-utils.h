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

  ManagedObject(VObject* vo);

  void update_vo();
};

class SceneManager {
public:
  std::unordered_map<VObject*, ManagedObject> meshes;
  GWindow* gw = nullptr;

  static std::vector<std::vector<float>> base_triangle, base_square;

  SceneManager();

  SceneManager(GWindow* gw);

  void add(VObject* vo);

  void remove(VObject* vo);

  void set_base_meshes();

  VObject* new_triangle(float x, float y, float scale, uint32_t color);

  VObject* new_square(float x, float y, float scale, uint32_t color);
};

// Called before init() of GWindow
void window_pre_init(GWindow* gw);

// Called after init() of GWindow
void window_post_init(GWindow* gw);

// Called before drawing cycle
void window_update(GWindow* gw, void (*update_function)(GWindow*));

#endif