#define CNFGOGL // to not use disgusting legacy APIs of X11
#define CNFG_IMPLEMENTATION
#define RDUI_IMPLEMENTATION
#include "rawgd.h"
#include <rdui/default-elements.h>
#include <rawdraw/os_generic.h>

#include <math.h>

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

static const level_count = 1;
static struct level levels[] = {
  {
    .name = "test",
    .difficulty = 1,
    .objects_count = 2,
    .objects = test_objects
  }
};

static struct level* playing_level = NULL;
static time_t when_started_playing = 0;
static size_t selected_level_index = 0;

static const short player_x = 400;
static short player_y = 100;

void jump() {
  player_y -= 100;
}

void HandleKey(int keycode, int bDown) {
	if(!playing_level) { // in menu
    RDUIHandleKeyImpl(keycode, bDown);
    
    if(keycode == CNFG_KEY_RIGHT_ARROW && selected_level_index < level_count - 1) selected_level_index++;
    if(keycode == CNFG_KEY_LEFT_ARROW && selected_level_index > 0) selected_level_index--;
  } else {
    if(bDown && keycode == ' ') jump();
  }
}

void HandleButton(int x, int y, int button, int bDown) {
	if(!playing_level) RDUIHandleButtonImpl(x, y, button, bDown);
}

void HandleMotion(int x, int y, int mask) {
	if(!playing_level) RDUIHandleMotionImpl(x, y, mask);
}

void HandleDestroy() {
  exit(0);
}

long get_time() {
  return round(OGGetAbsoluteTime() * 10000);
}

void play(struct RDUIButtonData* button) {
  playing_level = &levels[selected_level_index];
  when_started_playing = get_time();
}

void fail() {
  playing_level = NULL;
  when_started_playing = 0;
}

long last_time;
long get_position_offset() {
  return (get_time(0) - when_started_playing) / 50;
}

#define SQUARE_COLOR 0x40c040
#define SPIKE_COLOR 0x4040c0

#define SHAPE_SIZE 50

void render_level() {
  long render_offset = get_position_offset();
  
  for(int i = 0; i < playing_level->objects_count; i++) {
    struct object* obj = &playing_level->objects[i];
    
    RDPoint view_pos = {
      .x = obj->position.x - render_offset,
      .y = obj->position.y
    };
  
    switch(obj->type) {
      case square: {
        CNFGColor(SQUARE_COLOR);
        CNFGTackRectangle(view_pos.x, view_pos.y, view_pos.x + SHAPE_SIZE, view_pos.y + SHAPE_SIZE);
        break;
      }
      case spike: {
        CNFGColor(SPIKE_COLOR);
        RDPoint triangle[] = {
          { .x = view_pos.x, .y = view_pos.y + SHAPE_SIZE },
          { .x = view_pos.x + SHAPE_SIZE, .y = view_pos.y + SHAPE_SIZE },
          { .x = view_pos.x + SHAPE_SIZE / 2, .y = view_pos.y },
        };
        
        CNFGTackPoly(triangle, 3);
        break;
      }
    }
  }
}

int is_on_ground(short y) {
  if(y > 100) return 1;
  for(int i = 0; i < playing_level->objects_count; i++) {
    RDPoint pos = playing_level->objects[i].position;
    if(player_x < pos.x || player_x > pos.x + SHAPE_SIZE) continue;
    if(y < pos.y + SHAPE_SIZE) return 1;
  }
                     
  return 0;
}

int is_on_spike() {
  for(int i = 0; i < playing_level->objects_count; i++) {
    RDPoint pos = playing_level->objects[i].position;
    if(player_x < pos.x || player_x > pos.x + SHAPE_SIZE) continue;
    if(playing_level->objects[i].type != spike) continue;
    
    if(player_y < pos.y + SHAPE_SIZE) return 1;
  }
  
  return 0;
}

void fall() {
  short distance = (get_time() - last_time) / 30;
  if(!is_on_ground(player_y)) {
    player_y += distance % 100;
  }
}

void render_player() {
  CNFGColor(0xff8888);
  CNFGTackRectangle(player_x, player_y, player_x + SHAPE_SIZE, player_y + SHAPE_SIZE);
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
  
  last_time = get_time();
  while(1) {
    CNFGClearFrame();
    CNFGHandleInput();

    if(playing_level) {
      if(is_on_spike()) fail();
      render_level();
      fall();
      render_player();
    } else {
      CNFGColor(0xffffff);
      CNFGPenX = 300;
      CNFGPenY = 50;
      CNFGDrawText("Level:", 7);
      CNFGPenX = 300;
      CNFGPenY = 100;
      CNFGDrawText(levels[selected_level_index].name, 7);
      
      RDUIDispatchEvent(RDUIEvent_render, NULL);
    }
    last_time = get_time();

    CNFGSwapBuffers();
  }
}
