#define CNFGOGL // to not use disgusting legacy APIs of X11
#define CNFG_IMPLEMENTATION
#define RDUI_IMPLEMENTATION
#include "rawgd.h"
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

int main() {
  RDUIInit();
  CNFGSetup("Raw Geometry Dash", 800, 600);
  
  while(1) {
    CNFGClearFrame();
    CNFGHandleInput();
    
    RDUIDispatchEvent(RDUIEvent_render, NULL);
    
    CNFGColor(0xff3333);
    CNFGTackRectangle(10, 10, 20, 20);
    
    CNFGSwapBuffers();
  }
}
