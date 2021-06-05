// [main.c]
// this template is provided for the 2D shooter game.

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>

//#define LOG_ENABLED

/* Constants. */
int total_point;
int total_life = 5;
int total_can;
int enemy_speed = +3;
int character_idx;
// int level_idx = 1 ;

// if == 1 , on
int _bgm; // 1 = mute
int _p_mode; // 1 = 2p

// Frame rate (frame per second)
const int FPS = 60;
// Display (screen) width.
const int SCREEN_W = 1200;
// Display (screen) height.
const int SCREEN_H = 900;
// At most 4 audios can be played at a time.
const int RESERVE_SAMPLES = 4;
// Same as:
// const int SCENE_MENU = 1;
// const int SCENE_START = 2;
enum {
    SCENE_MENU = 1,
    SCENE_START = 2,
	// [HACKATHON 3-7]
	// TODO: Declare a new scene id.
	SCENE_SETTINGS = 3,
	SCENE_END = 4,
	SCENE_TUTORIAL = 5,
	SCENE_TUTORIAL2 = 6
};

/* Input states */

int active_scene;
bool key_state[ALLEGRO_KEY_MAX];
// Mouse state, whether the key is down or not.
// 1 is for left, 2 is for right, 3 is for middle.
bool *mouse_state;
// Mouse position.
int mouse_x, mouse_y;

/* Variables for allegro basic routines. */
ALLEGRO_DISPLAY* game_display;
ALLEGRO_EVENT_QUEUE* game_event_queue;
ALLEGRO_TIMER* game_update_timer;

// font resource
ALLEGRO_FONT* font_pirulen_32;
ALLEGRO_FONT* font_pirulen_24;
ALLEGRO_FONT* font_catcat_80;
ALLEGRO_FONT* font_jmh_32;
ALLEGRO_FONT* font_jmh_50;

ALLEGRO_FONT* font_jmh_80;
ALLEGRO_FONT* font_dot_50;
ALLEGRO_FONT* font_dot_90;

/* Menu Scene resources*/
ALLEGRO_BITMAP* main_img_background;

ALLEGRO_BITMAP* img_settings;
ALLEGRO_BITMAP* img_settings2;

ALLEGRO_BITMAP* img_start;
ALLEGRO_BITMAP* img_start2;
ALLEGRO_BITMAP* img_book;
ALLEGRO_BITMAP* img_book2;
ALLEGRO_BITMAP* img_exit;

ALLEGRO_BITMAP* img_exit2;
ALLEGRO_BITMAP* img_arrow;
ALLEGRO_BITMAP* img_arrow2;
ALLEGRO_BITMAP* img_volume_on;
ALLEGRO_BITMAP* img_volume_mute;

ALLEGRO_SAMPLE* main_bgm;
ALLEGRO_SAMPLE_ID main_bgm_id;

/* Start Scene resources*/
ALLEGRO_BITMAP* start_img_background;
ALLEGRO_BITMAP* start_img_plane;
ALLEGRO_BITMAP* start_img_plane2;
ALLEGRO_BITMAP* start_img_plane3;
ALLEGRO_BITMAP* start_img_plane_partner;
ALLEGRO_BITMAP* start_img_plane2_partner;
ALLEGRO_BITMAP* start_img_plane3_partner;
ALLEGRO_BITMAP* start_img_enemy;
ALLEGRO_BITMAP* start_img_bigbullet;

ALLEGRO_SAMPLE* start_bgm;
ALLEGRO_SAMPLE_ID start_bgm_id;

ALLEGRO_BITMAP* img_bullet;
ALLEGRO_BITMAP* img_bullet2;
ALLEGRO_BITMAP* img_bullet3;
ALLEGRO_BITMAP* img_heart;
ALLEGRO_BITMAP* img_can;

// tutorial resource
ALLEGRO_BITMAP* img_keyboard_right;
ALLEGRO_BITMAP* img_keyboard_left;
ALLEGRO_BITMAP* img_keyboard_up;
ALLEGRO_BITMAP* img_keyboard_down;
ALLEGRO_BITMAP* img_keyboard_space;
ALLEGRO_BITMAP* img_keyboard_q; 
ALLEGRO_BITMAP* img_keyboard_w; 
ALLEGRO_BITMAP* img_keyboard_a; 
ALLEGRO_BITMAP* img_keyboard_s; 
ALLEGRO_BITMAP* img_keyboard_d; 
ALLEGRO_BITMAP* img_screen_shot;

typedef struct {
	float x, y;
	// The width and height of the object.
	float w, h;
	// The velocity in x, y axes.
	float vx, vy;
	// Should we draw this object on the screen.
	bool hidden;
	// The pointer to the object’s image.
	int hitted_time;
	ALLEGRO_BITMAP* img;
} MovableObject;

void draw_movable_object(MovableObject obj);
#define MAX_ENEMY 5
#define MAX_CAN 3
#define WIN_CAN 10
#define MAX_BULLET 200000

MovableObject plane;
MovableObject plane_partner;
MovableObject bigbullet[MAX_BULLET];
MovableObject enemies[MAX_ENEMY];
MovableObject can[MAX_CAN];
MovableObject bullets[MAX_BULLET];

const float MAX_COOLDOWN = 0.2; // the time between each bullets appear
double last_shoot_timestamp;
double last_shoot_timestamp2;
double last_warn_timestamp;

/* Declare function prototypes. */

// Initialize allegro5 library
void allegro5_init(void);
// Initialize variables and resources.
// Allows the game to perform any initialization it needs before
// starting to run.
void game_init(void);
// Process events inside the event queue using an infinity loop.
void game_start_event_loop(void);
// Run game logic such as updating the world, checking for collision,
// switching scenes and so on.
// This is called when the game should update its logic.
void game_update(void);
// Draw to display.
// This is called when the game should draw itself.
void game_draw(void);
// Release resources.
// Free the pointers we allocated.
void game_destroy(void);
// Function to change from one scene to another.
void game_change_scene(int next_scene);
// Load resized bitmap and check if failed.
ALLEGRO_BITMAP *load_bitmap_resized(const char *filename, int w, int h);
bool pnt_in_rect(int px, int py, int x, int y, int w, int h);


void on_key_down(int keycode);
void on_mouse_down(int btn, int x, int y);
void game_abort(const char* format, ...);
void game_log(const char* format, ...);
void game_vlog(const char* format, va_list arg);

int main(int argc, char** argv) {
	// Set random seed for better random outcome.
	srand(time(NULL));
	allegro5_init();
	game_log("Allegro5 initialized");
	game_log("Game begin");
	// Initialize game variables.
	game_init();
	game_log("Game initialized");
	// Draw the first frame.
	game_draw();
	game_log("Game start event loop");
	// This call blocks until the game is finished.
	game_start_event_loop();
	game_log("Game end");
	game_destroy();
	return 0;
}

