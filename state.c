#include "rawgd.h"

enum game_state current_state = in_menu;

extern struct level* playing_level;
extern time_t when_started_playing;
extern size_t selected_level_index;
extern short player_y;

void play() {
  playing_level = &levels[selected_level_index];
  when_started_playing = get_time();
  player_y = playing_level->ground_y;

  current_state = playing;
}


void fail() {
  playing_level = NULL;
  when_started_playing = 0;
  player_y = 0;

  current_state = failed;
}

void go_to_menu() {
  current_state = in_menu;
}
