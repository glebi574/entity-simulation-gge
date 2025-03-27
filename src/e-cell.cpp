#include "entity.h"

ECell::ECell() {
  int i = randi(3);
  local_max[i] = max[i];
}

float& CellData::operator[](uint8_t index) {
  return *(reinterpret_cast<float*>(this) + offsets[index]);
}

const float& CellData::operator[](uint8_t index) const {
  return *(reinterpret_cast<const float*>(this) + offsets[index]);
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
    (health - min.health) / range.health * 192 + 63,
    (speed - min.speed) / range.speed * 192 + 63,
    (damage - min.damage) / range.damage * 192 + 63,
    255);
}

void ECell::randomize_stats() {
  for (int i = 0; i < 3; ++i)
    (*this)[i] = randf(min[i], avg_max[i]);
  c_health = health;
  update_regeneration();
}

void ECell::update_regeneration() {
  regeneration = health / 30.f;
}

uint8_t CellData::offsets[] = {
  offsetof(CellData, health) / sizeof(float),
  offsetof(CellData, damage) / sizeof(float),
  offsetof(CellData, speed) / sizeof(float)
};