#ifndef __interpreter_h__
#define __interpreter_h__

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

// Compatability for non-Clang compilers.
#ifndef __has_feature
  #define __has_feature(x) 0
#endif

const uint16_t base_addr = 0x200;

typedef struct {
	uint8_t[base_addr] interpreter; // Interpreter Space
	uint8_t[0xFFF - base_addr] rom; // Program Space
} Memory;

#endif