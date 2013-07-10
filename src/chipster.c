#include <SDL/SDL.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "interpreter.h"
#include "screen.h"

void chipster_st() {
	for (;;) {
		if (is_awaiting_keystroke()) {
			update_keys();
			uint8_t i;
			for (i = 0; i < 0x10; i++) if (get_key(i)) send_key(i);
		} else {
			if (!get_dt())
				interpret();
			update_timers();
			update(framebuf);
			sync();
		}
	}
}

int interpreter_thread() {
	for (;;) {
		if (is_awaiting_keystroke()) {
			update_keys();
			uint8_t i;
			for (i = 0; i < 0x10; i++) if (get_key(i)) send_key(i);
		} else if (!get_dt()) {
			interpret();
		}
	}
	return 0;
}

int main_thread() {
	for (;;) { 
		update_timers();
		update(framebuf);
		sync();
	}
	return 0;
}

void chipster_mt() {
	SDL_Thread *main, *interpreter;
	main = SDL_CreateThread(main_thread, NULL);
	check(main, "Failed to create main thread: %s\n", SDL_GetError());
	interpreter = SDL_CreateThread(interpreter_thread, NULL);
	check(interpreter, "Failed to create interpreter thread: %s\n", SDL_GetError());
	SDL_WaitThread(main, NULL);
	SDL_WaitThread(interpreter, NULL);
error:
	exit(1);
}

int main(int argc, char *argv[]) {
	int rom_index = 1, mt = 0;
	if (!strcmp(argv[1], "--test")) return 0; // just a simple test to see if installed.
	if (!strcmp(argv[1], "-mt")) {
		rom_index++;
		mt = 1;
		debug("chipster is running in multithreaded mode.");
	}
	check_user(argc >= rom_index + 1, "%s: you must supply a ROM argument!\n", argv[0]);
	check_user(access(argv[rom_index], R_OK) != -1, "%s: could not find ROM: %s\n", argv[0], argv[rom_index]);
	if (!init() || !load(argv[rom_index])) goto error;
	if (mt) chipster_mt();
	else chipster_st();
	return 0;
error:
	return 1;
}