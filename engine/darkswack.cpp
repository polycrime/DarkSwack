// raycasting code used from http://lodev.org/cgtutor/raycasting.html

#include <kos.h>
#include <math.h>
#include <dcplib/fnt.h>
#include <time.h>

// load external data
extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);

#define abs(a) ( (a) < 0 ? -(a) : (a) )

int map[10][10] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
	{1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
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
			_time = _oldTime = 0;
			dirX = -1;
			dirY = 0;
			planeX = 0;
			planeY = 0.66;
			t = time(NULL);
		}

		void init() {
			vid_set_mode(DM_640x480, PM_RGB565);
			
			/* pvr_init_params_t pvrInit = { {PVR_BINSIZE_0, PVR_BINSIZE_0, PVR_BINSIZE_32,
				PVR_BINSIZE_0, PVR_BINSIZE_0}, 512 * 1024};
			pvr_init(&pvrInit);
			
			text = new fntRenderer();
			font = new fntTexFont("/rd/default.txf"); */
		}

		bool run() {
			return check_input() && render();
		}

	private:
		bool check_input() {
			maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
			cont_state_t *state = (cont_state_t *)maple_dev_status(cont);
				
			if(state != NULL && state->buttons & CONT_DPAD_UP)
			{
				playerPosX += dirX * 0.1;
				playerPosY += dirY * 0.1;
			}
				
			if(state != NULL && state->buttons & CONT_DPAD_DOWN)
			{
				playerPosX -= dirX * 0.1;
				playerPosY -= dirY * 0.1;
			}
			
			if(state != NULL && state->buttons & CONT_DPAD_LEFT)
			{
				double oldDirX = dirX;
				dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
				dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
				double oldPlaneX = planeX;
				planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
				planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
			}
			
			if(state != NULL && state->buttons & CONT_DPAD_RIGHT)
			{
				double oldDirX = dirX;
				dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
				dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
				double oldPlaneX = planeX;
				planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
				planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
			}
				
			if(state != NULL && state->buttons & CONT_START)
				return false;

			return true;
		}

		bool render() {
			//char *stugg = NULL;
			//sprintf(stugg, "%d", difftime(t, _oldTime));
			//draw_text(0, 0, stugg);
			
			vid_clear(0, 0, 0);
		
			for(int x = 0; x < resX; x++) {
				double cameraX = 2 * x / double(resX) - 1; // x-coordinate in camera space
				double rayPosX = playerPosX;
				double rayPosY = playerPosY;
				double rayDirX = dirX + planeX * cameraX;
				double rayDirY = dirY + planeY * cameraX;
				
				//which box of the map we're in  
				int mapX = int(rayPosX);
				int mapY = int(rayPosY);
				
				double sideDistX;
				double sideDistY;
				
				double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
				double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
				double perpWallDist;
				
				int stepX;
				int stepY;
				
				int hit = 0; //was there a wall hit?
				int side; //was a NS or a EW wall hit?
				//calculate step and initial sideDist
				if (rayDirX < 0)
				{
					stepX = -1;
					sideDistX = (rayPosX - mapX) * deltaDistX;
				}
				else
				{
					stepX = 1;
					sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
				}
				
				if (rayDirY < 0)
				{
					stepY = -1;
					sideDistY = (rayPosY - mapY) * deltaDistY;
				}
				else
				{
					stepY = 1;
					sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
				}
      
				//perform DDA
				while (hit == 0)
				{
					//jump to next map square, OR in x-direction, OR in y-direction
					if (sideDistX < sideDistY)
					{
						sideDistX += deltaDistX;
						mapX += stepX;
						side = 0;
					}
					else
					{
						sideDistY += deltaDistY;
						mapY += stepY;
						side = 1;
					}
					//Check if ray has hit a wall
					if (map[mapX][mapY] > 0) hit = 1;
				} 
      
				//Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
				if (side == 0)
					perpWallDist = fabs((mapX - rayPosX + (1 - stepX) / 2) / rayDirX);
				else
					perpWallDist = fabs((mapY - rayPosY + (1 - stepY) / 2) / rayDirY);
      
				//Calculate height of line to draw on screen
				int lineHeight = abs(int(resY / perpWallDist));
       
				//calculate lowest and highest pixel to fill in current stripe
				int drawStart = -lineHeight / 2 + resY / 2;
				if(drawStart < 0) drawStart = 0;
      
				int drawEnd = lineHeight / 2 + resY / 2;
				if(drawEnd >= resY) drawEnd = resY - 1;

				//draw the pixels of the stripe as a vertical line
				draw_vertical_line(x, drawStart, drawEnd);
			}
			
			vid_waitvbl();
			
			_oldTime = t;
			
			return true;
		}
		
		void draw_vertical_line(int x, int start, int end) {
			if(end < start)
			{
				start += end;
				end = start - end;
				start = start - end;
			}
			
			if(end < 0 || start >= resY  || x < 0 || x >= resX) return;
			
			if(start < 0) start = 0; 
			
			if(end >= resX) end = resY - 1;
		
			for(int i = start; i <= end; i++) {
				draw_pixel(x, i, 0xFF);
			}
		}

		void draw_pixel(int x, int y, int color) {
			vram_s[x + (y * resX)] = color;
		}
		
		void draw_text(int x, int y, const char *message)
		{
			/* pvr_wait_ready();
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
			pvr_scene_finish(); */
		}

		static const int resX = 640;
		static const int resY = 480;
		static const int fov = 60;
		static const int gridUnit = 64;
		static const int rotSpeed = 1;

		double dirX, dirY;
		double playerPosX;
		double playerPosY;
		
		double planeX, planeY;
		
		double _time;
		double _oldTime;
		
		fntTexFont *font;
		fntRenderer *text;
		
		time_t t;
};

int main(int argc, char **argv) {
	DarkSwack *ds = new DarkSwack();
	ds->init();

	while(ds->run()) { }

	arch_reboot();
	return 0;
}