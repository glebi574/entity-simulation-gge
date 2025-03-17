#ifndef G_ENTITY_HANDLER
#define G_ENTITY_HANDLER

#include "entity.h"

#define CHUNKS_X 10
#define CHUNKS_Y 10

class HCEntity;

struct SceneChunk {
  int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
  std::unordered_map<uint32_t, HCEntity*> entities;

  static const int width = (PANEL1_RIGHT - PANEL1_LEFT) / CHUNKS_X, height = (PANEL1_TOP - PANEL1_BOTTOM) / CHUNKS_Y;

  SceneChunk();

  SceneChunk(int x, int y);
};

class HCEntity : public CEntity {
public:
  float x = 0, y = 0, angle = 0;
  bool was_updated = true;
  SceneChunk* chunk;

  HCEntity();

  void update_mo();

  void proc();
};

class EntityHandler {
public:
  std::vector<std::unique_ptr<HCEntity>> entities;
  SceneChunk chunks[CHUNKS_X][CHUNKS_Y];
  
  static const int left = PANEL1_LEFT, right = PANEL1_RIGHT, bottom = PANEL1_BOTTOM, top = PANEL1_TOP;

  EntityHandler();

  void new_entity(float x, float y);

  void initial_spawn();

  void proc();
};

#endif