#ifndef G_ENTITY
#define G_ENTITY

#include "ui.h"

static float cell_size = 16.f;

struct CellData {
  float regeneration = 0, health = 0, armor = 0, damage = 0, speed = 0, rotation_speed = 0;
};

class ECell : public CellData {
public:
  inline static const CellData
    min{ 0.1f, 0.4f, 0.f, 0.f, 0.f, 0.f },
    max{ 2.f, 10.f, 1.f, 5.f, 20.f, 1.f },
    avg_max{0.5f, 4.f, 0.4f, 2.f, 6.f, 0.4f};
  bool is_alive = true;

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

  static void from_uid(uint16_t uid, int8_t& x, int8_t& y);

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