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
  float health = 0.f;
  for (auto& [uid, cell] : cm.cells) {
    speed += cell.speed;
    health += cell.health;
  }
  offspring_iterator = ECell::max.health * cm.cells.size() / health;
  speed /= cm.cells.size();
  rotation_speed = speed / 60.f;
  offspring_counter = randi(-52, -16);
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
    dx1
    dy1
    ..
    dx6
    dy6

    angle
    health%

  Output:
    movement speed
    movement angle
    angle
*/

struct DistanceTool {
  HCEntity* e = nullptr;
  float dx = 0.f, dy = 0.f, lenght2 = FLT_MAX;

  DistanceTool() { }

  DistanceTool(HCEntity* e) {
    this->e = e;
  }

  void calculate(HCEntity* _e) {
    dx = _e->x - e->x + e->chunk->x1 - _e->chunk->x1 + (_e->chunk_x - e->chunk_x) * SceneChunk::width;
    dy = _e->y - e->y + e->chunk->y1 - _e->chunk->y1 + (_e->chunk_y - e->chunk_y) * SceneChunk::height;
    lenght2 = dx * dx + dy * dy;
  }
};

bool HCEntity::proc() {
  // Cell stuff

  for (auto& [eid, cell] : cm.cells) {
    if (cell.is_alive && cell.c_health < 0.f) {
      if (--alive_cell_amount == 0) {
        remove();
        return true;
      }
      cell.is_alive = false;
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

  // Generating offspring
  if ((offspring_counter += offspring_iterator) > offspring_timer) {
    eh->new_entity(x, y, *this, 1.f);
    offspring_counter = 0;
  }

  // Getting data for neural network

  // Closest entities
  std::vector<DistanceTool> entities;

  for (int i = chunk_x - 1; i <= chunk_x + 1; ++i) {
    for (int n = chunk_y - 1; n <= chunk_y + 1; ++n) {
      int cx = (i + CHUNKS_X) % CHUNKS_X, cy = (n + CHUNKS_Y) % CHUNKS_Y;
      for (auto& [eid, e] : eh->chunks[cx][cy].entities) {
        if (id == eid)
          continue;
        entities.emplace_back(DistanceTool(e));
        entities.back().calculate(this);
      }
    }
  }
  std::sort(entities.begin(), entities.end(), [](const DistanceTool& a, const DistanceTool& b) {return a.lenght2 < b.lenght2; });
  std::vector<float> input(14, 0.f);
  for (int i = 0; i < std::min<size_t>(6, entities.size()); ++i) {
    input[i * 2] = entities[i].dx;
    input[i * 2 + 1] = entities[i].dy;
  }
  input[12] = angle / static_cast<float>(TAU);
  input[13] = static_cast<float>(alive_cell_amount) / static_cast<float>(cm.cells.size());
  nn.calculate(input);

  // Transforming output values

  angle += nn.nodes_o[2] * rotation_speed;
  float ms = nn.nodes_o[0] * speed, ma = nn.nodes_o[1] * PI,
    nx = x + cos(ma) * ms,
    ny = y + sin(ma) * ms;

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

  // Entity collisions

  DistanceTool& ce = entities[0];
  if (ce.e == nullptr)
    return false;
  float r2 = radius + ce.e->radius;
  if (ce.lenght2 > r2 * r2)
    return false;
  // cell is attacking cell
  for (auto& [eid1, cell] : cm.cells)
    if (cell.is_alive)
      // ecell is attacked cell
      for (auto& [eid2, ecell] : ce.e->cm.cells) {
        if (!ecell.is_alive)
          continue;
        float
          cdx = ce.dx - ecell.x * cos(ce.e->angle) + cell.x * cos(angle),
          cdy = ce.dy - ecell.y * sin(ce.e->angle) + cell.y * sin(angle);
        if (cdx * cdx + cdy * cdy < cell_radius2) {
          ecell.c_health -= cell.damage;
          continue;
        }
      }
  return false;
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