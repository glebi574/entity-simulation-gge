#include "entity-handler.h"

/*
  Input:
    x
    y
    angle

  Output:
    movement angle
    movement speed
    angle
*/

bool check_chunk_change(float& x, float& y, float& nx, float& ny) {
  return
    static_cast<int>(x) / SceneChunk::width == static_cast<int>(nx) / SceneChunk::width &&
    static_cast<int>(y) / SceneChunk::width == static_cast<int>(ny) / SceneChunk::width;
}

HCEntity::HCEntity() {
  nn.set_node_amount(3, 4, 3);
  nn.randomize_ratios();
}

HCEntity::HCEntity(EntityHandler* eh) {
  nn.set_node_amount(3, 4, 3);
  nn.randomize_ratios();
  this->eh = eh;
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
  nn.calculate({(x - EntityHandler::left) / EntityHandler::width, (y - EntityHandler::bottom) / EntityHandler::top, angle});
  angle += (nn.nodes_o[2] * TAU - angle) * 0.1f;
  float ma = nn.nodes_o[0] * TAU, ms = nn.nodes_o[1],
    nx = x + cos(ma) * ms * 10,
    ny = y + sin(ma) * ms * 10;
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
  if (check_chunk_change(x, y, nx, ny)) {
    eh->remove_chunk_link(this);
    eh->add_entity_to_chunks(this);
  }
  update_mo();
}