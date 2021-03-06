#include <stdio.h>
#include <math.h>

#define CNFGOGL // to not use disgusting legacy APIs of X11
#define CNFG_IMPLEMENTATION
#define RDUI_IMPLEMENTATION
#include "rawgd.h"
#include <rdui/default-elements.h>
#include <rawdraw/os_generic.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

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

int level_count = 1;
struct level levels[] = {
  {
    .name = "test",
    .difficulty = 1,
    .ground_y = 100,
    .objects_count = 2,
    .objects = test_objects
  }
};

struct level* playing_level = NULL;
time_t when_started_playing = 0;
size_t selected_level_index = 0;
const short player_x = WINDOW_WIDTH / 2;
short player_y = 0;

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

int does_intersect(RDPoint amin, RDPoint amax, RDPoint bmin, RDPoint bmax) {
  int
    a = amin.x < bmax.x,
    b = amax.x > bmin.x,
    c = amin.y <= bmax.y,
    d = amax.y > bmin.y;

  return a && b && c && d;
}

int does_intersect_size(RDPoint amin, short asize[2], RDPoint bmin, short bsize[2]) {
  RDPoint amax = {
    .x = amin.x + asize[0],
    .y = amin.y + asize[1]
  };

  RDPoint bmax = {
    .x = bmin.x + bsize[0],
    .y = bmin.y + bsize[1]
  };

  return does_intersect(amin, amax, bmin, bmax);
}

long last_time;
long get_position_offset() {
  return (get_time(0) - when_started_playing) / 50 - 600;
}

#define SQUARE_COLOR 0x40c040ff
#define SPIKE_COLOR 0x4040c0ff

static short SHAPE_SIZE[2] = {50, 50};

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
        CNFGTackRectangle(view_pos.x, view_pos.y, view_pos.x + SHAPE_SIZE[0], view_pos.y + SHAPE_SIZE[1]);
        break;
      }
      case spike: {
        CNFGColor(SPIKE_COLOR);
        RDPoint triangle[] = {
          { .x = view_pos.x, .y = view_pos.y + SHAPE_SIZE[1] },
          { .x = view_pos.x + SHAPE_SIZE[0], .y = view_pos.y + SHAPE_SIZE[1] },
          { .x = view_pos.x + SHAPE_SIZE[0] / 2, .y = view_pos.y },
        };
        
        CNFGTackPoly(triangle, 3);
        break;
      }
    }
  }
}

struct object* get_ground(short y) {
  long pos_offset = get_position_offset();

  RDPoint player_pos = {.x = player_x, .y = y};
  for(int i = 0; i < playing_level->objects_count; i++) {
    RDPoint pos = playing_level->objects[i].position;
    RDPoint pos_moved = {.x = pos.x - pos_offset, y = pos.y};
    if(does_intersect_size(pos_moved, SHAPE_SIZE, player_pos, SHAPE_SIZE)) return &playing_level->objects[i];
  }

  return NULL;
}

void fall() {
  short new_y = player_y + (get_time() - last_time) / 30;
  if(new_y > playing_level->ground_y) {
    player_y = playing_level->ground_y;
  } else {
    struct object* ground = get_ground(new_y);
    struct object* inside_of = get_ground(new_y - SHAPE_SIZE[1]);
    if(inside_of) {
      player_y = inside_of->position.y - SHAPE_SIZE[1];
    } else if(ground) {
      player_y = ground->position.y - SHAPE_SIZE[1];
    } else player_y = new_y;
  }
}

void render_player() {
  CNFGColor(0xff8888ff);
  CNFGTackRectangle(player_x, player_y, player_x + SHAPE_SIZE[0], player_y + SHAPE_SIZE[1]);
}

void handle_play_button(struct RDUIButtonData* data) {
  play();
}

int main() {
  RDUIInit();
  CNFGSetup("Raw Geometry Dash", 800, 600);
  
  struct RDUIButtonData play_button = {
    .text = "Play",
    .font_size = 6,
    .padding = 5,
    .color = 0x4444AAFF,
    .font_color = 0x000000FF,
    .position = { .x = 300, .y = 300},
    .clicked_handler = handle_play_button
  };
  
  RDUIPushNode(RDUINewButton(&play_button));
  
  last_time = get_time();
  while(1) {
    CNFGClearFrame();
    CNFGHandleInput();

    switch(current_state) {
      case in_menu: {
        CNFGColor(0xffffffff);
        CNFGPenX = 300;
        CNFGPenY = 50;
        CNFGDrawText("Level:", 7);
        CNFGPenX = 300;
        CNFGPenY = 100;
        CNFGDrawText(levels[selected_level_index].name, 7);

        RDUIDispatchEvent(RDUIEvent_render, NULL);

        break;
      }

      case playing: {
        render_level();
        render_player();

        struct object* ground_obj = get_ground(player_y);
        struct object* inside_of = get_ground(player_y - SHAPE_SIZE[1] / 2);
        if(inside_of || (ground_obj && ground_obj->type == spike)) fail();

        fall();

        break;
      }
    }

    last_time = get_time();

    CNFGSwapBuffers();
  }
}
