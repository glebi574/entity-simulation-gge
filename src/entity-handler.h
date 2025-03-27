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

  static const int width = (UIHandler::right - UIHandler::left) / CHUNKS_X, height = (UIHandler::top - UIHandler::bottom) / CHUNKS_Y;

  SceneChunk();

  SceneChunk(int x, int y);
};

class HCEntity : public CEntity {
public:
  int alive_cell_amount = 0;
  float x = 0.f, y = 0.f, angle = 0.f, radius = 0.f, speed = 0.f, rotation_speed = 0.f, offspring_iterator = 0.f, offspring_counter = 0.f;
  uint8_t chunk_x = 0, chunk_y = 0;
  SceneChunk* chunk = nullptr;
  EntityHandler* eh = nullptr;
  NeuralNetwork nn;

  inline static const float offspring_timer = 1200.f;

  HCEntity();

  HCEntity(EntityHandler* eh);

  // Creates VObject and calculates stuff based on cells
  void init();

  void update_mo();

  bool check_chunk_change(float& x, float& y) const;

  void proc();

  void calculate_radius();

  void remove();

  void update_cell_alpha(ECell& cell, float alpha);
};

class EntityHandler {
public:
  std::unordered_map<uint32_t, std::unique_ptr<HCEntity>> entities;
  SceneChunk chunks[CHUNKS_X][CHUNKS_Y];
  int new_entity_counter = 9;
  
  inline static const int new_entity_timer = 0, initial_entities = 1000, min_entities = 100;
  static const int left = UIHandler::left, right = UIHandler::right, bottom = UIHandler::bottom, top = UIHandler::top,
    width = right - left, height = top - bottom;

  EntityHandler();

  // Creates random single-cell entity at given position
  void new_entity(float x, float y);

  // Creates entity, based on other one and may mutate it
  void new_entity(float x, float y, HCEntity& e, float mutation_chance, bool keep_c_stats = false);

  // Removes entity from its chunk; to be used with add_entity_to_chunk to update entity's chunk pointer
  void remove_chunk_link(HCEntity* e);

  // Adds entity to chunk and updates its chunk pointer
  void add_entity_to_chunks(HCEntity* e);

  void initial_spawn();

  void proc();
};

void get_chunk_indexes(float x, float y, int& ix, int& iy);

#endif