#ifndef __interpreter_h__
#define __interpreter_h__

#include <stdint.h>
#include "minuet/minuet.h"

// Compatability for non-clang compilers
#ifndef __has_feature
	#define __has_feature(x) 0
#endif

// Base Address for Program Space
#define BASE_ADDR 0x200
							
extern uint8_t *framebuf;
							 
typedef struct {
	uint8_t interpreter[BASE_ADDR]; // Interpreter Space
	uint8_t rom[0xFFF - BASE_ADDR]; // Program Space
} Memory;

typedef uint8_t await_key_register;

int load(char *path);
void interpret();
int is_awaiting_keystroke();
void send_key(uint8_t key);

#endif