void allegro5_init(void) {
	if (!al_init())
		game_abort("failed to initialize allegro");

	// Initialize add-ons.
	if (!al_init_primitives_addon())
		game_abort("failed to initialize primitives add-on");
	if (!al_init_font_addon())
		game_abort("failed to initialize font add-on");
	if (!al_init_ttf_addon())
		game_abort("failed to initialize ttf add-on");
	if (!al_init_image_addon())
		game_abort("failed to initialize image add-on");
	if (!al_install_audio())
		game_abort("failed to initialize audio add-on");
	if (!al_init_acodec_addon())
		game_abort("failed to initialize audio codec add-on");
	if (!al_reserve_samples(RESERVE_SAMPLES))
		game_abort("failed to reserve samples");
	if (!al_install_keyboard())
		game_abort("failed to install keyboard");
	if (!al_install_mouse())
		game_abort("failed to install mouse");
	// TODO: Initialize other addons such as video, ...

	// Setup game display.
	game_display = al_create_display(SCREEN_W, SCREEN_H);
	if (!game_display)
		game_abort("failed to create display");
	al_set_window_title(game_display, "I2P(I)_2019 Final Project <student_id>");

	// Setup update timer.
	game_update_timer = al_create_timer(1.0f / FPS);
	if (!game_update_timer)
		game_abort("failed to create timer");

	// Setup event queue.
	game_event_queue = al_create_event_queue();
	if (!game_event_queue)
		game_abort("failed to create event queue");

	// Malloc mouse buttons state according to button counts.
	const unsigned m_buttons = al_get_mouse_num_buttons();
	game_log("There are total %u supported mouse buttons", m_buttons);
	// mouse_state[0] will not be used.
	mouse_state = malloc((m_buttons + 1) * sizeof(bool));
	memset(mouse_state, false, (m_buttons + 1) * sizeof(bool));

	// Register display, timer, keyboard, mouse events to the event queue.
	al_register_event_source(game_event_queue, al_get_display_event_source(game_display));
	al_register_event_source(game_event_queue, al_get_timer_event_source(game_update_timer));
	al_register_event_source(game_event_queue, al_get_keyboard_event_source());
	al_register_event_source(game_event_queue, al_get_mouse_event_source());
	// TODO: Register other event sources such as timer, video, ...

	// Start the timer to update and draw the game.
	al_start_timer(game_update_timer);
}

// input files
void game_init(void) {
	// font resource
	font_pirulen_32 = al_load_font("pirulen.ttf", 32, 0);
	if (!font_pirulen_32)
		game_abort("failed to load font: pirulen.ttf with size 32");
	font_pirulen_24 = al_load_font("pirulen.ttf", 24, 0);
	if (!font_pirulen_24)
		game_abort("failed to load font: pirulen.ttf with size 24");

	font_catcat_80 = al_load_font("catcatfont.ttf", 80, 0);
	if (!font_catcat_80)
		game_abort("failed to load font: catcatfont.ttf with size 80");

	font_jmh_32 = al_load_font("JMH Beda.ttf", 40, 0);
	if (!font_jmh_32)
		game_abort("failed to load font: JMH Beda.ttf with size 32");
	font_jmh_50 = al_load_font("JMH Beda.ttf", 50, 0);
	if (!font_jmh_50)
		game_abort("failed to load font: JMH Beda.ttf with size 50");
	font_jmh_80 = al_load_font("JMH Beda.ttf", 80, 0);
	if (!font_jmh_80)
		game_abort("failed to load font: JMH Beda.ttf with size 80");

	font_dot_90 = al_load_font("dotmat.ttf", 90, 0);
	if (!font_dot_90)
		game_abort("failed to load font: dotmat.ttf with size 90");
	font_dot_50 = al_load_font("dotmat.ttf", 50, 0);
	if (!font_dot_50)
		game_abort("failed to load font: dotmat.ttf with size 50");

	// background pic
	main_img_background = load_bitmap_resized("cat_background.png", SCREEN_W, SCREEN_H);
	start_img_background = load_bitmap_resized("white_room.jpg", SCREEN_W, SCREEN_H);

	// music
	main_bgm = al_load_sample("S31-Night Prowler.ogg");
	if (!main_bgm)
		game_abort("failed to load audio: S31-Night Prowler.ogg");
	start_bgm = al_load_sample("mythica.ogg");
	if (!start_bgm)
		game_abort("failed to load audio: mythica.ogg");

	// BOTTOMS
	// menu
	img_settings = al_load_bitmap("settings.png");
	if (!img_settings)
		game_abort("failed to load image: settings.png");
	img_settings2 = al_load_bitmap("settings2.png");
	if (!img_settings2)
		game_abort("failed to load image: settings2.png");
	
	img_start = al_load_bitmap("start.png");
	if (!img_start)
		game_abort("failed to load image: start.png");
	img_start2 = al_load_bitmap("start2.png");
	if (!img_start2)
		game_abort("failed to load image: start2.png");

	img_book = al_load_bitmap("info.png");
	if (!img_book)
		game_abort("failed to load image: info.png");
	img_book2 = al_load_bitmap("info2.png");
	if (!img_book2)
		game_abort("failed to load image: info2.png");
	
	img_exit = al_load_bitmap("exit.png");
	if (!img_exit)
		game_abort("failed to load image: exit.png");
	img_exit2 = al_load_bitmap("exit2.png");
	if (!img_exit2)
		game_abort("failed to load image: exit2.png");

	// setting 
	img_volume_on = al_load_bitmap("volume.png");
	if (!img_volume_on)
		game_abort("failed to load image: volume.png");

	img_volume_mute = al_load_bitmap("volume_mute.png");
	if (!img_volume_mute)
		game_abort("failed to load image: volume_mute.png");
	img_arrow = al_load_bitmap("arrow.png");
	if (!img_arrow)
		game_abort("failed to load image: arrow.png");
	img_arrow2 = al_load_bitmap("arrow2.png");
	if (!img_arrow2)
		game_abort("failed to load image: arrow2.png");

	// tutorial
	img_keyboard_left = al_load_bitmap("keyboard_left2.png");
	if (!img_keyboard_left)
		game_abort("failed to load image: keyboard_left2.png");

	img_keyboard_right = al_load_bitmap("keyboard_right2.png");
	if (!img_keyboard_right)
		game_abort("failed to load image: keyboard_right2.png");

	img_keyboard_up = al_load_bitmap("keyboard_up.png");
	if (!img_keyboard_up)
		game_abort("failed to load image: keyboard_up.png");

	img_keyboard_down = al_load_bitmap("keyboard_down.png");
	if (!img_keyboard_down)
		game_abort("failed to load image: keyboard_down.png");

	img_keyboard_space = al_load_bitmap("keyboard_space.png");
	if (!img_keyboard_space)
		game_abort("failed to load image: keyboard_space.png");

	img_screen_shot = al_load_bitmap("screenshot.png");
	if (!img_screen_shot)
		game_abort("failed to load image: screenshot.png");

	// 2p tutorial
	img_keyboard_q = al_load_bitmap("keyboard_q.png");
	if (!img_keyboard_q)
		game_abort("failed to load image: keyboard_q.png");
	img_keyboard_w = al_load_bitmap("keyboard_w.png");
	if (!img_keyboard_w)
		game_abort("failed to load image: keyboard_w.png");
	img_keyboard_a = al_load_bitmap("keyboard_a.png");
	if (!img_keyboard_a)
		game_abort("failed to load image: keyboard_a.png");
	img_keyboard_s = al_load_bitmap("keyboard_s.png");
	if (!img_keyboard_s)
		game_abort("failed to load image: keyboard_s.png");
	img_keyboard_d = al_load_bitmap("keyboard_d.png");
	if (!img_keyboard_d)
		game_abort("failed to load image: keyboard_d.png");

	// start scene
	// main character
	start_img_plane = al_load_bitmap("yellow_cat1.png");
	if (!start_img_plane)
		game_abort("failed to load image: yellow_cat1.png");
	start_img_plane2 = al_load_bitmap("white_cat5.png");
	if (!start_img_plane2)
		game_abort("failed to load image: w_cat5.png");
	start_img_plane3 = al_load_bitmap("brown_cat5.png");
	if (!start_img_plane3)
		game_abort("failed to load image: b_cat5.png");

	// partner
	start_img_plane_partner = al_load_bitmap("yellow_cat5.png");
	if (!start_img_plane_partner)
		game_abort("failed to load image: yellow_cat5.png");
	start_img_plane2_partner = al_load_bitmap("white_cat3.png");
	if (!start_img_plane2_partner)
		game_abort("failed to load image: w_cat3.png");
	start_img_plane3_partner = al_load_bitmap("brown_cat1.png");
	if (!start_img_plane3_partner)
		game_abort("failed to load image: b_cat1.png");

	// enemy 
	start_img_enemy = al_load_bitmap("smallmouse.png");
	if (!start_img_enemy)
		game_abort("failed to load image: smallmouse.png");
	
	start_img_bigbullet = al_load_bitmap("boom.png");
	if (!start_img_bigbullet)
		game_abort("failed to load image: boom.png");
	
	// bullet 
	img_bullet = al_load_bitmap("apple.png");
	if (!img_bullet)
		game_abort("failed to load image: apple.png");
	img_bullet2 = al_load_bitmap("fire.png");
	if (!img_bullet2)
		game_abort("failed to load image: fire.png");
	img_bullet3 = al_load_bitmap("ball.png");
	if (!img_bullet3)
		game_abort("failed to load image: ball.png");

	// accessories
	img_heart = al_load_bitmap("heart.png");
	if (!img_heart)
		game_abort("failed to load image: heart.png");

	img_can = al_load_bitmap("can.png");
	if (!img_can)
		game_abort("failed to load image: can.png");
	
	
	// Change to first scene.
	game_change_scene(SCENE_MENU);
}

