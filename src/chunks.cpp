#include "entity-handler.h"

SceneChunk::SceneChunk() {

}

SceneChunk::SceneChunk(int x, int y) {
  x1 = x;
  y1 = y;
  x2 = x + width;
  y2 = y + height;
}