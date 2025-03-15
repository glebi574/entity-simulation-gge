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
  VObject* vo;

  static SceneManager* sm;

  ECell();

  ECell(int8_t x, int8_t y);

  uint32_t stats_to_color();

  void randomize_stats();
};

class CellManager {
public:
  std::unordered_map<uint16_t, ECell> cells;

  CellManager();

  void add(int8_t x, int8_t y);

  constexpr uint16_t to_uid(int8_t x, int8_t y) const;

  ECell& operator()(int8_t x, int8_t y);

  const ECell& operator()(int8_t x, int8_t y) const;
};

class CEntity {
public:
  static UIHandler* ui;
  CellManager cm;
  float x = 0, y = 0;

  CEntity();

  CEntity(CEntity* ce);
};

#endif