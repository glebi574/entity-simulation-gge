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
  for (int i = 0; i < 1000; ++i)
    scene.new_square(irand(-500, 500), irand(-500, 500), 32, 0xffa000ff);
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