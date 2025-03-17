#include "entity.h"

// Offset for the cell pattern
float cell_size = 16, tdx = cell_size * sqrt(3) / 2, tdy = cell_size / 2;

ECell::ECell() {
  
}

/*
alternative color schemes:
  float
    r = (regeneration / max.regeneration + health / max.health + armor / max.armor) / 3,
    g = (speed / max.speed + rotation_speed / max.rotation_speed) / 2,
    b = damage / max.damage;
  if (r > g)
    if (r > b) {
      r = 1 - r * r;
    }
    else {
      b = 1 - b * b;
    }
  else
    if (g > b) {
      g = 1 - g * g;
    }
    else {
      b = 1 - b * b;
    }
  return rgba_to_color(r * 255, g * 255, b * 255, 255);

  float
    r = (regeneration / max.regeneration + health / max.health + armor / max.armor) / 3,
    g = (speed / max.speed + rotation_speed / max.rotation_speed) / 2,
    b = damage / max.damage;
  if (r > g)
    if (r > b)
      return rgba_to_color(r * 255, g * 128, b * 128, 255);
    else
      return rgba_to_color(r * 128, g * 128, b * 255, 255);
  else
    if (g > b)
      return rgba_to_color(r * 128, g * 255, b * 128, 255);
    else
      return rgba_to_color(r * 128, g * 128, b * 255, 255);
*/

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

void ECell::create_vo(int8_t x, int8_t y) {
  MeshConstructor mc;
  bool flip = (x + y) % 2;
  mc.add_triangle(tdx * x + (flip ? 0 : -tdx / 3), tdy * y, flip ? -cell_size : cell_size, stats_to_color());
  vo = sm->add(mc);
}

SceneManager* ECell::sm;