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
  ce->x = x;
  ce->y = y;
  ce->init();
  ce->nn.set_node_amount(3, 4, 3);
  ce->nn.randomize_ratios();
}

void mutate_nn_values(std::vector<float>& vec) {
  for (float& v : vec)
    if (randf() <= 0.05f)
      v *= randf(0.8f, 1.2f);
}

void EntityHandler::new_entity(float x, float y, HCEntity& e, float mutation_chance, bool keep_c_stats) {
  std::unique_ptr<HCEntity> new_e = std::make_unique<HCEntity>(this);
  HCEntity* ce = new_e.get();
  entities[ce->id] = std::move(new_e);
  ce->cm.cells = e.cm.cells;
  ce->nn = e.nn;
  if (!keep_c_stats)
    for (auto& [id, cell] : ce->cm.cells) {
      cell.is_alive = true;
      cell.c_health = cell.health;
    }

  if (mutation_chance != 0.f && (mutation_chance == 1.f || randf() <= mutation_chance)) {
    // Cell mutation
    for (auto& [id, cell] : ce->cm.cells) {
      for (int i = 0; i < 6; ++i)
        if (randf() <= 0.1f) {
          cell[i] *= randf(0.8f, 1.2f);
          if (cell[i] < cell.min[i])
            cell[i] = cell.min[i];
          if (cell[i] > cell.max[i])
            cell[i] = cell.max[i];
        }
    }

    // Neural network mutation
    mutate_nn_values(ce->nn.biases_h);
    mutate_nn_values(ce->nn.biases_o);
    mutate_nn_values(ce->nn.nodes_h);
    mutate_nn_values(ce->nn.nodes_o);
    mutate_nn_values(ce->nn.weights_i);
    mutate_nn_values(ce->nn.weights_o);

    // Cell growth mutation

    std::unordered_set<uint16_t> free_space;
    for (auto& [uid, cell] : ce->cm.cells) {
      int8_t x, y;
      CellManager::from_uid(uid, x, y);
      for (int i = x - 1; i <= x + 1; ++i)
        for (int n = y - 1; n <= y + 1; ++n)
          if (!ce->cm.contains(i, n)) {
            free_space.emplace(CellManager::to_uid(i, n));
          }
    }
    std::vector<uint16_t> positions(free_space.begin(), free_space.end());
    while (positions.size() > 0 && randf() <= 0.2f) {
      int index = randi(positions.size());
      ECell* c = ce->cm.add(positions[index]);
      c->randomize_stats();
      positions.erase(positions.begin() + index);
    }
  }

  ce->x = x;
  ce->y = y;
  ce->init();
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
    new_entity(randf(left, right), randf(bottom, top));
  }
}

void EntityHandler::proc() {
  for (auto& [id, e] : entities)
    e.get()->proc();
  for (int i = entities.size(); i < min_entities; ++i)
    new_entity(randf(left, right), randf(bottom, top));
}