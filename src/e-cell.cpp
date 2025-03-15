#include "entity.h"

// Offset for the cell pattern
float cell_size = 16, tdx = cell_size * sqrt(3) / 2, tdy = cell_size / 2;

ECell::ECell() {
  randomize_stats();
}

ECell::ECell(int8_t x, int8_t y) {
  randomize_stats();
  MeshConstructor mc;
  bool flip = (x + y) % 2;
  mc.add_triangle(tdx * x + (flip ? 0 : -tdx / 3), tdy * y, flip ? -cell_size : cell_size, stats_to_color());
  vo = sm->add(mc);
}

uint32_t ECell::stats_to_color() {
  return rgba_to_color(
    (regeneration / max.regeneration + health / max.health + armor / max.armor) / 3 * 255,
    (speed / max.speed + rotation_speed / max.rotation_speed) / 2 * 255,
    damage / max.damage * 255,
    255);
}

void ECell::randomize_stats() {
  health = frand(min.health, avg_max.health);
  regeneration = frand(min.regeneration, avg_max.regeneration);
  armor = frand(min.armor, avg_max.armor);
  damage = frand(min.damage, avg_max.damage);
  speed = frand(min.speed, avg_max.speed);
  rotation_speed = frand(min.rotation_speed, avg_max.rotation_speed);
}

SceneManager* ECell::sm;