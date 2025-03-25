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
  calculate_energy();
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
  // Generating offspring and limiting energy
  if (energy > max_energy) {
    offspring_counter += 3;
    energy = max_energy;
  }
  else
    ++offspring_counter;
  if (offspring_counter > 500) {
    eh->new_entity(x, y, *this, 1.f);
    energy = max_energy * 0.7f;
    offspring_counter = 0;
  }
  // Getting data for neural network
  
  // Closest entity
  HCEntity* ce = nullptr;
  float cl = 0xffffffff;
  
  for (int i = chunk_x - 1; i < chunk_x + 2; ++i) {
    if (i < 0 || i >= CHUNKS_X)
      continue;
    for (int n = chunk_y - 1; n < chunk_y + 2; ++n) {
      if (n < 0 || n >= CHUNKS_Y)
        continue;
      for (auto& [eid, e] : eh->chunks[i][n].entities) {
        if (eid == id)
          continue;
        float dx = e->x - x, dy = e->y - y, l = dx * dx + dy * dy;
        if (l < cl) {
          cl = l;
          ce = e;
        }
      }
    }
  }
  if (ce == nullptr)
    nn.calculate({0, 0, angle});
  else
    nn.calculate({ ce->x - x, ce->y - y, angle });

  // Transforming output values

  angle += (nn.nodes_o[2] * TAU - angle) * rotation_speed;
  float ma = nn.nodes_o[0] * TAU, ms = nn.nodes_o[1],
    nx = x + cos(ma) * ms * speed,
    ny = y + sin(ma) * ms * speed;

  // Border collisions

  if (nx < EntityHandler::left)
    nx = EntityHandler::left;
  else if (nx > EntityHandler::right)
    nx = EntityHandler::right;
  if (ny < EntityHandler::bottom)
    ny = EntityHandler::bottom;
  else if (ny > EntityHandler::top)
    ny = EntityHandler::top;
  x = nx;
  y = ny;
  if (check_chunk_change(x, y)) {
    eh->remove_chunk_link(this);
    eh->add_entity_to_chunks(this);
  }
  update_mo();

  if ((energy -= energy_usage) < min_energy)
    return remove();

  // Cell stuff

  for (auto& [eid, cell] : cm.cells) {
    if (cell.is_alive && cell.c_health < 0.f) {
      cell.is_alive = false;
      speed *= speed_modifier;
      rotation_speed *= speed_modifier;
      max_energy -= energy_per_cell;
      --alive_cell_amount;
      if (destruction_check())
        return remove();
      update_cell_color(cell, 0.5f);
    }

    if (!cell.is_alive && cell.c_health >= cell.health) {
      cell.is_alive = true;
      speed /= speed_modifier;
      rotation_speed /= speed_modifier;
      max_energy += energy_per_cell;
      ++alive_cell_amount;
      update_cell_color(cell, 1.f);
    }

    // Overheal is possible and it's feature ig
    if (cell.c_health < cell.health)
      cell.c_health += cell.regeneration;
  }

  // Entity collisions
  
  if (invincibility_timer)
    --invincibility_timer;
  if (invincibility_timer || ce == nullptr || ce->invincibility_timer)
    return;
  float dx = x - ce->x, dy = y - ce->y, r2 = radius + ce->radius;
  if (dx * dx + dy * dy > r2 * r2)
    return;
  for (auto& [eid1, cell] : cm.cells)
    for (auto& [eid2, ecell] : ce->cm.cells) {
      float
        cdx = dx + cell.x - ecell.x,
        cdy = dy + cell.y - ecell.y;
      if (cdx * cdx + cdy * cdy < cell_radius2) {
        float attack = cell.damage - ecell.armor;
        ecell.c_health -= attack;
        energy += attack;
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

void HCEntity::calculate_speed() {
  std::vector<ECell*> cells;
  umap_to_vec(cells, cm.cells);
  std::sort(cells.begin(), cells.end(), [](const ECell* a, const ECell* b) {
    return a->speed > b->speed;
    });
  float divider = 0.5f;
  for (const ECell* c : cells)
    speed += c->speed / (divider *= 2.f);
  std::sort(cells.begin(), cells.end(), [](const ECell* a, const ECell* b) {
    return a->rotation_speed > b->rotation_speed;
    });
  divider = 0.5f;
  for (const ECell* c : cells)
    rotation_speed += c->rotation_speed / (divider *= 2.f);
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

void HCEntity::calculate_energy() {
  for (auto& [uid, cell] : cm.cells) {
    cell.calculate_energy();
    energy_usage += cell.energy_usage;
  }
  energy = max_energy = pow(cm.cells.size(), 1.5) * energy_per_cell;
  min_energy = -max_energy / 2;
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

void HCEntity::update_cell_color(ECell& cell, float alpha) {
  VObject* vo = cm.vo;
  TriangleVO* tvo = reinterpret_cast<TriangleVO*>(vo);
  glBindVertexArray(vo->VAO);
  glBindBuffer(GL_ARRAY_BUFFER, vo->VBO);
  tvo->vertexes[cell.vertex_index].color[3] = alpha;
  glBufferSubData(GL_ARRAY_BUFFER, cell.vertex_index * sizeof(Vertex), sizeof(Vertex), &tvo->vertexes[cell.vertex_index]);
}