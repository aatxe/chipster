#include <SDL/SDL.h>
#include <stdlib.h>
#include "screen.h"

SDL_Surface* screen;
screen_type m_type;
key_state keys[0x10];
uint32_t last_updated;

const uint16_t scale = 0x4; // Display Scale
const uint32_t tick_length = 1000 / 60; // milliseconds per tick

int init() {
    return SDL_Init(SDL_INIT_VIDEO);
}

void setup(screen_type type) {
	screen = SDL_SetVideoMode(64 * scale * type, 32 * scale * type, 8, SDL_SWSURFACE);
	SDL_Color Colors[2] = { {0, 0, 0, 255}, {255, 255, 255, 255} };
	SDL_SetColors(screen, Colors, 0, 2);
	m_type = type;
}

void update_keys() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		int key_value = 0;
		switch (e.type) {
		case SDL_QUIT:
			exit(0);
			break;
		case SDL_KEYDOWN:
			key_value = 1;
		case SDL_KEYUP:
			switch (e.key.keysym.sym) {
			case SDLK_1:
				keys[0x0] = key_value;
				break;
			case SDLK_2:
				keys[0x1] = key_value;
				break;
			case SDLK_3:
				keys[0x2] = key_value;
				break;
			case SDLK_4:
				keys[0x3] = key_value;
				break;
			case SDLK_q:
				keys[0x4] = key_value;
				break;
			case SDLK_w:
				keys[0x5] = key_value;
				break;
			case SDLK_e:
				keys[0x6] = key_value;
				break;
			case SDLK_r:
				keys[0x7] = key_value;
				break;
			case SDLK_a:
				keys[0x8] = key_value;
				break;
			case SDLK_s:
				keys[0x9] = key_value;
				break;
			case SDLK_d:
				keys[0xA] = key_value;
				break;
			case SDLK_f:
				keys[0xB] = key_value;
				break;
			case SDLK_z:
				keys[0xC] = key_value;
				break;
			case SDLK_x:
				keys[0xD] = key_value;
				break;
			case SDLK_c:
				keys[0xE] = key_value;
				break;
			case SDLK_v:
				keys[0xF] = key_value;
				break;
			default:
				break;
			}
		}
	}
}

key_state get_key(uint8_t k) {
	check(k < 0x10, "Invalid key: %x\n", k);
	return keys[k];
error:
	return KEY_UP;
}

void update_screen(uint8_t *fb) {
	SDL_Rect b = {0, 0, scale, scale};
	for (uint16_t y = 0; y < 32 * m_type; ++y) {
		b.y = y * scale;
		for (uint16_t x = 0; x < 64 * m_type; ++x) {
			b.x = x * scale;
			SDL_FillRect(screen, &b, fb[y * (64 * m_type) + x]);
		}
	}
	SDL_Flip(screen);
}

void update(uint8_t *fb) {
	update_keys();
	update_screen(fb);
}

void sync() {
	uint32_t diff = SDL_GetTicks() - last_updated;
	if (diff < tick_length)
		SDL_Delay(tick_length - diff);
	last_updated = SDL_GetTicks();
}