#ifndef G_ENTITY
#define G_ENTITY

#include "ui.h"

struct CellData;

static float cell_size = 12.f, cell_radius = cell_size / sqrt(3) * (4.f), cell_radius2 = cell_radius * cell_radius * 4;

struct CellData {
  float regeneration = 0.f, health = 0.f, armor = 0.f, damage = 0.f, speed = 0.f, rotation_speed = 0.f;
  static uint8_t offsets[6];

  float& operator[](uint8_t index);

  const float& operator[](uint8_t index) const;
};

class ECell : public CellData {
public:
  inline static const CellData
    min    { 0.06f, 3.f, 0.f, 2.f, 0.f, 0.f },
    max    { 2.f, 40.f, 1.f, 12.f, 10.f, 0.2f },
    avg_max{ 0.6f, 7.f, 0.4f, 5.f, 3.f, 0.04f },
    range{ max.regeneration - min.regeneration, max.health - min.health,
      max.armor - min.armor, max.damage - min.damage,
      max.speed - min.speed, max.rotation_speed - min.rotation_speed };
  bool is_alive = true;
  int vertex_index = 0;
  float x = 0.f, y = 0.f, c_health = 0.f;

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