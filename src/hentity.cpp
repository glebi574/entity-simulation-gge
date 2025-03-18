#include "entity-handler.h"

HCEntity::HCEntity() {

}

void HCEntity::update_mo() {
  if (!was_updated)
    return;
  was_updated = false;
  ManagedObject* mo = &cm.sm->meshes[cm.vo];
  mo->pos[0] = x;
  mo->pos[1] = y;
  mo->angle[2] = angle;
  mo->update_vo();
}

void HCEntity::proc() {

}