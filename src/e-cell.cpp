#include "entity.h"

ECell::ECell() {

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
    (regeneration / max.regeneration + health / max.health + armor / max.armor) / 3 * 192 + 63,
    (speed / max.speed + rotation_speed / max.rotation_speed) / 2 * 192 + 63,
    damage / max.damage * 192 + 63,
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

void ECell::calculate_energy() {
  for (int i = 0; i < 6; ++i) {
    double e = ((*this)[i] - min[i]) / range[i];
    energy_usage += e * e;
  }
}

uint8_t CellData::offsets[] = {
  offsetof(CellData, regeneration) / sizeof(float), offsetof(CellData, health) / sizeof(float),
  offsetof(CellData, armor) / sizeof(float), offsetof(CellData, damage) / sizeof(float),
  offsetof(CellData, speed) / sizeof(float), offsetof(CellData, rotation_speed) / sizeof(float)
};