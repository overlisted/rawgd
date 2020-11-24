#define CNFGOGL // to not use disgusting legacy APIs of X11
#define CNFG_IMPLEMENTATION
#define RDUI_IMPLEMENTATION
#include "rawgd.h"
#include <rdui/default-elements.h>
#include <rawdraw/os_generic.h>

#include <math.h>

void HandleKey(int keycode, int bDown) {
	RDUIHandleKeyImpl(keycode, bDown);
}

void HandleButton(int x, int y, int button, int bDown) {
	RDUIHandleButtonImpl(x, y, button, bDown);
}

void HandleMotion(int x, int y, int mask) {
	RDUIHandleMotionImpl(x, y, mask);
}

void HandleDestroy() {
  exit(0);
}

static struct object test_objects[] = {
  {
    .type = square,
    .position = { .x = 100, .y = 100 }
  },
  {
    .type = spike,
    .position = { .x = 160, .y = 100 }
  }
};

static struct level levels[] = {
  {
    .name = "test",
    .difficulty = 1,
    .objects_count = 2,
    .objects = test_objects
  }
};

long get_time() {
  return round(OGGetAbsoluteTime() * 10000);
}

void play(struct RDUIButtonData* button) {
  playing_level = &levels[0];
  when_started_playing = get_time();
}

void fail() {
  playing_level = NULL;
  when_started_playing = 0;
}

int main() {
  RDUIInit();
  CNFGSetup("Raw Geometry Dash", 800, 600);
  
  struct RDUIButtonData play_button = {
    .text = "Play",
    .font_size = 6,
    .padding = 5,
    .color = 0x4444AA,
    .font_color = 0x00,
    .position = { .x = 300, .y = 300},
    .clicked_handler = play
  };
  
  RDUIPushNode(RDUINewButton(&play_button));
  
  while(1) {
    CNFGClearFrame();
    CNFGHandleInput();
    CNFGSwapBuffers();
  }
}