void game_start_event_loop(void) {
	bool done = false;
	ALLEGRO_EVENT event;
	int redraws = 0;
	while (!done) {
		al_wait_for_event(game_event_queue, &event);
		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			// Event for clicking the window close button.
			game_log("Window close button clicked");
			done = true;
		} 
		else if (event.type == ALLEGRO_EVENT_TIMER) {
			// Event for redrawing the display.
			if (event.timer.source == game_update_timer)
				// The redraw timer has ticked.
				redraws++;
		} 
		else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			// Event for keyboard key down.
			game_log("Key with keycode %d down", event.keyboard.keycode);
			key_state[event.keyboard.keycode] = true;
			on_key_down(event.keyboard.keycode);
		} 
		else if (event.type == ALLEGRO_EVENT_KEY_UP) {
			// Event for keyboard key up.
			game_log("Key with keycode %d up", event.keyboard.keycode);
			key_state[event.keyboard.keycode] = false;
		} 
		else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			// Event for mouse key down.
			game_log("Mouse button %d down at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
			mouse_state[event.mouse.button] = true;
			on_mouse_down(event.mouse.button, event.mouse.x, event.mouse.y);
		} 
		else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			// Event for mouse key up.
			game_log("Mouse button %d up at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
			mouse_state[event.mouse.button] = false;
		} 
		else if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
			if (event.mouse.dx != 0 || event.mouse.dy != 0) {
				// Event for mouse move.
				game_log("Mouse move to (%d, %d)", event.mouse.x, event.mouse.y);
				mouse_x = event.mouse.x;
				mouse_y = event.mouse.y;
			} else if (event.mouse.dz != 0) {
				// Event for mouse scroll.
				game_log("Mouse scroll at (%d, %d) with delta %d", event.mouse.x, event.mouse.y, event.mouse.dz);
			}
		}
		// TODO: Process more events and call callbacks by adding more
		// entries inside Scene.

		// Redraw
		if (redraws > 0 && al_is_event_queue_empty(game_event_queue)) {
			// if (redraws > 1)
			// 	game_log("%d frame(s) dropped", redraws - 1);
			// Update and draw the next frame.
			game_update();
			game_draw();
			redraws = 0;
		}
	}
}

