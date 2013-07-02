#include <stdint.h>
#include <unistd.h>
#include "interpreter.h"
#include "screen.h"

int main(int argc, char *argv[]) {
	check_user(argc >= 2, "%s: you must supply a ROM argument!", argv[0]);
	check_user(access(argv[1], R_OK) != -1, "%s: could not find ROM: %s", argv[0], argv[1]);
	if (!init() || !load(argv[1])) goto error;
	while (1) {
		if (is_awaiting_keystroke) {
			update_keys();
			for (uint8_t i = 0; i < 0x10; i++) if (get_key(i)) send_key(i);
		} else {
			interpret();
			update(framebuf);
			sync();
		}
	}
error:
	return 1;
}