#include "entity-handler.h"

void get_chunk_indexes(float x, float y, int& ix, int& iy) {
  ix = (static_cast<int>(x) - PANEL1_LEFT) / SceneChunk::width;
  iy = (static_cast<int>(y) - PANEL1_BOTTOM) / SceneChunk::height;
  if (ix == CHUNKS_X)
    --ix;
  if (iy == CHUNKS_Y)
    --iy;
}

EntityHandler::EntityHandler() {
  for (int i = 0; i < CHUNKS_X; ++i)
    for (int n = 0; n < CHUNKS_Y; ++n)
      chunks[i][n] = SceneChunk(i * SceneChunk::width + EntityHandler::left , n * SceneChunk::height + EntityHandler::bottom);
}

void EntityHandler::new_entity(float x, float y) {
  std::unique_ptr<HCEntity> e = std::make_unique<HCEntity>(this);
  HCEntity* ce = e.get();
  entities[ce->id] = std::move(e);
  ce->cm.add(0, 0);
  ce->cm(0, 0).randomize_stats();
  ce->cm.create_vo();
  ce->x = x;
  ce->y = y;
  add_entity_to_chunks(ce);
  ce->update_mo();
  ce->calculate_speed();
  ce->calculate_radius();
  ce->calculate_energy();
}

void EntityHandler::remove_chunk_link(HCEntity* e) {
  e->chunk->entities.erase(e->id);
}

void EntityHandler::add_entity_to_chunks(HCEntity* e) {
  int cx, cy;
  get_chunk_indexes(e->x, e->y, cx, cy);
  e->chunk = &chunks[cx][cy];
  e->chunk_x = cx;
  e->chunk_y = cy;
  chunks[cx][cy].entities[e->id] = e;
}

void EntityHandler::initial_spawn() {
  for (int i = 0; i < min_entities; ++i) {
    new_entity(frand(left, right), frand(bottom, top));
  }
}

void EntityHandler::proc() {
  for (auto& [id, e] : entities)
    e.get()->proc();
  for (int i = entities.size(); i < min_entities; ++i)
    new_entity(frand(left, right), frand(bottom, top));
}