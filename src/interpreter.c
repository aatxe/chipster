#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "interpreter.h"
#include "screen.h"

uint16_t *pc; // Program Counter
Memory mem; // System memory
uint8_t si; // Stack Index
uint16_t *stack[0x10]; // Stack
uint8_t *I; // Memory-Access Register 
uint8_t V[0x10]; // Registers
uint8_t dt, st; // Timers (Delay and Sound)
uint8_t *framebuf; // Frame Buffer
screen_type m_type; // Screen Type
await_key_register await_reg; // Awaiting Key Register, 0x10 if none.

#if __has_feature(c_static_assert)
	_Static_assert(sizeof(mem) == 0xFFF)
#endif

// System Font Sprites
const uint8_t font[0x50] = { 0xF0, 0x90, 0x90, 0x90, 0xF0, 0x20, 0x60, 0x20, 0x20, 0x70,
0xF0, 0x10, 0xF0, 0x80, 0xF0, 0xF0, 0x10, 0xF0, 0x10, 0xF0, 0x90, 0x90, 0xF0, 0x10, 0x10, 
0xF0, 0x80, 0xF0, 0x10, 0xF0, 0xF0, 0x80, 0xF0, 0x90, 0xF0, 0xF0, 0x10, 0x20, 0x40, 0x40,
0xF0, 0x90, 0xF0, 0x90, 0xF0, 0xF0, 0x90, 0xF0, 0x10, 0xF0, 0xF0, 0x90, 0xF0, 0x90, 0x90,
0xE0, 0x90, 0xE0, 0x90, 0xE0, 0xF0, 0x80, 0x80, 0x80, 0xF0, 0xE0, 0x90, 0x90, 0x90, 0xE0,
0xF0, 0x80, 0xF0, 0x80, 0xF0, 0xF0, 0x80, 0xF0, 0x80, 0x80 };

int load(char *path) {
	// Loads fonts into interpreter space.
	uint8_t* d = (&mem)->interpreter;
	for (; d < (&mem)->interpreter + 0x50; d++)
		*d = font[d - (&mem)->interpreter];
	
	// Loads specified ROM into program space.
	FILE* rom = fopen(path, "r");
	check(rom != NULL, "Failed to load ROM: %s\n", path);
	uint8_t* p = (&mem)->rom;
	while(fread(++p, sizeof(uint8_t), 1, rom));
	fclose(rom);
	
	// Allocates frame buffer for rendering.
	framebuf = malloc(sizeof(uint8_t) * 64 * 32);
	check_mem(framebuf);
	
	// Other initialization stuffs.
	m_type = SCREEN_LOW;
	setup(m_type);
	await_reg = 0x10;
	return 1;
error:
	fclose(rom);
	free(framebuf);
	return 0;
}

void regen_frame_buffer(screen_type type) {
	free(framebuf);
	framebuf = malloc(sizeof(uint8_t) * 64 * 32 * type * type);
	check_mem(framebuf);
error:
	return;
}

