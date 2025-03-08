#include "draw-utils.h"

static bool limit_framerate = true;
static int frame_counter = 0;
// 112 = 64; Tested on different systems, with different precision of time and clocks and by different software. Why?
static int frame_limit = 112;
static std::chrono::milliseconds frame_duration(1000 / frame_limit);
static std::chrono::steady_clock::time_point last_second_time = std::chrono::steady_clock::now();
static std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();

void frame_time_update() {
  std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();
  if (std::chrono::duration_cast<std::chrono::seconds>(current_time - last_second_time).count() >= 1) {
    std::cout << "Frames: " << frame_counter << '\n';
    frame_counter = 0;
    last_second_time = current_time;
  }
  else
    ++frame_counter;
}

void frame_limiter() {
  if (!limit_framerate)
    return;
  std::chrono::milliseconds frame_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_time);
  if (frame_time < frame_duration)
    std::this_thread::sleep_for(frame_duration - frame_time);
  last_time = std::chrono::steady_clock::now();
}

void MeshConstructor::set_base_meshes() {
  float multiplier_triangle = TAU / 3, multiplier_square = TAU / 4, offset_square = TAU / 8;
  for (int i = 0; i < 3; ++i) {
    float angle = multiplier_triangle * i;
    base_triangle.emplace_back(std::vector<float>{ cos(angle), sin(angle) });
  }
  for (int i = 0; i < 4; ++i) {
    float angle = multiplier_square * i + offset_square;
    base_square.emplace_back(std::vector<float>{ cos(angle), sin(angle) });
  }
}

MeshConstructor::MeshConstructor() {
  if (base_triangle.empty())
    set_base_meshes();
}

// {{vx, vy}} -> {vx * scale + x, vy * scale + y, 0}
void insert_vertex(std::vector<float>& to, const std::vector<float>& from, float x, float y, float scale) {
  to.emplace_back(from[0] * scale + x);
  to.emplace_back(from[1] * scale + y);
  to.emplace_back(0);
}

// {x1, y1, ..., xn, yn} -> {x1, y1, 0, x2, y2, 0, x3, y3, 0}
void insert_vertexD(std::vector<float>& to, const std::vector<float>& from, int offset = 0) {
  for (int i = offset; i < offset + 5; i += 2) {
    to.emplace_back(from[i % from.size()]);
    to.emplace_back(from[(i + 1) % from.size()]);
    to.emplace_back(0);
  }
}

// {x1, y1, x2, y2} -> {x1, y1, 0, ..., x2, y2, 0} ; transforms into rectangle with points 1 and 2 being edges
void insert_vertex24(std::vector<float>& to, const std::vector<float>& from, int side) {
  to.emplace_back(from[(side == 1 | side == 2) * 2]);
  to.emplace_back(from[(side > 1) * 2 + 1]);
  to.emplace_back(0);
}

float triangle_scale = sin(PI / 3), square_scale = sin(PI / 4);

void MeshConstructor::add_triangle(float x, float y, float scale, uint32_t color) {
  scale *= triangle_scale;
  for (const std::vector<float>& p : base_triangle)
    insert_vertex(pos, p, x, y, scale);
  colors.insert(colors.end(), 3, color);
}

void MeshConstructor::add_triangle(const std::vector<float>& p, uint32_t color) {
  insert_vertexD(pos, p);
  colors.insert(colors.end(), 3, color);
}

void MeshConstructor::add_square(float x, float y, float scale, uint32_t color) {
  scale *= square_scale;
  for (int i = 0; i < 3; ++i) {
    const std::vector<float>& p = base_square[i];
    insert_vertex(pos, p, x, y, scale);
  }
  for (int i = 2; i < 5; ++i) {
    const std::vector<float>& p = base_square[i % 4];
    insert_vertex(pos, p, x, y, scale);
  }
  colors.insert(colors.end(), 6, color);
}

void MeshConstructor::add_rectangle(const std::vector<float>& p, uint32_t color) {
  insert_vertex24(pos, p, 0);
  insert_vertex24(pos, p, 1);
  insert_vertex24(pos, p, 2);
  insert_vertex24(pos, p, 2);
  insert_vertex24(pos, p, 3);
  insert_vertex24(pos, p, 0);
  colors.insert(colors.end(), 6, color);
}

void MeshConstructor::add_poly4(const std::vector<float>& p, uint32_t color) {
  insert_vertexD(pos, p);
  insert_vertexD(pos, p, 4);
  colors.insert(colors.end(), 6, color);
}

std::vector<std::vector<float>> MeshConstructor::base_triangle, MeshConstructor::base_square;