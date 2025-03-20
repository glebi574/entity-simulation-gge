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

HCEntity::HCEntity() {
  nn.set_node_amount(3, 4, 3);
  nn.randomize_ratios();
}

void HCEntity::update_mo() {
  ManagedObject* mo = &cm.sm->meshes[cm.vo];
  mo->pos[0] = x;
  mo->pos[1] = y;
  mo->angle[2] = angle;
  mo->update_vo();
}

void HCEntity::proc() {
  nn.calculate({(x - EntityHandler::left) / EntityHandler::width, (y - EntityHandler::bottom) / EntityHandler::top, angle});
  float ma = nn.nodes_o[0] * TAU, ms = nn.nodes_o[1];
  x += cos(ma) * ms * 10;
  y += sin(ma) * ms * 10;
  angle += (nn.nodes_o[2] * TAU - angle) * 0.1f;
  update_mo();
}