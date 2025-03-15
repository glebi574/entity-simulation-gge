#include "entity.h"

constexpr uint16_t CellManager::to_uid(int8_t x, int8_t y) const {
  return (static_cast<uint8_t>(x) << 8) | (static_cast<uint8_t>(y));
}

CellManager::CellManager() {
  for (int i = 0; i < 225; ++i) {
    add(i / 15 - 7, i % 15 - 7);
  }
}

void CellManager::add(int8_t x, int8_t y) {
  (*this)(x, y) = ECell(x, y);
}

ECell& CellManager::operator()(int8_t x, int8_t y) {
  return cells[to_uid(x, y)];
}

const ECell& CellManager::operator()(int8_t x, int8_t y) const {
  return cells.at(to_uid(x, y));
}