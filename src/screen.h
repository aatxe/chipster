#ifndef __screen_h__
#define __screen_h__

#include <stdint.h>
#include "src/minuet.h"

const uint16_t scale = 0x4; // Display Scale
const uint32_t tick_length = 1000 / 60; // milliseconds per tick
const int COLOR_LOW[4] = {0, 0, 0, 255};
const int COLOR_HIGH[4] = {255, 255, 255, 255};

typedef enum { SCREEN_LOW = 1, SCREEN_HIGH } screen_type; 
typedef enum { KEY_UP, KEY_DOWN } key_state;

int init();
void setup(screen_type type);
void update_keys();
void update(uint8_t fb[]);
key_state get_key(uint8_t k);
void sync();

#endif