// update
void game_update(void) {
	
	if (active_scene == SCENE_END){
		if(key_state[ALLEGRO_KEY_ESCAPE]  ){
			game_log("Game end");
			game_destroy();
		}
	} 
	else if (active_scene == SCENE_START) {

		// 1p plane12
		plane.vx = plane.vy = 0;
		if (key_state[ALLEGRO_KEY_UP] )
			plane.vy -= (1+ total_can*0.5);
		if (key_state[ALLEGRO_KEY_DOWN])
			plane.vy += (1+ total_can*0.5);
		if (key_state[ALLEGRO_KEY_LEFT])
			plane.vx -= (1+ total_can*0.5) ;
		if (key_state[ALLEGRO_KEY_RIGHT])
			plane.vx += (1+ total_can*0.5);
		// 0.71 is (1/sqrt(2)).
		plane.y += plane.vy * 4 * (plane.vx ? 0.71f : 1);
		plane.x += plane.vx * 4 * (plane.vy ? 0.71f : 1);
		
		if (plane.x - plane.w/2 < 0)
			plane.x = plane.w/2;
		else if (plane.x > SCREEN_W - plane.w/2)
			plane.x = SCREEN_W - plane.w/2;
		if (plane.y - plane.h/2 < 0)
			plane.y = plane.h/2;
		else if (plane.y > SCREEN_H - plane.h/2)
			plane.y = SCREEN_H - plane.h/2;

		// 2p palne
		if(_p_mode == 1){
			plane_partner.vx = plane_partner.vy = 0;
			if (key_state[ALLEGRO_KEY_W])
				plane_partner.vy -= 1;
			if (key_state[ALLEGRO_KEY_S])
				plane_partner.vy += 1;
			if (key_state[ALLEGRO_KEY_A])
				plane_partner.vx -= 1+ total_can/3 ;
			if (key_state[ALLEGRO_KEY_D])
				plane_partner.vx += 1+ total_can/3;
			// 0.71 is (1/sqrt(2)).
			plane_partner.y += plane_partner.vy * 4 * (plane_partner.vx ? 0.71f : 1);
			plane_partner.x += plane_partner.vx * 4 * (plane_partner.vy ? 0.71f : 1);
			
			if (plane_partner.x - plane_partner.w/2 < 0)
				plane_partner.x = plane_partner.w/2;
			else if (plane_partner.x > SCREEN_W - plane_partner.w/2)
				plane_partner.x = SCREEN_W - plane_partner.w/2;
			if (plane_partner.y - plane_partner.h/2 < 0)
				plane_partner.y = plane_partner.h/2;
			else if (plane_partner.y > SCREEN_H - plane_partner.h/2)
				plane_partner.y = SCREEN_H - plane_partner.h/2;
		}
		

		int i,j,k,c,b;
		// boom position
		for(b = 0; b < total_can ; b ++){
			bigbullet[b].x += bigbullet[b].vx;
			bigbullet[b].y += bigbullet[b].vy;
			if ( bigbullet[b].y > SCREEN_H ){
				
				bigbullet[b].x = 2 * bigbullet[b].w  + (float)rand() / RAND_MAX * (SCREEN_W - 2 * bigbullet[b].w);
				bigbullet[b].y = bigbullet[b].h  + (float)rand() / RAND_MAX * (SCREEN_H/4 - bigbullet[b].h);
			}
		} 
		// enemy position
		for(k = 0; k < MAX_ENEMY ; k ++){
			enemies[k].x += enemies[k].vx;
			enemies[k].y += enemies[k].vy * ( 1 + 0.4*total_point/10);
		
			if ( enemies[k].y > SCREEN_H || enemies[k].hidden == true ){
				enemies[k].x = enemies[k].w  + (float)rand() / RAND_MAX * (SCREEN_W - 2 * enemies[k].w);
				enemies[k].y = enemies[k].h  + (float)rand() / RAND_MAX * (SCREEN_H/4 - enemies[k].h);
			}
		} 
		// can position
		for(c = 0; c < MAX_CAN; c ++){
			can[c].x += can[c].vx;
			can[c].y += can[c].vy;
			if( can[c].y > SCREEN_H || can[c].hidden == true ){
				can[c].x = 2 * can[c].w + (float)rand() / RAND_MAX * (SCREEN_W - 2 * can[k].w);
				can[c].y = can[c].h + (float)rand() / RAND_MAX * (SCREEN_H - 2 * can[k].h);
			}
		}

		// bullet hit the enemy
		for (i = 0; i < MAX_BULLET ; i ++) {
			if (bullets[i].hidden )
				continue;
			bullets[i].x += bullets[i].vx;
			bullets[i].y += bullets[i].vy-(2*total_can);
			
			if ( bullets[i].y < 0 )
				bullets[i].hidden = true;
			else{
				for( j = 0; j < MAX_ENEMY; j ++){
					// if( bullets[i].y <= enemies[j].y + enemies[j].h && bullets[i].x >= enemies[j].x - enemies[j].w/2  && bullets[i].x + bullets[i].w <= enemies[j].x + enemies[j].w ){
					if( bullets[i].y <= enemies[j].y + enemies[j].h &&  bullets[i].x <= enemies[j].x + enemies[j].w && bullets[i].x + bullets[i].w >= enemies[j].x  ){
						bullets[i].hidden = true;
						enemies[j].hidden = true;
						total_point ++;
					}
				}
			}
		}

		// hit by mouse -> life --
		for(j = 0; j < MAX_ENEMY; j ++){
			if( plane.x + plane.w >= enemies[j].x && plane.x <= enemies[j].x+enemies[j].w && enemies[j].y+enemies[j].h/2 >= plane.y){
				enemies[j].hidden = true;
				total_life --;
			}
			if(_p_mode == 1){
				if( plane_partner.x + plane_partner.w >= enemies[j].x && plane_partner.x <= enemies[j].x+enemies[j].w && enemies[j].y+enemies[j].h/2 >= plane_partner.y){
				enemies[j].hidden = true;
				total_life --;
				}
			}
		}

		// eat the can
		for(c = 0; c < MAX_CAN;c ++){
			if( plane.x + plane.w >= can[c].x && plane.x <= can[c].x+can[c].w && can[c].y+can[c].h/2 >= plane.y){
				can[c].hidden = true;
				if(total_can +1 <= MAX_CAN)
					total_can ++;
			}
			if(_p_mode == 1){
				if( plane_partner.x + plane_partner.w >= can[c].x && plane_partner.x <= can[c].x+can[c].w && can[c].y+can[c].h/2 >= plane_partner.y){
				can[c].hidden = true;
				if(total_can +1 <= MAX_CAN)
					total_can ++;
				}
			}
		}

		// hit the boom and die
		for(b = 0; b < MAX_CAN;b ++){
			if( plane.x + plane.w >= bigbullet[b].x && plane.x <= bigbullet[b].x+bigbullet[b].w && bigbullet[b].y+bigbullet[b].h/2 >= plane.y){
				game_change_scene(SCENE_END);
			}
			if(_p_mode == 1){
				if( plane_partner.x + plane_partner.w >= bigbullet[b].x && plane_partner.x <= bigbullet[b].x+bigbullet[b].w && bigbullet[b].y+bigbullet[b].h/2 >= plane_partner.y){
				game_change_scene(SCENE_END);
				}
			}
		}

		// position of bullet at the front of partner's plane
		double now = al_get_time();
		if (key_state[ALLEGRO_KEY_SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN  ) {
		    for (i = 0; i < MAX_BULLET; i ++ ) {
		         if (bullets[i].hidden)
		            break;
		    }
		    if (i < MAX_BULLET) {
		        last_shoot_timestamp = now;
		        bullets[i].hidden = false;
		        bullets[i].x = plane.x;
		        bullets[i].y = plane.y - plane.h/2;

		    }
		}
		// 2p's bullet position at the front of partner's plane
		if(_p_mode == 1){
			double now2 = al_get_time();
			if (key_state[ALLEGRO_KEY_Q] && now2 - last_shoot_timestamp2 >= MAX_COOLDOWN  ) {
				for (b = 0; b < MAX_BULLET; b ++ ) {
					if (bullets[b].hidden)
						break;
				}
				if (b < MAX_BULLET) {
					last_shoot_timestamp2= now2;
					bullets[b].hidden = false;
					bullets[b].x = plane_partner.x;
					bullets[b].y = plane_partner.y - plane_partner.h/2;
				}
			}
		}
		
		// enemy
		for(k = 0; k < MAX_ENEMY; k ++){
			if(enemies[k].hidden)
				break;
		}
		if(k < MAX_ENEMY){
			enemies[k].hidden = false;
			enemies[k].x = enemies[k].w  + (float)rand() / RAND_MAX * (SCREEN_W - 2 * enemies[k].w);
			enemies[k].y = enemies[k].h  + (float)rand() / RAND_MAX * (SCREEN_H/4 - enemies[k].h);
		}

		// boom
		for(b = 0; b < total_can ; b ++){
			if(bigbullet[b].hidden)
				break;
		}
		if(b < total_can){
			bigbullet[b].hidden = false;
			bigbullet[b].x = bigbullet[b].w  + (float)rand() / RAND_MAX * (SCREEN_W - 2 * bigbullet[b].w);
			bigbullet[b].y = bigbullet[b].h  + (float)rand() / RAND_MAX * (SCREEN_H/4 - bigbullet[b].h);
		}
		
		// can
		for(c = 0; c < MAX_CAN; c ++){
			if(can[c].hidden)
				break;
		}
		if(c < MAX_CAN){
			can[c].hidden = false;
			can[c].x = 3 * can[c].w  + (float)rand() / RAND_MAX * (SCREEN_W - 2 * can[c].w);
			can[c].y = can[c].h  + (float)rand() / RAND_MAX * (SCREEN_H/4 - can[c].h);
		}
		
		// if "esc" , change the scene to scene4
		if(key_state[ALLEGRO_KEY_ESCAPE] || total_life <= 0){
			game_change_scene(SCENE_END);
		}
	}
}

// draw for each scene
void game_draw(void) {
	
	if (active_scene == SCENE_MENU) {
		al_draw_bitmap(main_img_background, 0, 0, 0);
		al_draw_text(font_jmh_50, al_map_rgb(255, 255, 255), 200, 30, 0, "Mice Shooter");
		// al_draw_text(font_jmh_50, al_map_rgb(255, 255, 255), 20, SCREEN_H - 70, 0, "Press enter key to start");
		// [HACKATHON 3-5]
		// TODO: Draw settings images.
		// The settings icon should be located at (x, y, w, h) =
		// (SCREEN_W - 48, 10, 38, 38).
		// Change its image according to your mouse position.
		// Uncomment and fill in the code below.
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W - 48, 10, 38, 38))
			al_draw_bitmap(img_settings2, SCREEN_W - 48, 10, 0);
		else
			al_draw_bitmap(img_settings, SCREEN_W - 48, 10, 0);
		
		// start sign
		if (pnt_in_rect(mouse_x, mouse_y, 150, 700, 100, 100))
			al_draw_bitmap(img_start2, 150, 700, 0 );
		else
			al_draw_bitmap(img_start, 150, 710, 0 );

		// book sign
		if (pnt_in_rect(mouse_x, mouse_y, 300, 700, 100, 100))
			al_draw_bitmap(img_book2, 300, 700, 0);
		else
			al_draw_bitmap(img_book, 300, 710, 0);

		// exit sign
		if (pnt_in_rect(mouse_x, mouse_y, 450, 700, 100, 100))
			al_draw_bitmap(img_exit2, 450, 700, 0);
		else
			al_draw_bitmap(img_exit, 450, 710, 0);
		
	} 
	else if (active_scene == SCENE_END) {
		al_clear_to_color( al_map_rgb(164,104,67)) ;
		al_draw_text(font_jmh_80, al_map_rgb(240,203,72), SCREEN_W / 2, SCREEN_H/2-200, ALLEGRO_ALIGN_CENTER, "end game");
		al_draw_text(font_catcat_80, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H/2-100, ALLEGRO_ALIGN_CENTER, "THIS");
		al_draw_textf(font_jmh_50, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H/2, ALLEGRO_ALIGN_CENTER, "points you get: %d", total_point);
		//al_draw_text(font_jmh_50, al_map_rgb(255,217,84), SCREEN_W / 2, SCREEN_H/2+100, ALLEGRO_ALIGN_CENTER, "press ESC to exit");
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2-50, SCREEN_H/2+100, 100, 100))
			al_draw_bitmap(img_exit2, SCREEN_W / 2-50, SCREEN_H/2+100, 0);
		else
			al_draw_bitmap(img_exit, SCREEN_W / 2-50, SCREEN_H/2+110, 0);
	} 
	else if (active_scene == SCENE_TUTORIAL) { 
		al_clear_to_color( al_map_rgb(189,142,98) );
		// al_draw_text(font_catcat_80, al_map_rgb(255, 255, 255), SCREEN_W / 2, 80, ALLEGRO_ALIGN_CENTER, "ABCDF");
		al_draw_text(font_jmh_32, al_map_rgb(255, 255, 255), SCREEN_W / 2-410 , 30, 0, "How to play ?");
		
		// movement
		
		
		al_draw_bitmap(img_keyboard_up, SCREEN_W / 2 - 320 , 90, 0);
		al_draw_bitmap(img_keyboard_left, SCREEN_W / 2 - 400 , 160, 0);
		al_draw_bitmap(img_keyboard_down, SCREEN_W / 2 - 320, 160, 0);
		al_draw_bitmap(img_keyboard_right, SCREEN_W / 2 - 240, 160, 0);
		al_draw_text(font_jmh_32, al_map_rgb(240,203,72), SCREEN_W / 2-390,230, 0, "to move the cat");
		al_draw_bitmap(img_keyboard_space, SCREEN_W / 2  , 160, 0);
		al_draw_text(font_jmh_32, al_map_rgb(240,203,72), SCREEN_W / 2 ,230, 0, "to shoot");

		

		// screenshot
		al_draw_bitmap(img_screen_shot, SCREEN_W / 2 -400, 310, 0);
		al_draw_text(font_jmh_32, al_map_rgb(96,143,138), SCREEN_W / 2-100 , 330, 0, "* shoot one mouse : +1");
		al_draw_text(font_jmh_32, al_map_rgb(186,34,36), SCREEN_W / 2-100 , 370, 0, "* lose life if hitten by enemy.");
		al_draw_text(font_jmh_32, al_map_rgb(240,203,72), SCREEN_W / 2-100, 410, 0, "* meet one cat-food : +1");
		al_draw_text(font_jmh_32, al_map_rgb(240,203,72), SCREEN_W / 2-100 ,450, 0, "  cat-food makes cat run faster.");
		
		// optional settings
		al_draw_text(font_jmh_32, al_map_rgb(255, 255, 255), SCREEN_W / 2 -410, 540, 0, "Optional Settings");
		al_draw_bitmap(start_img_plane2, SCREEN_W / 2 -400, 590, 0);
		al_draw_bitmap(img_bullet2, SCREEN_W / 2 -300, 590, 0);
		al_draw_bitmap(start_img_plane3, SCREEN_W / 2 -400, 660, 0);
		al_draw_bitmap(img_bullet3, SCREEN_W / 2 -300, 660, 0);
		al_draw_text(font_jmh_32, al_map_rgb(240,203,72), SCREEN_W / 2-100 ,600, 0, "* change different character");
		al_draw_text(font_jmh_32, al_map_rgb(240,203,72), SCREEN_W / 2-100 ,640, 0, " to have different weapon.");
		/* al_draw_text(font_jmh_32, al_map_rgb(186,34,36), SCREEN_W / 2-100 ,650, 0, "* warning!!");
		al_draw_text(font_jmh_32, al_map_rgb(186,34,36), SCREEN_W / 2-100 ,700, 0, "  don't shoot your friend"); */


		al_draw_bitmap(start_img_plane3_partner, SCREEN_W / 2 -400, 780, 0);
		al_draw_bitmap(start_img_plane3, SCREEN_W / 2 -300, 780, 0);
		al_draw_text(font_jmh_32, al_map_rgb(240,203,72), SCREEN_W / 2-100 ,790, 0, "* change to 2P mode.");

		// back to menu
		if (pnt_in_rect(mouse_x, mouse_y, 50, SCREEN_H/2, 100, 100))
			al_draw_bitmap(img_arrow2, 50, SCREEN_H/2, 0);
		else
			al_draw_bitmap(img_arrow, 50, SCREEN_H/2, 0);
		
		// go to play
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W - 150, SCREEN_H/2, 100, 100))
			al_draw_bitmap(img_start2, SCREEN_W - 150, SCREEN_H/2, 0);
		else
			al_draw_bitmap(img_start, SCREEN_W - 150, SCREEN_H/2, 0);
	} 
	
	else if (active_scene == SCENE_TUTORIAL2){
		al_clear_to_color( al_map_rgb(127,177,116) );
		al_draw_text(font_jmh_50, al_map_rgb(255, 255, 255), SCREEN_W / 2-410 , 50, 0, "How to control second player ?");
		al_draw_text(font_jmh_50, al_map_rgb(186,34,36), SCREEN_W / 2-410 ,120, 0, "you are in 2p mode now");
		al_draw_text(font_jmh_32, al_map_rgb(255, 255, 255), SCREEN_W / 2-410 ,230, 0, "* second cat");
		al_draw_text(font_jmh_32, al_map_rgb(255, 255, 255), SCREEN_W / 2 ,230, 0, "* first cat");
		al_draw_text(font_jmh_32, al_map_rgb(240,203,72), SCREEN_W / 2-410 ,290, 0, "to shoot");
		al_draw_filled_rectangle(SCREEN_W/2-100, 230, SCREEN_W/2-90, SCREEN_H/2+250,al_map_rgb(255, 255, 255));
		

		al_draw_bitmap(img_keyboard_q, SCREEN_W / 2 - 390, SCREEN_H/2-100, 0);
		
		al_draw_text(font_jmh_32, al_map_rgb(240,203,72), SCREEN_W / 2-410 ,SCREEN_H/2, 0, "to move");
		al_draw_bitmap(img_keyboard_w, SCREEN_W / 2 - 350 , SCREEN_H/2+80, 0);
		al_draw_bitmap(img_keyboard_a, SCREEN_W / 2 - 390, SCREEN_H/2+160, 0);
		al_draw_bitmap(img_keyboard_s, SCREEN_W / 2 -310, SCREEN_H/2+160, 0);
		al_draw_bitmap(img_keyboard_d, SCREEN_W / 2 -230, SCREEN_H/2+160, 0);

		al_draw_bitmap(img_keyboard_space, SCREEN_W / 2 , SCREEN_H/2-100, 0);
		al_draw_bitmap(img_keyboard_up, SCREEN_W / 2 +80 , SCREEN_H/2+80, 0);
		al_draw_bitmap(img_keyboard_left, SCREEN_W / 2 , SCREEN_H/2+160, 0);
		al_draw_bitmap(img_keyboard_down, SCREEN_W / 2 +80, SCREEN_H/2+160, 0);
		al_draw_bitmap(img_keyboard_right, SCREEN_W / 2 +160, SCREEN_H/2+160, 0);
		

		// back to menu
		if (pnt_in_rect(mouse_x, mouse_y, 50, SCREEN_H/2, 100, 100))
			al_draw_bitmap(img_arrow2, 50, SCREEN_H/2, 0);
		else
			al_draw_bitmap(img_arrow, 50, SCREEN_H/2, 0);

		// go to play
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W-150 , SCREEN_H/2, 100, 100))
			al_draw_bitmap(img_start2, SCREEN_W -150, SCREEN_H/2, 0);
		else
			al_draw_bitmap(img_start, SCREEN_W -150, SCREEN_H/2, 0);
	}

	else if (active_scene == SCENE_START) {
		int i;
		al_draw_bitmap(start_img_background, 0, 0, 0);
		for(i = 0; i < total_life; i ++)
			al_draw_bitmap(img_heart, 30 +35*i, 63, 0);
		for(i = 0; i < total_can ; i ++)
			al_draw_bitmap(img_can, 30 +35*i, 100, 0);
		
		al_draw_textf(font_jmh_32, al_map_rgb(96,143,138), 30, 20, 0, "score: %d", total_point);
		// al_draw_textf(font_jmh_32, al_map_rgb(104, 156, 151), 0, 100, 0, "life: %d/5", total_life);

		// "level up warning"
		if(total_point%10 >= 0 && total_point%10 <= 2  ){
			al_draw_textf(font_dot_90, al_map_rgb(186,34,36), SCREEN_W/2, SCREEN_H/2 , ALLEGRO_ALIGN_CENTER, "LEVEL %d",total_point/10 +1);
			if(total_point >= 10)
				al_draw_textf(font_dot_50, al_map_rgb(186,34,36), SCREEN_W / 2, SCREEN_H/2 + 100, ALLEGRO_ALIGN_CENTER, "speed up");
		}
		if(total_can >= 1){
			al_draw_text(font_jmh_32, al_map_rgb(186,34,36), 30, SCREEN_H - 90, 0, "don't meet the boom!");
		}
		al_draw_textf(font_jmh_32, al_map_rgb(186,34,36), 30, SCREEN_H - 50, 0, "%dp mode",_p_mode+1);
		
		for (i = 0; i < MAX_BULLET; i ++)
			draw_movable_object(bullets[i]);
		draw_movable_object(plane);
		for (i = 0; i < MAX_ENEMY; i++)
			draw_movable_object(enemies[i]);
		for (i = 0; i < MAX_CAN; i++)
			draw_movable_object(can[i]);
		for (i = 0; i < total_can ; i++)
			draw_movable_object(bigbullet[i]);
		if(_p_mode == 1)
			draw_movable_object(plane_partner);
		
	}
	
	else if (active_scene == SCENE_SETTINGS) {
		al_clear_to_color( al_map_rgb(104,156,151) );
		al_draw_text(font_catcat_80, al_map_rgb(255, 255, 255), SCREEN_W / 2, 80, ALLEGRO_ALIGN_CENTER, "ABCDF");
		al_draw_text(font_dot_90, al_map_rgb(255, 255, 255), SCREEN_W / 2, 170, ALLEGRO_ALIGN_CENTER, "-SETTING-");
		al_draw_text(font_dot_50, al_map_rgb(255, 255, 255), SCREEN_W / 2, 280, ALLEGRO_ALIGN_CENTER, "MODE");
		
		// 1p
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 -150, 350, 60,60))
			al_draw_bitmap(start_img_plane3, SCREEN_W / 2-150, 350, 0);
		else
			al_draw_bitmap(start_img_plane3, SCREEN_W / 2-150, 340, 0);

		// 2p
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 +50, 350, 70,60))
			al_draw_bitmap(start_img_plane3, SCREEN_W / 2+50, 350, 0);
		else
			al_draw_bitmap(start_img_plane3, SCREEN_W / 2+50, 340, 0);
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 +100, 350, 70,60))
			al_draw_bitmap(start_img_plane3_partner, SCREEN_W / 2+100, 350, 0);
		else
			al_draw_bitmap(start_img_plane3_partner, SCREEN_W / 2+100, 340, 0);

		al_draw_text(font_dot_50, al_map_rgb(255, 255, 255), SCREEN_W / 2, 450, ALLEGRO_ALIGN_CENTER, "SOUND");
		// sound changing signs
		// mute
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 -100, 510, 70,70))
			al_draw_bitmap(img_volume_mute, SCREEN_W / 2-100, 510, 0);
		else
			al_draw_bitmap(img_volume_mute, SCREEN_W / 2-100, 500, 0);
		//on
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2+30, 510, 70,70))
			al_draw_bitmap(img_volume_on, SCREEN_W / 2+30, 510, 0);
		else
			al_draw_bitmap(img_volume_on, SCREEN_W / 2+30, 500, 0);
		

		al_draw_text(font_dot_50, al_map_rgb(255, 255, 255), SCREEN_W / 2, 630, ALLEGRO_ALIGN_CENTER, "CHARACTER");
		// ori-cat
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2-150, 710, 60,60))
			al_draw_bitmap(img_bullet, SCREEN_W / 2-150, 710, 0);
		else
			al_draw_bitmap(start_img_plane, SCREEN_W / 2-150, 700, 0);
		// cat2
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2-40, 710, 60,60))
			al_draw_bitmap(img_bullet2, SCREEN_W / 2-40, 710, 0);
		else
			al_draw_bitmap(start_img_plane2, SCREEN_W / 2-40, 700, 0);
		// cat3
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2+70, 710, 70,70))
			al_draw_bitmap(img_bullet3, SCREEN_W / 2+70, 710, 0);
		else
			al_draw_bitmap(start_img_plane3, SCREEN_W / 2+70, 700, 0);
		al_draw_text(font_jmh_32, al_map_rgb(242,171,57), SCREEN_W / 2, 800, ALLEGRO_ALIGN_CENTER, "Press           or \"enter\" to start the game.");
		//al_draw_bitmap(img_start, SCREEN_W /2 - 240, 780, 0);

		// exit sign
		if (pnt_in_rect(mouse_x, mouse_y, 50, SCREEN_H/2, 100, 100))
			al_draw_bitmap(img_arrow2, 50, SCREEN_H/2, 0);
		else
			al_draw_bitmap(img_arrow, 50, SCREEN_H/2, 0);
		// go to play
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W /2 - 240, 780, 100, 100))
			al_draw_bitmap(img_start2, SCREEN_W /2 - 240, 780, 0);
		else
			al_draw_bitmap(img_start, SCREEN_W /2 - 240, 780, 0);
		
	}
	al_flip_display();

}