void interpret() {
	uint16_t instr = *pc;
	// nnn is the last 12-bits of the instruction
	uint16_t nnn = instr & 0xFFF;
	// kk is the last 8-bits of the instruction
	uint8_t kk = instr & 0xFF;
	// nX are individual nibbles (4-bit segments) of the instruction
	uint8_t n1 = instr >> 12;
	uint8_t n2 = (instr >> 8) & 0xF;
	uint8_t n3 = (instr >> 4) & 0xF;
	uint8_t n4 = instr & 0xF;
	// States used within certain instruction interpretations.
	uint16_t s;
	uint8_t* i = I;
	switch (n1) {
	case 0x0:
		switch (n2) {
		case 0x0:
			switch (n3) {
			case 0xE:
				switch (n4) {
				case 0x0:
					// CLS
					for (uint16_t i = 0; i < sizeof(framebuf) / sizeof(uint8_t); i++) framebuf[i] = 0;
					break;
					
				case 0xE:
					// RET
					pc = stack[si--];
					break;
				}
				
			case 0xF:
				switch (n4) {
				case 0xD:
					// EXIT
					exit(0);
					
				case 0xE:
					// LOW
					m_type = SCREEN_LOW;
					regen_frame_buffer(SCREEN_LOW);
					setup(SCREEN_LOW);
					break;
					
				case 0xF:
					// HIGH
					m_type = SCREEN_HIGH;
					regen_frame_buffer(SCREEN_HIGH);
					setup(SCREEN_HIGH);
					break;

				default:
					debug("Unknown instruction: %X\n", instr);	
				}
				
			default:
				debug("Unknown instruction: %X\n", instr);	
			}
			break;
			
			default:
				// SYS addr
				break;
		}
		break;
		
	case 0x1:
		// JP addr
		pc = (uint16_t*) ((uint8_t*) &mem + nnn);
		break;
		
	case 0x2:
		// CALL addr
		stack[si++] = pc;
		pc = (uint16_t*) ((uint8_t*) &mem + nnn);
		break;
		
	case 0x3:
		// SE Vx, byte
		if (V[n2] == kk) pc++;
		break;
		
	case 0x4:
		// SNE Vx, byte
		if (V[n2] != kk) pc++;
		break;
		
	case 0x5:
		// SE Vx, Vy
		if (V[n2] == V[n3]) pc++;
		break;
		
	case 0x6:
		// LD Vx, byte
		V[n2] = kk;
		break;
		
	case 0x7:
		// ADD Vx, byte
		V[n2] += kk;
		break;
		
	case 0x8:
		switch (n4) {
		case 0x0:
			// LD Vx, Vy
			V[n2] = V[n3];
			break;
			
		case 0x1:
			// OR Vx, Vy
			V[n2] |= V[n3];
			break;
			
		case 0x3:
			// XOR Vx, Vy
			V[n2] ^= V[n3]; 
			break;
			
		case 0x4:
			// ADD Vx, Vy
			V[0xF] = (V[n2] + V[n3] > 0xFFF) ? 0x1 : 0x0;
			V[n2] = (V[n2] + V[n3]) & 0xFFF;
			break;
			
		case 0x5:
			// SUB Vx, Vy
			V[0xF] = (V[n2] > V[n3]) ? 0x1 : 0x0;
			V[n2] -= V[n3];
			break;
			
		case 0x6:
			// SHR Vx {, Vy}
			V[0xF] = V[n2] & 0x1;
			V[n2] >>= 1;
			break;
			
		case 0x7:
			// SUBN Vx, Vy
			V[0xF] = (V[n3] > V[n2]) ? 0x1 : 0x0;
			V[n2] = V[n3] - V[n2];
			break;
			
		case 0xE:
			// SHL Vx {, Vy}
			V[0xF] = V[n2] >> 7;
			V[n2] <<= 1;
			break;
			
		default:
			debug("Unknown instruction: %X\n", instr);
		}
		break;
		
	case 0x9:
		// SNE Vx, Vy
		if (V[n2] != V[n3]) pc++;
		break;
		
	case 0xA:
		// LD I, addr
		I = (uint8_t*) &mem + nnn;
		break;
		
	case 0xB:
		// JP V0, addr
		pc = (uint16_t*) ((uint8_t*) &mem + nnn + V[0]);
		break;
		
	case 0xC:
		// RND Vx, byte
		V[n2] = (rand() % 0x100) & kk;
		break;
		
	case 0xD:
		// DRW Vx, Vy, nibble
		s = V[n3] * (64 * m_type) + V[n2];
		for (; i < I + n4; i++) {
			V[0xF] &= ((*i ^ framebuf[s + i - I]) == 0) ? 0x1 : 0x0;
			framebuf[s + i - I] ^= *i;
		}
		break;
		
	case 0xE:
		switch (kk) {
		case 0x9E:
			// SKP Vx
			if (get_key(V[n2])) pc++;
			break;
			
		case 0xA1:
			// SKNP Vx
			if (!get_key(V[n2])) pc++;
			break;
			
		default:
			debug("Unknown instruction: %X\n", instr);
		}
		break;
		
	case 0xF:
		switch (kk) {
		case 0x07:
			// LD Vx, DT
			V[n2] = dt;
			break;
			
		case 0x0A:
			// LD Vx, K
			await_reg = n2;
			break;
			
		case 0x15:
			// LD DT, Vx
			dt = V[n2];
			break;
			
		case 0x18:
			// LD ST, Vx
			st = V[n2];
			break;
			
		case 0x1E:
			// ADD I, Vx
			I += V[n2];
			break;
			
		case 0x29:
			// LD F, Vx
			I = (uint8_t*) &mem + V[n2];
			break;
			
		case 0x33:
			// LD B, Vx
			*I = V[n2] / 100;
			*(I + 1) = (V[n2] % 100) / 10;
			*(I + 2) = V[n2] % 10;
			break;
			
		case 0x55:
			// LD [I], Vx
			for (; i < I + n2; i++) *i = V[i - I];
			break;
			
		case 0x65:
			// LD [Vx], I
			for (; i < I + n2; i++) V[i - I] = *i;
			
		default:
			debug("Unknown instruction: %X\n", instr);
		}
		break;
		
	default:
		debug("Unknown instruction: %X\n", instr);
	}
	if (dt > 0) dt--;
	if (st > 0) st--;
	pc++;
}

int is_awaiting_keystroke() {
	return (await_reg < 0x10) ? 1 : 0;
}

void send_key(uint8_t key) {
	check_debug(await_reg < 0x10, "Received key whilst not awaiting a keypress.\n");
	V[await_reg] = key;
	await_reg = 0x10;
error:
	return;
}
