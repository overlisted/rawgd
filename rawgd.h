#pragma once

#include <rawdraw/CNFG.h>
#include <rdui/RDUI.h>

enum object_type {
  square,
  spike
};

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