void game_destroy(void) {
	// font
	al_destroy_font(font_pirulen_32);
	al_destroy_font(font_pirulen_24);
	al_destroy_font(font_jmh_32);
	al_destroy_font(font_jmh_50);
	al_destroy_font(font_jmh_80);

	al_destroy_font(font_catcat_80);
	al_destroy_font(font_dot_90);
	al_destroy_font(font_dot_50);

	// background, music
	al_destroy_bitmap(main_img_background);
	al_destroy_bitmap(start_img_background);
	al_destroy_sample(main_bgm);
	al_destroy_sample(start_bgm);
	
	// menu, setting
	al_destroy_bitmap(img_settings);
	al_destroy_bitmap(img_settings2);
	al_destroy_bitmap(img_book);
	al_destroy_bitmap(img_book2);
	al_destroy_bitmap(img_start);
	al_destroy_bitmap(img_start2);
	al_destroy_bitmap(img_exit);
	al_destroy_bitmap(img_exit2);
	al_destroy_bitmap(img_arrow);
	al_destroy_bitmap(img_arrow2);
	al_destroy_bitmap(img_volume_on);
	al_destroy_bitmap(img_volume_mute);

	// tutorial
	al_destroy_bitmap(img_keyboard_left);
	al_destroy_bitmap(img_keyboard_right);
	al_destroy_bitmap(img_keyboard_up);
	al_destroy_bitmap(img_keyboard_down);
	al_destroy_bitmap(img_keyboard_space);
	al_destroy_bitmap(img_screen_shot);

	//2p tutorial
	al_destroy_bitmap(img_keyboard_q);
	al_destroy_bitmap(img_keyboard_w);
	al_destroy_bitmap(img_keyboard_a);
	al_destroy_bitmap(img_keyboard_s);
	al_destroy_bitmap(img_keyboard_d);

	// start
	al_destroy_bitmap(start_img_plane);
	al_destroy_bitmap(start_img_plane2);
	al_destroy_bitmap(start_img_plane3);
	al_destroy_bitmap(start_img_plane_partner);
	al_destroy_bitmap(start_img_plane2_partner);
	al_destroy_bitmap(start_img_plane3_partner);
	al_destroy_bitmap(start_img_enemy);
	al_destroy_bitmap(start_img_bigbullet);
	

	al_destroy_bitmap(img_bullet);
	al_destroy_bitmap(img_bullet2);
	al_destroy_bitmap(img_bullet3);
	al_destroy_bitmap(img_can);
	al_destroy_bitmap(img_heart);
	
	// others
	al_destroy_timer(game_update_timer);
	al_destroy_event_queue(game_event_queue);
	al_destroy_display(game_display);
	free(mouse_state);
}

