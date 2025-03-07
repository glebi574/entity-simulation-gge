#include "draw-utils.h"

static bool limit_framerate = true;
static int frame_counter = 0;
// 112 = 64; Tested on different systems, with different precision of time and clocks and by different software. Why?
static int frame_limit = 112;
static std::chrono::milliseconds frame_duration(1000 / frame_limit);
static std::chrono::steady_clock::time_point last_second_time = std::chrono::steady_clock::now();
static std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();

SceneManager scene;

void window_pre_init(GWindow* gw) {
  scene = SceneManager(gw);
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

  VObject* vo = gw->add_tvo(positions, colors);

  scene.add(vo);

  gw->add_text("String or something, idk", 0, 0, 1, 0xffffffff);
}

void window_update(GWindow* gw, void (*update_function)(GWindow*)) {
  std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();
  if (std::chrono::duration_cast<std::chrono::seconds>(current_time - last_second_time).count() >= 1) {
    std::cout << "Frames: " << frame_counter << '\n';
    frame_counter = 0;
    last_second_time = current_time;
  }
  else
    ++frame_counter;
  
  update_function(gw);

  if (!limit_framerate)
    return;
  std::chrono::milliseconds frame_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_time);
  if (frame_time < frame_duration)
    std::this_thread::sleep_for(frame_duration - frame_time);
  last_time = std::chrono::steady_clock::now();
}