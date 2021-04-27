#ifndef __SHIPS__
#define __SHIPS__

#include "timeutil.h"

//colors
#define COLOR_NRM "\x1B[0m"
#define COLOR_RED "\x1B[31m"
#define COLOR_GRN "\x1B[32m"
#define COLOR_YEL "\x1B[33m"
#define COLOR_BLU "\x1B[34m"
#define COLOR_INV "\e[7m"
#define COLOR_BUL "\e[1m"
//resources paths
#define MOD_DER_PATH "./deroulement"
#define SHIPS_PATH "./vaisseaux/"
#define LVLS_PATH "./niveaux/"




typedef struct {
  int x, y;
} vec2;



typedef struct {
  int hp;
  vec2 pos;// position
  vec2 dir;// direction of movement
  char symbol;
  ms_t speed;// speed
  ms_t timeout;// used to see when to move
  int bombs;// how many bombs
  int bombs_on_refill;
  int radius; //explosion radius
  ms_t rspeed; //refill speed
  int rtime;// refill time remaining
} player_struct;


typedef struct {
  vec2 pos; //position
  int area; //explosion area (specifies width and height of the explosion)
  int dmg; //damage caused by the explosion
  ms_t time; //time to wait before exploding
  char sym; //just in case they should look different
} bomb;

typedef struct {
  int h, l;
  char** map;
  char** powerups;
} level;

/*
 *load resources functions
 */
void load();//loads all resources

void load_lvl(char[]);//loads levels from path (parameter = path)
void load_der();

/*
 *free resources functions
 */
void free_resources();

void free_levels();
void free_der();
/*
 *free functions
 */
void free_level(level *);

void free_player_struct(player_struct*);

int cntlvls();//counts current loaded levels
int cnttitles();//count menu titles


level* getlvl(int);


/*
 *player_struct creation functions
 */
player_struct* createPlayer(char sym);
player_struct* createPlayer1();
player_struct* createPlayer2();

/*
 *printing functions
 */
void print_player(player_struct);
void print_bomb(bomb);
void print_lvl(const level*);
/*
void print_lvl(level*);
*/

/*
 *buffer functions
 */
void create_buffer(int, int);
void flush_buffer(int, int);
void free_buffer();

/*
 *collision functions
 */

int collide_map(const level* lvl, int x, int y); //Returns true if position(x, y) collides with a wall in level


void pick_power(level* lvl, player_struct* p); //Pick powerups from map
int explode_map(level* lvl, int x, int y); //Changes the tile in the level  wheere the explosion occured


void move(player_struct*p,vec2 dir);

/*
 *info funct
 */
char* get_title();
char* get_lvl_title(int);

vec2 set_player_posA(player_struct* p);
vec2 set_player_posB(level* cur_lvl,player_struct* p);

int collide(player_struct* p1, player_struct* p2);
#endif