void game_change_scene(int next_scene) {
	game_log("Change scene from %d to %d", active_scene, next_scene);
	// TODO: Destroy resources initialized when creating scene.
	if (active_scene == SCENE_MENU) {
		al_stop_sample(&main_bgm_id);
		game_log("stop audio (bgm)");
	} 
	else if (active_scene == SCENE_START) {
		al_stop_sample(&start_bgm_id);
		game_log("stop audio (bgm)");
	}
	active_scene = next_scene;
	// TODO: Allocate resources before entering scene.
	if (active_scene == SCENE_MENU) {
		/* if(_bgm != 1){
			if (!al_play_sample(main_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &main_bgm_id))
			game_abort("failed to play audio (bgm)");
		} */
		if(_bgm == 0){
			al_play_sample(main_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &main_bgm_id);
		}
		if(_bgm == 1){
			al_stop_sample(&main_bgm_id);
		}

	} 
	else if (active_scene == SCENE_START) {
		int i,c;
		
		// main plane
		if(character_idx == 0)
			plane.img = start_img_plane;
		else if(character_idx == 2)
			plane.img = start_img_plane2;
		else if(character_idx == 3)
			plane.img = start_img_plane3;
		plane.x = plane.w  + (float)rand() / RAND_MAX * (SCREEN_W - plane.w);
		plane.y = 850;
		plane.w = al_get_bitmap_width(plane.img);
        plane.h = al_get_bitmap_height(plane.img);

		// 2p
		
		if(_p_mode == 1){
			if(character_idx == 0 )
				plane_partner.img = start_img_plane_partner;
			else if(character_idx == 2)
				plane_partner.img = start_img_plane2_partner;
			else if(character_idx == 3)
				plane_partner.img = start_img_plane3_partner;
			plane_partner.x = plane_partner.w  + (float)rand() / RAND_MAX * (SCREEN_W - plane_partner.w);
			plane_partner.y = 850;
			plane_partner.w = al_get_bitmap_width(plane.img);
			plane_partner.h = al_get_bitmap_height(plane.img);
		}
		
	
		for (i = 0; i < MAX_ENEMY; i++) {
			enemies[i].img = start_img_enemy;
			enemies[i].w = al_get_bitmap_width(start_img_enemy);
			enemies[i].h = al_get_bitmap_height(start_img_enemy);
			enemies[i].vx = 0;
			enemies[i].vy = enemy_speed;
			enemies[i].hidden = false;
	
			enemies[i].x = enemies[i].w  + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
			enemies[i].y = enemies[i].h  + (float)rand() / RAND_MAX * (SCREEN_H/4 - enemies[i].h);	
		}
		for (c = 0; c < MAX_CAN; c++) {
			can[c].img = img_can;
			can[c].w = al_get_bitmap_width(img_can);
			can[c].h = al_get_bitmap_height(img_can);
			can[c].vx = 0;
			can[c].vy = 10;
			can[c].hidden = false;
	
			can[c].x = 2 * can[c].w  + (float)rand() / RAND_MAX * (SCREEN_W - can[c].w);
			can[c].y = can[c].h  + (float)rand() / RAND_MAX * (SCREEN_H/4 - can[c].h);
			
		}

		for (i = 0; i < MAX_BULLET; i ++) {
			bullets[i].w = al_get_bitmap_width(img_bullet);
			bullets[i].h = al_get_bitmap_height(img_bullet);
			if(character_idx == 0)
				bullets[i].img = img_bullet;
			else if(character_idx == 2)
				bullets[i].img = img_bullet2;
			else if(character_idx == 3)
				bullets[i].img = img_bullet3;
			bullets[i].vx = 0;
			bullets[i].vy = -3 ;
			bullets[i].hidden = true;
		}
		
		for (i = 0; i < MAX_CAN ; i++) {
			bigbullet[i].img = start_img_bigbullet;
			bigbullet[i].w = al_get_bitmap_width(start_img_bigbullet);
			bigbullet[i].h = al_get_bitmap_height(start_img_bigbullet);
			bigbullet[i].hidden = false;
			bigbullet[i].vx = 0;
			bigbullet[i].vy = +1;
	
			bigbullet[i].x = bigbullet[i].w  + (float)rand() / RAND_MAX * (SCREEN_W - bigbullet[i].w);
			bigbullet[i].y = bigbullet[i].h  + (float)rand() / RAND_MAX * (SCREEN_H/4 - bigbullet[i].h);	
		}
		
		if(_bgm == 0){
			al_play_sample(start_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &start_bgm_id);
			al_stop_sample(&main_bgm_id);
		}
		if(_bgm == 1){
			al_stop_sample(&start_bgm_id);
			al_stop_sample(&main_bgm_id);
		}

	}
}

