#include "entity.h"

CEntity::CEntity() {
  id = id_counter++;
}

UIHandler* CEntity::ui;
uint32_t CEntity::id_counter = 0;