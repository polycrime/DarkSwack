#include <kos.h>

int main(int argc, char **argv) {
	vid_set_mode(DM_640x480, PM_RGB565);

	vram_s[200 + (200 * 640)] = 0xFF; // draw a blue pixel

	return 0;
}