void on_key_down(int keycode) {
	if (active_scene == SCENE_MENU) {
		if (keycode == ALLEGRO_KEY_ENTER )
			game_change_scene(SCENE_START);
	}
	else if (active_scene == SCENE_START) {
		if (keycode == ALLEGRO_KEY_ESCAPE)
			game_change_scene(SCENE_END);
	}
	else if (active_scene == SCENE_SETTINGS) {
		if (keycode == ALLEGRO_KEY_ENTER)
			game_change_scene(SCENE_START);
	}
	else if(active_scene == SCENE_MENU || active_scene == SCENE_END || active_scene == SCENE_MENU) {
		if (keycode == ALLEGRO_KEY_ESCAPE){
			game_log("Game end");
			game_destroy();
		}
	}
}


void on_mouse_down(int btn, int x, int y) {
	
	if (btn == 1){
		if (active_scene == SCENE_MENU) {
			if (pnt_in_rect(x, y, SCREEN_W - 48, 10, 38, 38))
				game_change_scene(SCENE_SETTINGS);
			
			else if (pnt_in_rect(x, y, 150,710,100,100))
				game_change_scene(SCENE_START);

			else if (pnt_in_rect(x, y, 300, 710,100,100))
				game_change_scene(SCENE_TUTORIAL);

			else if (pnt_in_rect(x, y, 450, 710,100,100)){
				game_log("Game end");
				game_destroy();
			}
	}
		else if(active_scene == SCENE_TUTORIAL){
			if (pnt_in_rect(x, y, 50, SCREEN_H/2, 100, 100))
				game_change_scene(SCENE_MENU);

			if(pnt_in_rect(x, y, SCREEN_W - 150, SCREEN_H/2, 100,100))
				game_change_scene(SCENE_START);
		}
		
		else if(active_scene == SCENE_TUTORIAL2){
			if (pnt_in_rect(x, y, SCREEN_W -150, SCREEN_H/2, 100, 100))
				game_change_scene(SCENE_START);
			if(pnt_in_rect(x,y,50, SCREEN_H/2, 100, 100))
				game_change_scene(SCENE_SETTINGS);
		}

		else if(active_scene == SCENE_SETTINGS){
			// backpage
			if (pnt_in_rect(x, y, 50, SCREEN_H/2, 100, 100))
				game_change_scene(SCENE_MENU);
			// start sign
			if(pnt_in_rect(mouse_x, mouse_y, SCREEN_W /2 - 240, 780, 100, 100))
			game_change_scene(SCENE_START);

			// p_mode
			if(pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 -150, 350, 60,60))
				_p_mode = 0;
			
			else if(pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 +50, 350, 60,60)){
				_p_mode = 1;
				game_change_scene(SCENE_TUTORIAL2);
			}
			else if(pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 +100, 350, 60,60)){
				_p_mode = 1;
				game_change_scene(SCENE_TUTORIAL2);
			}
			
			// mute
			if(pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2-100, 510, 70,70))
				_bgm = 1;

			// on
			else if(pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2+30, 510, 70,70))
				_bgm = 0;
			
			else if(pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2-150, 710, 60,60))
				character_idx = 0;
			else if(pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2-40, 710, 60,60))
				character_idx = 2;
			else if(pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2+70, 710, 60,60))
				character_idx = 3;
		}
		else if(active_scene == SCENE_END){
			if (pnt_in_rect(x, y, SCREEN_W / 2-50,SCREEN_H/2+100,100,100)){
				game_log("Game end");
				game_destroy();
			}
		}
	}
	
}

