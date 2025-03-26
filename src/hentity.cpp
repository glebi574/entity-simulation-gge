#include "entity-handler.h"

bool HCEntity::check_chunk_change(float& x, float& y) const {
  return
    static_cast<uint8_t>(x) / SceneChunk::width != chunk_x ||
    static_cast<uint8_t>(y) / SceneChunk::height != chunk_y;
}

HCEntity::HCEntity() {

}

HCEntity::HCEntity(EntityHandler* eh) {
  this->eh = eh;
}

void HCEntity::init() {
  eh->add_entity_to_chunks(this);
  cm.create_vo();
  update_mo();
  calculate_radius();
  alive_cell_amount = cm.cells.size();
  offspring_counter = randi(-96, -32);
  lifetime = lifetime_per_cell * cm.cells.size();
  max_lifetime = max_lifetime_per_cell * cm.cells.size();
  energy_usage = static_cast<float>(0.1 * sqrt(cm.cells.size()));
  for (auto& [uid, cell] : cm.cells)
    for (int i = 0; i < 6; ++i)
      stats[i] += cell[i];
  stats.speed /= cm.cells.size();
  stats.rotation_speed /= cm.cells.size();
}

void HCEntity::update_mo() {
  ManagedObject* mo = &cm.sm->meshes[cm.vo];
  mo->pos[0] = x;
  mo->pos[1] = y;
  mo->angle[2] = angle;
  mo->update_vo();
}

/*
  Chunk checks:

  // Mouse control
  double dnx, dny;
  CEntity::ui->gw.get_cursor_pos(dnx, dny);
  nx = dnx;
  ny = dny;

  // Chunk highlight
  if (current_vo != nullptr)
    CEntity::ui->gw.remove_vo(current_vo);
  MeshConstructor mc;
  mc.add_rectangle({ (float)chunk->x1, (float)chunk->y1, (float)chunk->x2, (float)chunk->y2 }, 0xffffff40);
  current_vo = CellManager::sm->add(mc);
  
*/

/*
    Neural Network:
  Input:
    closest_dx
    closest_dy
    angle
    health%

    regeneration
    health
    armor
    attack
    speed
    rotation_speed

    e_regeneration
    e_health
    e_armor
    e_attack
    e_speed
    e_rotation_speed

  Output:
    movement speed
    angle
*/

