#include "entity-handler.h"

void get_chunk_indexes(float x, float y, int& ix, int& iy) {
  ix = static_cast<int>(x) % SceneChunk::width;
  iy = static_cast<int>(y) % SceneChunk::height;
}

SceneChunk::SceneChunk() {

}

SceneChunk::SceneChunk(int x, int y) {
  x1 = x;
  y1 = y;
  x2 = x + width;
  y2 = y + height;
}

HCEntity::HCEntity() {

}

void HCEntity::update_mo() {
  if (!was_updated)
    return;
  was_updated = false;
  for (auto& [index, cell] : cm.cells) {
    ManagedObject* mo = &ECell::sm->meshes[cell.vo];
    mo->pos[0] = x;
    mo->pos[1] = y;
    mo->angle[2] = angle;
    mo->update_vo();
  }
}

void HCEntity::proc() {

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
  ce->cm(0, 0).create_vo(0, 0);
  ce->x = x;
  ce->y = y;

  ce->update_mo();
}

void EntityHandler::initial_spawn() {
  for (int i = 0; i < 10; ++i) {
    new_entity(frand(left, right), frand(bottom, top));
  }
}

void EntityHandler::proc() {

}