void draw_movable_object(MovableObject obj) {
	if (obj.hidden)
		return;
	al_draw_bitmap(obj.img, round(obj.x - obj.w / 2), round(obj.y - obj.h / 2), 0);
}

ALLEGRO_BITMAP *load_bitmap_resized(const char *filename, int w, int h) {
	ALLEGRO_BITMAP* loaded_bmp = al_load_bitmap(filename);
	if (!loaded_bmp)
		game_abort("failed to load image: %s", filename);
	ALLEGRO_BITMAP *resized_bmp = al_create_bitmap(w, h);
	ALLEGRO_BITMAP *prev_target = al_get_target_bitmap();

	if (!resized_bmp)
		game_abort("failed to create bitmap when creating resized image: %s", filename);
	al_set_target_bitmap(resized_bmp);
	al_draw_scaled_bitmap(loaded_bmp, 0, 0,
		al_get_bitmap_width(loaded_bmp),
		al_get_bitmap_height(loaded_bmp),
		0, 0, w, h, 0);
	al_set_target_bitmap(prev_target);
	al_destroy_bitmap(loaded_bmp);
    //printf("resized image: %s", filename);
    //game_log("resized image: %s", filename);

	return resized_bmp;
}

// [HACKATHON 3-3]
// TODO: Define bool pnt_in_rect(int px, int py, int x, int y, int w, int h)
// Uncomment and fill in the code below.
bool pnt_in_rect(int px, int py, int x, int y, int w, int h) {
    if(px >= x && px <= x+w && py >= y && py <= y+h)
        return true;
    else
        return false;
}


// +=================================================================+
// | Code below is for debugging purpose, it's fine to remove it.    |
// | Deleting the code below and removing all calls to the functions |
// | doesn't affect the game.                                        |
// +=================================================================+

void game_abort(const char* format, ...) {
	va_list arg;
	va_start(arg, format);
	game_vlog(format, arg);
	va_end(arg);
	fprintf(stderr, "error occured, exiting after 2 secs");
	// Wait 2 secs before exiting.
	al_rest(2);
	// Force exit program.
	exit(1);
}

void game_log(const char* format, ...) {
#ifdef LOG_ENABLED
	va_list arg;
	va_start(arg, format);
	game_vlog(format, arg);
	va_end(arg);
#endif
}

void game_vlog(const char* format, va_list arg) {
#ifdef LOG_ENABLED
	static bool clear_file = true;
	vprintf(format, arg);
	printf("\n");
	// Write log to file for later debugging.
	FILE* pFile = fopen("log.txt", clear_file ? "w" : "a");
	if (pFile) {
		vfprintf(pFile, format, arg);
		fprintf(pFile, "\n");
		fclose(pFile);
	}
	clear_file = false;
#endif
}
