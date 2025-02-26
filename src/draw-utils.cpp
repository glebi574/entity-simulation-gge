#include "draw-utils.h"

int object_amount = 100;
static std::vector<MObject> scene_things;

MObject::MObject(VObject* vo) {
  this->vo = vo;
}

void MObject::update_vo() {
  vo->tm.set_angle(angle[0], angle[1], angle[2]);
  vo->tm.set_scale(scale[0], scale[1], scale[2]);
  vo->tm.set_offset(pos[0], pos[1], pos[2]);
}

static bool limit_framerate = true;
static int frame_counter = 0, frame_limit = 112;
static std::chrono::milliseconds frame_duration(1000 / frame_limit);
static std::chrono::steady_clock::time_point last_second_time = std::chrono::steady_clock::now();
static std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();

void window_pre_init(GWindow* gw) {
  
}

void window_post_init(GWindow* gw) {
  // test - defining vertexes
  Scene* sp = gw->add_scene("triangle_vs.glsl", "triangle_fs.glsl");
  for (int i = 0; i < object_amount; ++i) {
    std::vector<float> positions;
    float s = 8;
    float h = s / 2 * sqrt(3);
    positions.emplace_back(h / 2);
    positions.emplace_back(0);
    positions.emplace_back(0);

    positions.emplace_back(-h / 2);
    positions.emplace_back(-s / 2);
    positions.emplace_back(0);

    positions.emplace_back(-h / 2);
    positions.emplace_back(s / 2);
    positions.emplace_back(0);

    std::vector<uint32_t> colors;
    uint32_t color = 0xffa000ff;
    for (int i = 0; i < 3; ++i)
      colors.emplace_back(color);
    scene_things.emplace_back(MObject(sp->add_vobject(positions, colors)));
    MObject* mo = &scene_things.back();
    mo->pos[0] = irand(-300, 300);
    mo->pos[1] = irand(-300, 300);

    mo->dx = frand(-1, 1);
    mo->dy = frand(-1, 1);

    mo->update_vo();
  }
}

void additional_draw_function(GWindow* gw) {
  std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();
  if (std::chrono::duration_cast<std::chrono::seconds>(current_time - last_second_time).count() >= 1) {
    std::cout << "Frames: " << frame_counter << '\n';
    frame_counter = 0;
    last_second_time = current_time;
  }
  else
    ++frame_counter;

  for (MObject& mo : scene_things) {
    float force = sqrt(mo.pos[0] * mo.pos[0] + mo.pos[1] * mo.pos[1]) / 1000000;
    float angle = atan2(-mo.pos[1], -mo.pos[0]);
    mo.dx += cos(angle) * force + frand(-0.1, 0.1);
    mo.dy += sin(angle) * force + frand(-0.1, 0.1);
    mo.pos[0] += mo.dx;
    mo.pos[1] += mo.dy;
    if (mo.pos[0] < -1000 || mo.pos[0] > 1000)
      mo.dx *= -1;
    if (mo.pos[1] < -1000 || mo.pos[1] > 1000)
      mo.dy *= -1;
    mo.angle[2] = atan2(-mo.dy, mo.dx);

    mo.update_vo();
  }

  if (!limit_framerate)
    return;
  std::chrono::milliseconds frame_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_time);
  if (frame_time < frame_duration)
    std::this_thread::sleep_for(frame_duration - frame_time);
  last_time = std::chrono::steady_clock::now();
}