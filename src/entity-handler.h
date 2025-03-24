#ifndef G_ENTITY_HANDLER
#define G_ENTITY_HANDLER

#include "entity.h"
#include "neural-network.h"

#define CHUNKS_X 10
#define CHUNKS_Y 10

class HCEntity;
class EntityHandler;

struct SceneChunk {
  int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
  std::unordered_map<uint32_t, HCEntity*> entities;

  static const int width = (PANEL1_RIGHT - PANEL1_LEFT) / CHUNKS_X, height = (PANEL1_TOP - PANEL1_BOTTOM) / CHUNKS_Y;

  SceneChunk();

  SceneChunk(int x, int y);
};

class HCEntity : public CEntity {
public:
  float x = 0.f, y = 0.f, angle = 0.f, speed = 0.f, rotation_speed = 0.f, radius = 0.f;
  inline static const double energy_per_cell = 100.0;
  double energy = 0.0, energy_usage = 0.0, min_energy = 0.0, max_energy = 0.0;
  uint8_t chunk_x = 0, chunk_y = 0, invincibility_timer = 128;
  SceneChunk* chunk = nullptr;
  EntityHandler* eh = nullptr;
  NeuralNetwork nn;

  HCEntity();

  HCEntity(EntityHandler* eh);

  void update_mo();

  bool check_chunk_change(float& x, float& y) const;

  void proc();

  void calculate_speed();

  void calculate_radius();

  void calculate_energy();

  void remove();
};

class EntityHandler {
public:
  std::unordered_map<uint32_t, std::unique_ptr<HCEntity>> entities;
  SceneChunk chunks[CHUNKS_X][CHUNKS_Y];
  int min_entities = 100;
  
  static const int left = PANEL1_LEFT, right = PANEL1_RIGHT, bottom = PANEL1_BOTTOM, top = PANEL1_TOP,
    width = right - left, height = top - bottom;

  EntityHandler();

  // Creates random single-cell entity at given position
  void new_entity(float x, float y);

  // Removes entity from its chunk; to be used with add_entity_to_chunk to update entity's chunk pointer
  void remove_chunk_link(HCEntity* e);

  // Adds entity to chunk and updates its chunk pointer
  void add_entity_to_chunks(HCEntity* e);

  void initial_spawn();

  void proc();
};

void get_chunk_indexes(float x, float y, int& ix, int& iy);

#endif