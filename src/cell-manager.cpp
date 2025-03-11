#include "entity.h"

constexpr uint16_t get_key(int8_t x, int8_t y) {
  return (static_cast<uint8_t>(x) << 8) | (static_cast<uint8_t>(y));
}

CellManager::CellManager() {

}

ECell& CellManager::operator()(int8_t x, int8_t y) {
  return cells[get_key(x, y)];
}

const ECell& CellManager::operator()(int8_t x, int8_t y) const {
  return cells.at(get_key(x, y));
}