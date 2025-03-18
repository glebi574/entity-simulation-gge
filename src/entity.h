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

  uint32_t stats_to_color();

  void randomize_stats();
};

class CellManager {
public:
  std::unordered_map<uint16_t, ECell> cells;
  VObject* vo;

  static SceneManager* sm;

  CellManager();

  ECell* add(int8_t x, int8_t y);

  void create_vo();

  constexpr uint16_t to_uid(int8_t x, int8_t y) const;

  void from_uid(uint16_t uid, int8_t& x, int8_t& y) const;

  bool contains(int8_t x, int8_t y);

  ECell& operator()(int8_t x, int8_t y);

  const ECell& operator()(int8_t x, int8_t y) const;
};

class CEntity {
public:
  uint32_t id = 0;
  CellManager cm;

  static uint32_t id_counter;
  static UIHandler* ui;

  CEntity();

  CEntity(CEntity* ce);
};

#endif