#pragma once

#include <rawdraw/CNFG.h>
#include <rdui/RDUI.h>

extern struct level* playing_level;
extern time_t when_started_playing;
extern size_t selected_level_index;
extern short player_y;

enum game_state {
  in_menu,
  playing,
  failed
};

extern enum game_state current_state;

void play();
void fail();
void go_to_menu();

enum object_type {
  square,
  spike
};

long get_time();

struct object {
  enum object_type type;
  RDPoint position;
};

struct level {
  const char* name;
  float difficulty;
  short ground_y;
  
  size_t objects_count;
  struct object* objects;
};

extern int level_count;
extern struct level levels[];
