#include "draw-utils.h"

ManagedObject::ManagedObject(VObject* vo) {
  this->vo = vo;
}

void ManagedObject::update_vo() {
  vo->tm.set_angle(angle[0], angle[1], angle[2]);
  vo->tm.set_scale(scale[0], scale[1], scale[2]);
  vo->tm.set_offset(pos[0], pos[1], pos[2]);
}

SceneManager::SceneManager() { }

SceneManager::SceneManager(GWindow* gw) {
  this->gw = gw;
}

void SceneManager::add(VObject* vo) {
  meshes.insert({ vo, ManagedObject(vo) });
}

void SceneManager::remove(VObject* vo) {
  gw->remove_vo(vo);
  meshes.erase(vo);
}