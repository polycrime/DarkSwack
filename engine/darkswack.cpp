#include <kos.h>
#include <math.h>
#include <dcplib/fnt.h>

// load external data
extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);

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
			playerRot = 0;
			playerPosX = playerPosY = 5 * gridUnit;
			time = oldTime = 0;
		}

		void init() {
			vid_set_mode(DM_640x480, PM_RGB565);
			
			pvr_init_params_t pvrInit = { {PVR_BINSIZE_0, PVR_BINSIZE_0, PVR_BINSIZE_32,
				PVR_BINSIZE_0, PVR_BINSIZE_0}, 512 * 1024};
			pvr_init(&pvrInit);
			
			text = new fntRenderer();
			font = new fntTexFont("/rd/default.txf");
		}

		bool run() {
			return check_input() && render();
		}

	private:
		bool check_input() {
			maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
			cont_state_t *state = (cont_state_t *)maple_dev_status(cont);
				
			if(state != NULL && state->buttons & CONT_DPAD_UP)
				playerPosY -= 1;
				
			if(state != NULL && state->buttons & CONT_DPAD_DOWN)
				playerPosY += 1;
				
			if(state != NULL && state->buttons & CONT_START)
				return false;

			return true;
		}

		bool render() {
			double ay = floor(playerPosY / gridUnit) * gridUnit - 1; // face up for now
			int gridY = ay / round(gridUnit);
			
			double ax = playerPosX + (playerPosY - ay) / tan(fov);
			int gridX = ax / gridUnit;
			
			char fname[256];
			sprintf(fname, "ray dir - x: %d, y: %d", gridX, gridY);
			draw_text(50, 50, fname);
			
			sprintf(fname, "playerY: %f", playerPosY);
			draw_text(50, 100, fname);
			
			return true;
		}

		void draw_pixel(int x, int y, int color) {
			vram_s[x + (y * resX)] = color;
		}
		
		void draw_text(int x, int y, const char *message)
		{
			pvr_wait_ready();
			pvr_scene_begin();
			pvr_list_begin(PVR_LIST_TR_POLY);

			text->setFilterMode(0);
			text->setFont(font);
			text->setPointSize(25);

			text->begin();
			text->setColor(1, 1, 0);
			text->start2f(x, y);
			text->puts(message);
			text->end();
			
			pvr_list_finish();
			pvr_scene_finish();
		}

		static const int resX = 640;
		static const int resY = 480;
		static const int fov = 60;
		static const int gridUnit = 64;

		double playerRot;
		double playerPosX;
		double playerPosY;
		
		double time;
		double oldTime;
		
		fntTexFont *font;
		fntRenderer *text;
};

int main(int argc, char **argv) {
	DarkSwack *ds = new DarkSwack();
	ds->init();

	while(ds->run()) { }

	arch_reboot();
	return 0;
}