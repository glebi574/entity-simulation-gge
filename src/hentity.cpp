#include "entity-handler.h"

/*
    Neural Network:
  Input:
    closest_dx
    closest_dy
    angle

  Output:
    movement angle
    movement speed
    angle
*/

bool HCEntity::check_chunk_change(float& x, float& y) const {
  return
    static_cast<int>(x) / SceneChunk::width == chunk_x &&
    static_cast<int>(y) / SceneChunk::width == chunk_y;
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
  calculate_speed();
  calculate_radius();
  alive_cell_amount = cm.cells.size();
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

void HCEntity::proc() {
  // Generating offspring
  ++offspring_counter;
  if (offspring_counter > offspring_timer) {
    if (randb())
      eh->new_entity(x, y, *this, 1.f);
    offspring_counter = 0;
  }
  // Getting data for neural network

  // Closest entity
  HCEntity* ce = nullptr;
  float cl = 0xffffffff;

  for (int i = chunk_x - 1; i < chunk_x + 2; ++i) {
    for (int n = chunk_y - 1; n < chunk_y + 2; ++n) {
      for (auto& [eid, e] : eh->chunks[(i + CHUNKS_X) % CHUNKS_X][(n + CHUNKS_Y) % CHUNKS_Y].entities) {
        if (eid == id)
          continue;
        float
          dx = e->x - e->chunk->x1 - x + chunk->x1 + (i - chunk_x) * SceneChunk::width,
          dy = e->y - e->chunk->y1 - y + chunk->y1 + (n - chunk_y) * SceneChunk::height,
          l = dx * dx + dy * dy;
        if (l < cl) {
          cl = l;
          ce = e;
        }
      }
    }
  }
  if (ce == nullptr)
    nn.calculate({ 0.f, 0.f, angle });
  else
    nn.calculate({ (ce->x - x) / SceneChunk::width, (ce->y - y) / SceneChunk::height, angle / static_cast<float>(TAU) });

  // Transforming output values

  angle += (nn.nodes_o[2] * 2.f - 1.f) * rotation_speed;
  float ma = nn.nodes_o[0] * TAU, ms = nn.nodes_o[1],
    nx = x + cos(ma) * ms * speed,
    ny = y + sin(ma) * ms * speed;

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
      speed *= speed_modifier;
      rotation_speed *= speed_modifier;
      --alive_cell_amount;
      if (destruction_check())
        return remove();
      update_cell_alpha(cell, 0.2f);
    }

    if (!cell.is_alive && cell.c_health >= cell.health) {
      cell.is_alive = true;
      speed /= speed_modifier;
      rotation_speed /= speed_modifier;
      ++alive_cell_amount;
      update_cell_alpha(cell, 1.f);
    }

    // Overheal is possible and it's feature ig
    if (cell.c_health < cell.health)
      cell.c_health += cell.regeneration + hit_bonus ? 0.1f / cm.cells.size() : 0.f;
  }
  if (hit_bonus)
    hit_bonus = false;

  // Entity collisions

  if (invincibility_timer)
    --invincibility_timer;
  if (invincibility_timer || ce == nullptr || ce->invincibility_timer)
    return;
  float dx = x - ce->x, dy = y - ce->y, r2 = radius + ce->radius;
  if (dx * dx + dy * dy > r2 * r2)
    return;
  // cell is attacking cell
  for (auto& [eid1, cell] : cm.cells)
    if (cell.is_alive)
      // ecell is attacked cell
      for (auto& [eid2, ecell] : ce->cm.cells) {
        if (!ecell.is_alive)
          continue;
        float
          cdx = dx + cell.x - ecell.x,
          cdy = dy + cell.y - ecell.y;
        if (cdx * cdx + cdy * cdy < cell_radius2) {
          ecell.c_health -= cell.damage - ecell.armor;
          hit_bonus = true;
          ++offspring_counter;
          continue;
        }
      }
}

template <typename T, typename TA>
void umap_to_vec(std::vector<T*>& output, std::unordered_map<TA, T>& umap) {
  output.reserve(umap.size());
  for (auto& [key, value] : umap)
    output.emplace_back(&value);
}

const float speed_multiplier = 4.f;

void HCEntity::calculate_speed() {
  std::vector<ECell*> cells;
  umap_to_vec(cells, cm.cells);
  std::sort(cells.begin(), cells.end(), [](const ECell* a, const ECell* b)
    { return a->speed > b->speed; });
  float divider = 1.f / speed_multiplier;
  for (const ECell* c : cells)
    speed += c->speed / (divider *= speed_multiplier);
  std::sort(cells.begin(), cells.end(), [](const ECell* a, const ECell* b)
    { return a->rotation_speed > b->rotation_speed; });
  divider = 1.f / speed_multiplier;
  for (const ECell* c : cells)
    rotation_speed += c->rotation_speed / (divider *= speed_multiplier);
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