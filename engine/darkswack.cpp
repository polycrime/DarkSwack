#include <kos.h>

int map[10][10] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

class DarkSwack {
	public:
		DarkSwack() {
			playerPosX = playerPosY = 5;
		}

		void init() {
			vid_set_mode(DM_640x480, PM_RGB565);
		}

		bool run() {
			return check_input() && render();
		}

	private:
		bool check_input() {
			maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
			cont_state_t *state = (cont_state_t *)maple_dev_status(cont);

			if(state != NULL && state->buttons & CONT_START)
					return false;

			return true;
		}

		bool render() {
			for(int i = 0; i < resX; i++) {
				for(int j = 0; j < resY; j++) {
					draw_pixel(i, j, 0xCC);
				}
			}
			return true;
		}

		void draw_pixel(int x, int y, int color) {
			vram_s[x + (y * resX)] = color;
		}

		static const int resX = 640;
		static const int resY = 480;

		double playerPosX;
		double playerPosY;
};

int main(int argc, char **argv) {
	DarkSwack *ds = new DarkSwack();
	ds->init();

	while(ds->run()) { }

	arch_reboot();
	return 0;
}