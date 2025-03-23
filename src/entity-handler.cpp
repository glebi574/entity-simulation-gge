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

template <typename T, typename TA>
void umap_to_vec(std::vector<T*>& output, std::unordered_map<TA, T>& umap) {
  output.reserve(umap.size());
  for (auto& [key, value] : umap)
    output.emplace_back(&value);
}

void EntityHandler::new_entity(float x, float y) {
  entities.emplace_back(std::make_unique<HCEntity>(this));
  HCEntity* ce = entities.back().get();
  ce->cm.add(0, 0);
  ce->cm(0, 0).randomize_stats();
  ce->cm.create_vo();
  ce->x = x;
  ce->y = y;
  add_entity_to_chunks(ce);
  ce->update_mo();

  std::vector<ECell*> cells;
  umap_to_vec(cells, ce->cm.cells);
  std::sort(cells.begin(), cells.end(), [](const ECell* a, const ECell* b) {
    return a->speed > b->speed;
    });
  float divider = 0.5f;
  for (const ECell* c : cells)
    ce->speed += c->speed / (divider *= 2.f);
  std::sort(cells.begin(), cells.end(), [](const ECell* a, const ECell* b) {
    return a->rotation_speed > b->rotation_speed;
    });
  divider = 0.5f;
  for (const ECell* c : cells)
    ce->rotation_speed += c->rotation_speed / (divider *= 2.f);

  int8_t fl = 0;
  ECell* fc = nullptr;
  for (auto& [uid, cell] : ce->cm.cells) {
    int8_t cx, cy;
    CellManager::from_uid(uid, cx, cy);
    float cl = static_cast<float>(cx) * cx * 4 + static_cast<float>(cy) * cy; // I approximate that x axis is 2 times bigger per cell
    if (cl > fl) {
      fl = cl;
      fc = &cell;
    }
  }
  float rx = static_cast<TriangleVO*>(ce->cm.vo)->vertexes[0].pos[0],
    ry = static_cast<TriangleVO*>(ce->cm.vo)->vertexes[0].pos[1];
  rx += (rx > 0 ? cell_size : -cell_size) * 2.f;
  ry += (ry > 0 ? cell_size : -cell_size) * 0.5f;
  ce->radius = sqrt(rx * ry + ry * ry);
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
  for (int i = 0; i < 100; ++i) {
    new_entity(frand(left, right), frand(bottom, top));
  }
}

void EntityHandler::proc() {
  for (std::unique_ptr<HCEntity>& e : entities)
    e.get()->proc();
}