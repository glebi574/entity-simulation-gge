#include "draw-utils.h"

int object_amount = 100;
static std::vector<ManagedObject> scene_things;

ManagedObject::ManagedObject() { }

ManagedObject::ManagedObject(VObject* vo) {
  this->vo = vo;
}

void ManagedObject::update_vo() {
  vo->tm.set_angle(angle[0], angle[1], angle[2]);
  vo->tm.set_scale(scale[0], scale[1], scale[2]);
  vo->tm.set_offset(pos[0], pos[1], pos[2]);
}

void remove_mo(GWindow* gw, VObject* vo) {
  for (auto c = scene_things.begin(); c != scene_things.end(); ++c) {
    ManagedObject mo = *c;
    if (mo.vo == vo) {
      gw->remove_vo(vo);
      scene_things.erase(c);
      return;
    }
  }
}

static bool limit_framerate = true;
static int frame_counter = 0, frame_limit = 112;
static std::chrono::milliseconds frame_duration(1000 / frame_limit);
static std::chrono::steady_clock::time_point last_second_time = std::chrono::steady_clock::now();
static std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();

void window_pre_init(GWindow* gw) {
  
}

void window_post_init(GWindow* gw) {
  std::vector<float> positions;
  std::vector<uint32_t> colors;

  float s = 8;

  for (int i = 0; i < 3; ++i) {
    float a = TAU / 3 * i;
    positions.emplace_back(cos(a) * s);
    positions.emplace_back(sin(a) * s);
    positions.emplace_back(0);
  }

  for (int i = 0; i < 3; ++i)
    colors.emplace_back(0xffa000ff);

  scene_things.emplace_back(ManagedObject(gw->add_tvo(positions, colors)));

  gw->add_text("String or something, idk", 0, 0, 1, 0xffffffff);
}

void update(GWindow* gw) {
  for (ManagedObject& mo : scene_things) {
    
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
  
  update(gw);

  if (!limit_framerate)
    return;
  std::chrono::milliseconds frame_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_time);
  if (frame_time < frame_duration)
    std::this_thread::sleep_for(frame_duration - frame_time);
  last_time = std::chrono::steady_clock::now();
}