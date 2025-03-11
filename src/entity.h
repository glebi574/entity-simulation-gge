#ifndef G_ENTITY
#define G_ENTITY

#include "ui.h"

struct CellData {
  float regeneration = 0, health = 0, armor = 0, damage = 0, speed = 0, rotation_speed = 0;
};

class ECell : public CellData {
public:
  inline static const CellData
    min{ 0.1, 0.4, 0, 0, 0, 0 },
    max{ 2, 10, 1, 5, 20, 1 },
    avg_max{0.5, 4, 0.4, 2, 6, 0.4};

  ECell();
};

class CellManager {
public:
  std::unordered_map<uint16_t, ECell> cells;

  CellManager();

  ECell& operator()(int8_t x, int8_t y);

  const ECell& operator()(int8_t x, int8_t y) const;
};

class CEntity {
public:
  static UIHandler* ui;

  CEntity(UIHandler* ui);
};

#endif