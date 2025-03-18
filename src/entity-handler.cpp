#include "entity-handler.h"

void get_chunk_indexes(float x, float y, int& ix, int& iy) {
  ix = (static_cast<int>(x) - PANEL1_LEFT) / SceneChunk::width;
  iy = (static_cast<int>(y) - PANEL1_BOTTOM) / SceneChunk::height;
}

EntityHandler::EntityHandler() {
  for (int i = 0; i < CHUNKS_X; ++i)
    for (int n = 0; n < CHUNKS_Y; ++n)
      chunks[i][n] = SceneChunk(i * SceneChunk::width, n * SceneChunk::height);
}

void EntityHandler::new_entity(float x, float y) {
  entities.emplace_back(std::make_unique<HCEntity>());
  HCEntity* ce = entities.back().get();
  ce->cm.add(0, 0);
  ce->cm(0, 0).randomize_stats();
  ce->cm.create_vo();
  ce->x = x;
  ce->y = y;
  add_entity_to_chunks(ce);
  ce->update_mo();
}

void EntityHandler::remove_chunk_link(HCEntity* e) {
  e->chunk->entities.erase(e->id);
}

void EntityHandler::add_entity_to_chunks(HCEntity* e) {
  int cx, cy;
  get_chunk_indexes(e->x, e->y, cx, cy);
  e->chunk = &chunks[cx][cy];
  chunks[cx][cy].entities[e->id] = e;
}

void EntityHandler::initial_spawn() {
  for (int i = 0; i < 10; ++i) {
    new_entity(frand(left, right), frand(bottom, top));
  }
}

void EntityHandler::proc() {

}