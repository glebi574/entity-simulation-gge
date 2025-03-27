#ifndef G_ENTITY
#define G_ENTITY

#include "ui.h"

struct CellData;

static float cell_size = 16.f, cell_radius = cell_size / sqrt(3) * (2.f), cell_radius2 = cell_radius * cell_radius * 4.f;

struct CellData {
  float health = 0.f, damage = 0.f, speed = 0.f;
  static uint8_t offsets[3];

  float& operator[](uint8_t index);

  const float& operator[](uint8_t index) const;
};

class ECell : public CellData {
public:
  inline static const CellData
    min = { 40.f, 4.f, 0.f },
    max{ 80.f, 16.f, 12.f },
    avg_max{ 50.f, 6.f, 4.f },
    range{ max.health - min.health, max.damage - min.damage, max.speed - min.speed };
  CellData local_max = avg_max;
  bool is_alive = true;
  int vertex_index = 0;
  float x = 0.f, y = 0.f, c_health = 0.f, regeneration = 0.f;

  ECell();

  uint32_t stats_to_color();

  void randomize_stats();

  void update_regeneration();
};

class CellManager {
public:
  std::unordered_map<uint16_t, ECell> cells;
  VObject* vo;

  static SceneManager* sm;

  CellManager();

  ECell* add(int8_t x, int8_t y);

  ECell* add(int16_t uid);

  void create_vo();

  static constexpr uint16_t to_uid(int8_t x, int8_t y);

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
};

#endif