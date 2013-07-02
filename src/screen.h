#ifndef __screen_h__
#define __screen_h__

#include <stdint.h>
#include "minuet/minuet.h"

typedef enum { SCREEN_LOW = 1, SCREEN_HIGH } screen_type; 
typedef enum { KEY_UP, KEY_DOWN } key_state;

typedef struct {
	uint8_t *buf; // Frame Buffer
	uint16_t length;
} FrameBuf;

int init();
void setup(screen_type type);
void update_keys();
void update(FrameBuf *fb);
key_state get_key(uint8_t k);
void sync();

#endif