VObject* current_vo = nullptr;
void HCEntity::proc() {
  // Generating offspring
  if ((lifetime -= energy_usage) < 0.f)
    return remove();
  ++offspring_counter;
  if (offspring_counter > offspring_timer) {
    eh->new_entity(x, y, *this, 1.f);
    offspring_counter = 0;
  }
  // Getting data for neural network

  // Closest entity
  HCEntity* ce = nullptr;
  float cl = FLT_MAX, dx = 0.f, dy = 0.f;

  for (int i = chunk_x - 1; i < chunk_x + 2; ++i) {
    for (int n = chunk_y - 1; n < chunk_y + 2; ++n) {
      int cx = (i + CHUNKS_X) % CHUNKS_X, cy = (n + CHUNKS_Y) % CHUNKS_Y;
      float
        offset_x = chunk->x1 - eh->chunks[cx][cy].x1 - x + (cx - chunk_x) * SceneChunk::width,
        offset_y = chunk->y1 - eh->chunks[cx][cy].y1 - y + (cy - chunk_y) * SceneChunk::height;
      for (auto& [eid, e] : eh->chunks[cx][cy].entities) {
        if (e->group == group)
          continue;
        float
          cdx = e->x + offset_x,
          cdy = e->y + offset_y;
        float l = cdx * cdx + cdy * cdy;
        if (l < cl) {
          dx = cdx;
          dy = cdy;
          cl = l;
          ce = e;
        }
      }
    }
  }
  float p_health = static_cast<float>(alive_cell_amount) / static_cast<float>(cm.cells.size()),
    p_angle = angle / static_cast<float>(TAU);
  if (ce == nullptr) {
    std::vector<float> input(16, 0.f);
    input[2] = p_angle;
    input[3] = p_health;
    for (int i = 4; i < 10; ++i)
      input[i] = stats[i - 4];
    nn.calculate(input);
  }
  else {
    std::vector<float> input(16);
    input[0] = dx / SceneChunk::width;
    input[1] = dy / SceneChunk::height;
    input[2] = p_angle;
    input[3] = p_health;
    for (int i = 4; i < 10; ++i)
      input[i] = stats[i - 4];
    for (int i = 10; i < 16; ++i)
      input[i] = ce->stats[i - 10];
    nn.calculate(input);
  }

  // Transforming output values

  angle += nn.nodes_o[1] * stats.rotation_speed;
  float ms = nn.nodes_o[0] * stats.speed,
    nx = x + cos(angle) * ms,
    ny = y + sin(angle) * ms;

  // Border collisions

  if (nx < EntityHandler::left)
    nx += EntityHandler::width;
  else if (nx > EntityHandler::right)
    nx -= EntityHandler::width;
  if (ny < EntityHandler::bottom)
    ny += EntityHandler::height;
  else if (ny > EntityHandler::top)
    ny -= EntityHandler::height;
  x = nx;
  y = ny;
  if (check_chunk_change(x, y)) {
    eh->remove_chunk_link(this);
    eh->add_entity_to_chunks(this);
  }
  update_mo();

  // Cell stuff

  for (auto& [eid, cell] : cm.cells) {
    if (cell.is_alive && cell.c_health < 0.f) {
      cell.is_alive = false;
      --alive_cell_amount;
      if (destruction_check())
        return remove();
      update_cell_alpha(cell, 0.2f);
    }

    if (!cell.is_alive && cell.c_health >= cell.health) {
      cell.is_alive = true;
      ++alive_cell_amount;
      update_cell_alpha(cell, 1.f);
    }

    // Overheal is possible and it's feature ig
    if (cell.c_health < cell.health)
      cell.c_health += cell.regeneration;
  }

  // Entity collisions

  if (ce == nullptr)
    return;
  float r2 = radius + ce->radius;
  if (cl > r2 * r2)
    return;
  // cell is attacking cell
  for (auto& [eid1, cell] : cm.cells)
    if (cell.is_alive)
      // ecell is attacked cell
      for (auto& [eid2, ecell] : ce->cm.cells) {
        if (!ecell.is_alive)
          continue;
        float
          cdx = dx + ecell.x - cell.x,
          cdy = dy + ecell.y - cell.y;
        if (cdx * cdx + cdy * cdy < cell_radius2) {
          float attack = cell.damage - ecell.armor;
          ecell.c_health -= attack;
          lifetime += attack * (1.f - ecell.regeneration / ecell.max.regeneration);
          ++offspring_counter;
          continue;
        }
      }
  if (lifetime > max_lifetime)
    lifetime = max_lifetime;
}

template <typename T, typename TA>
void umap_to_vec(std::vector<T*>& output, std::unordered_map<TA, T>& umap) {
  output.reserve(umap.size());
  for (auto& [key, value] : umap)
    output.emplace_back(&value);
}

void HCEntity::calculate_radius() {
  int8_t fl = 0;
  ECell* fc = nullptr;
  for (auto& [uid, cell] : cm.cells) {
    int8_t cx, cy;
    CellManager::from_uid(uid, cx, cy);
    float cl = cell.x * cell.x + cell.y * cell.y;
    if (cl > fl) {
      fl = cl;
      fc = &cell;
    }
  }
  radius = sqrt(fc->x * fc->x + fc->y * fc->y) + cell_radius;
}

void HCEntity::remove() {
  CellManager::sm->remove(cm.vo);
  eh->remove_chunk_link(this);
  eh->entities.erase(id);
}

bool HCEntity::destruction_check() {
  if (alive_cell_amount == 0)
    return true;
  return false;
}

void HCEntity::update_cell_alpha(ECell& cell, float alpha) {
  VObject* vo = cm.vo;
  TriangleVO* tvo = reinterpret_cast<TriangleVO*>(vo);
  glBindVertexArray(vo->VAO);
  glBindBuffer(GL_ARRAY_BUFFER, vo->VBO);
  for (int i = 0; i < 3; ++i)
    tvo->vertexes[cell.vertex_index + i].color[3] = alpha;
  glBufferSubData(GL_ARRAY_BUFFER, cell.vertex_index * sizeof(Vertex), sizeof(Vertex) * 3, &tvo->vertexes[cell.vertex_index]);
}