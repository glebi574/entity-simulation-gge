#include "entity.h"

constexpr uint16_t CellManager::to_uid(int8_t x, int8_t y) const {
  return (static_cast<uint8_t>(x) << 8) | (static_cast<uint8_t>(y));
}

void CellManager::from_uid(uint16_t uid, int8_t& x, int8_t& y) {
  x = static_cast<int8_t>(uid >> 8);
  y = static_cast<int8_t>(uid & 0xff);
}

CellManager::CellManager() {
  
}

ECell* CellManager::add(int8_t x, int8_t y) {
  (*this)(x, y) = ECell();
  return &(*this)(x, y);
}

// Offset for the cell pattern
float tdx = cell_size * sqrt(3) / 2, tdy = cell_size / 2;

void CellManager::create_vo() {
  MeshConstructor mc;
  for (auto& [uid, cell] : cells) {
    int8_t x, y;
    from_uid(uid, x, y);
    bool flip = (x + y) % 2;
    cell.x = tdx * x + (flip ? 0 : -tdx / 3);
    cell.y = tdy * y;
    mc.add_triangle(cell.x, cell.y, flip ? -cell_size : cell_size, (*this)(x, y).stats_to_color());
  }
  vo = sm->add(mc);
}

bool CellManager::contains(int8_t x, int8_t y) {
  return cells.find(to_uid(x, y)) == cells.end();
}

ECell& CellManager::operator()(int8_t x, int8_t y) {
  return cells[to_uid(x, y)];
}

const ECell& CellManager::operator()(int8_t x, int8_t y) const {
  return cells.at(to_uid(x, y));
}

SceneManager* CellManager::sm;