#include "term.h"
#include "linkstack.h"
#include "timeutil.h"
/*
 *Default defines
 */
//default move speed
#define MOVE_SPEED 300
//default wait time before adding a new bomb
#define SHOOT_T 250
//default explosion timeout
#define BOMB_TIME 1550
//default bomb hit-points
#define BOMB_HIT 1
//default bomb explosion area
#define BOMB_AREA 3


#define STAT_WON 1
#define STAT_LOSE 2

#define WIN_MSG "Player 1 Won!"
#define LOSE_MSG "Player 2 Won!"
#define MSG_COLOR "\e[7m"

#define MAX_MODS 50

//mode functions
void mod_init();
void mod_loop();
void mod_input(int k);
void mod_clean();
//game functions
void game_init();
void game_loop();
void game_input(int k);
void game_clean();
//menu functions
void menu_init();
void menu_loop();
void menu_input(int k);
//message screen functions
void msg_init();
void msg_input(int k);

int width = 10;//default term size
int height = 10;

node* bombs = NULL;

player_struct* player_1;//player 1
player_struct* player_2;// player 2

ms_t s_freq = SHOOT_T;//player shooting frequency
ms_t m_timeout;
int m_speed = MOVE_SPEED;//player move speed

level* cur_lvl;
int sel_lvl = 0;


 vec2 left={-1,0};
 vec2 right={1,0};
 vec2 up={0,-1};
 vec2 down={0,1};


typedef struct {
	ms_t start;
	ms_t passed;
} dtime;

dtime delta_time;

ms_t last_shoot;

ms_t dtrace;//delta time for traces/shooting stuff

int game_status;

void goto_mod(){
  start_game(&mod_init, &mod_loop, &mod_input);
  
}

void goto_menu(){
  end_game(&game_clean);
  free_level(cur_lvl);
  start_game(&menu_init, &menu_loop, &menu_input);
}

void goto_msg(){
  start_game(&msg_init, NULL, &msg_input);
}

void goto_game(){
  cur_lvl = getlvl(sel_lvl);
  start_game(&game_init, &game_loop, &game_input);
}

int main(){
  init_term();
  //first create buffer
  create_buffer(getWinWidth(), getWinHeight());
  //set width & height
  width = getWinWidth();
  height = getWinHeight();
  //select mod
  goto_mod();
  	
  dispose_term();
  free_buffer();
  return 0;
}

void game_clean(){
  free(player_1);
  free(player_2);
  delete_list(&bombs);
}

void game_init(){
  game_status = 0;
  width = getWinWidth();
  height = getWinHeight();
  create_buffer(width-2, height);
  
  s_freq = SHOOT_T;//player shooting frequency
  m_speed = MOVE_SPEED;//player move speed
  
   player_1 = createPlayer1();
  player_1->pos=set_player_posA(player_1);
  
  player_2 = createPlayer2();
  player_2->pos=set_player_posB(cur_lvl,player_2);

	
  delta_time.start = gettom();
  delta_time.passed = 0;
  last_shoot = delta_time.start;
  m_timeout = delta_time.start;
  

  int i;

  GOTO(0, 0);
  for(i = 0; i < width; i++)
    printf("%c", '#');
  GOTO(0, height);
  for(i = 0; i < width; i++)
    printf("%c", '#');
  for(int y = 0; y < height; y++){
    GOTO(0, y);
    printf("%c", '#');
    GOTO(width, y);
    printf("%c", '#');
  }

}

void boom(int x, int y, int dirX, int dirY, int area){
  if(area <= 0)
    return;
  if(player_1->pos.x == x && player_1->pos.y == y)
    player_1->hp -= 1;
  if(player_2->pos.x == x && player_2->pos.y == y)
    player_2->hp -= 1;
  if(explode_map(cur_lvl, x, y))
    return;
  boom(x + dirX, y + dirY, dirX, dirY, area-1);
}

void update_bombs(){
  node* tmp = NULL;
  while(!is_empty(bombs)){
    bomb b;
    bombs = pop_node(bombs, &b);
    if(b.time <= delta_time.passed){
      int x = b.pos.x;
      int y = b.pos.y;
      int area = b.area;
      for(int i = -1; i < 2; i++)
	for(int j = -1; j < 2; j++)
	  if(i == 0 || j == 0)
	    boom(x+i, y+j, i, j, area);
    }else{
      b.time -= delta_time.passed;
      if(is_empty(tmp))
	tmp = create_node(NULL, b);
      else
	tmp = push_node(tmp, b);
    }
  }
  bombs = tmp;
}

void update_player(player_struct* player){
  pick_power(cur_lvl, player);

  //refill bombs
  if(player->rtime <= delta_time.passed){
    player->rtime = player->rspeed;
    player->bombs = player->bombs_on_refill;
  }else{
    player->rtime -= delta_time.passed;
  }
  
  if(player->dir.x == 0 && player->dir.y == 0)
    return;
  if(player->timeout <= delta_time.passed){
    int x = player->pos.x + player->dir.x;
    int y = player->pos.y + player->dir.y;
    if(!collide_map(cur_lvl, x, y) && !collide(player, player == player_1 ? player_2 : player_1)){
      player->pos.x = x;
      player->pos.y = y;
    }
    player->timeout = player->speed;
    player->dir.x = 0;
    player->dir.y = 0;
  }else{
    player->timeout -= delta_time.passed;
  }
}


void update(){
  ms_t ctime = gettom();
  delta_time.passed = ctime - delta_time.start;
  delta_time.start = ctime;
  
}

void p_bomb(node* n){
  print_bomb(n->data);
}

void game_loop(){
  update();
  update_player(player_1);
  update_player(player_2);
  update_bombs();
  print_lvl(cur_lvl);
  print_player(*player_1);
  print_player(*player_2);
  iterate(bombs, &p_bomb);
  
  flush_buffer(2, 3);
  GOTO(2, 2);
  
  printf("  PLAYER1 HP:%d +:%lld @:%d *:%d  |  PLAYER2 HP:%d +:%lld @:%d *:%d    test %d  ",player_1->hp, 100-player_1->speed, player_1->bombs, player_1->radius, player_2->hp, 100-player_2->speed, player_2->bombs, player_2->radius,height);
  if(player_1->hp<1)
      game_status=STAT_WON;
  else if(player_2->hp<1)
      game_status=STAT_LOSE;
  if(game_status !=0)
      goto_msg();
  
}

void put_bomb(int x, int y, int radius){
  bomb b;
  b.pos.x = x;
  b.pos.y = y;
  b.area = radius;
  b.time = BOMB_TIME;
  b.sym = '@';
  b.dmg = BOMB_HIT;
  if(is_empty(bombs))
    bombs = create_node(bombs, b);
  else
    bombs = push_node(bombs, b);
}

void game_input(int k){
  if(player_1->hp > 0){
    switch(k){
    case K_A:
      move(player_1,left);
      break;
    case K_S:
      move(player_1,down);
      break;
    case K_D:
      move(player_1,right);
      break;
    case K_W:
      move(player_1,up);
      break;
    case K_SPACE:
      if(player_1->bombs > 0){
	put_bomb(player_1->pos.x, player_1->
		 pos.y, player_1->radius);
	player_1->bombs--;
      }
    }
  }
  if(player_2->hp > 0){
    switch(k){
    case K_LEFT:
      move(player_2,left);
      break;
    case K_DOWN:
      move(player_2,down);
      break;
    case K_RIGHT:
      move(player_2,right);
      break;
    case K_UP:
      move(player_2,up);
      break;
    case 'e':
      if(player_2->bombs > 0){
	put_bomb(player_2->pos.x, player_2->
	       pos.y, player_2->radius);
	player_2->bombs--;
      }
    }
  }
  if(k == 'q' || k == 'Q')
    goto_menu();
}

void menu_init(){
  GOTO(width/2 - (int)strlen(get_title())/2, 4);
  fprintf(stdout, "\e[37m\e[41m%s\033[0m", get_title());
}

void menu_loop(){
  
  for(int i = 0; i < cnttitles(); i++)
    if(get_lvl_title(i)){
      GOTO(width/2 - (int)strlen(get_lvl_title(i)+2)/2, 12 + (i*2));
      if(sel_lvl == i)
	fprintf(stdout, COLOR_INV);
      fprintf(stdout, " %s \033[0m ", get_lvl_title(i));
      fflush(stdout);
    }
  fflush(stdout);
}

void menu_input(int k){
  switch(k){
  case K_DOWN:
  case K_S:
    if(sel_lvl < cntlvls()-1)
      sel_lvl++;
    break;
  case K_UP:
  case K_W:
    if(sel_lvl > 0)
      sel_lvl--;
    break;
  case K_SPACE:
  case K_RET:
    goto_game();
    break;
  case K_ESC:
  case K_Q:
  case 'Q':
    end_game(NULL);
    break;
  }
}

void msg_init(){
  CLS();
  fflush(stdout);
  char * buff = malloc(width);
  memset(buff, '#', width);
  GOTO(0, height/2 - 6);
  write(STDOUT_FILENO, MSG_COLOR, strlen(MSG_COLOR));
  fflush(stdout);
  write(STDOUT_FILENO, buff, width);
  memset(buff, ' ', width);
  buff[0] = '#';
  buff[width-1] = '#';
  for(int i = 1; i < 6; i++){
    GOTO(0, height/2 - i);
    fflush(stdout);
    write(STDOUT_FILENO, buff, width);
    fflush(stdout);
  }
  if(game_status == STAT_WON){
    GOTO(width/2-(int)strlen(WIN_MSG)/2, height/2-4);
    fflush(stdout);
    write(STDOUT_FILENO, WIN_MSG, strlen(WIN_MSG));
    GOTO(width/2-3, height/2-2);
    fflush(stdout);
    write(STDOUT_FILENO, COLOR_NRM, strlen(COLOR_NRM));
    write(STDOUT_FILENO, " next ", 6);
    write(STDOUT_FILENO, MSG_COLOR, strlen(MSG_COLOR));
  }else{
    GOTO(width/2-(int)strlen(LOSE_MSG)/2, height/2-4);
    fflush(stdout);
    write(STDOUT_FILENO, LOSE_MSG, strlen(LOSE_MSG));
    GOTO(width/2-2, height/2-2);
    fflush(stdout);
    write(STDOUT_FILENO, COLOR_NRM, strlen(COLOR_NRM));
    write(STDOUT_FILENO, " ok ", 4);
    write(STDOUT_FILENO, MSG_COLOR, strlen(MSG_COLOR));
  }
  memset(buff, '#', width);
  GOTO(0, height/2);
  fflush(stdout);
  write(STDOUT_FILENO, buff, width);
  write(STDOUT_FILENO, COLOR_NRM, strlen(COLOR_NRM));
  fflush(stdout);
  free(buff);
}

void msg_input(int k){
  if(k == K_RET){
    if(game_status == STAT_WON && sel_lvl < cnttitles()-1){
      sel_lvl++;
      end_game(&game_clean);
      free_level(cur_lvl);
      goto_game();
    }else{
      goto_menu();
    }
  }
  if(k == 'q' || k == 'Q'){
    goto_menu();
  }
}
//mod selection screen

int is_mod(char* path){
  char buff[4096];
  getcwd(buff, 4096);
  chdir(path);
  int valid_mod = 0;
  int fd;
  if((fd = open(MOD_DER_PATH, O_RDONLY)) != -1){
    valid_mod = 1;
    close(fd);
  }
  chdir(buff);
  return valid_mod;
}

char** mods;//mods names (also used to change path when loading mod)
int sel_mod;//current selected mod
int n_mods;//mods count

void mod_init(){
  CLS();
  DIR* dir = opendir("./");
  struct dirent *entry;
  mods = malloc(sizeof(char*) * MAX_MODS);
  n_mods = 0;
  if(dir){
    while((entry= readdir(dir))){
      if(file_type(entry) == FILE_DIR && is_mod(entry->d_name)){
	mods[n_mods] = malloc(sizeof(entry->d_name));
	strcpy(mods[n_mods++], entry->d_name);
      }
    }
  }
  closedir(dir);
  mods = realloc(mods, sizeof(char*) * n_mods);
  GOTO(width/2-22, 3);
  fflush(stdout);
  fprintf(stdout, "%s%s%s", COLOR_BUL, ".___  ___.   ______    _______       _______.", COLOR_NRM);
  GOTO(width/2-22, 4);
  fflush(stdout);
  fprintf(stdout, "%s%s%s", COLOR_BUL, "|   \\/   |  /  __  \\  |       \\     /       |", COLOR_NRM);
  GOTO(width/2-22, 5);
  fflush(stdout);
  fprintf(stdout, "%s%s%s", COLOR_BUL, "|  \\  /  | |  |  |  | |  .--.  |   |   (----`", COLOR_NRM);
  GOTO(width/2-22, 6);
  fflush(stdout);
  fprintf(stdout, "%s%s%s", COLOR_BUL, "|  |\\/|  | |  |  |  | |  |  |  |    \\   \\    ", COLOR_NRM);
  GOTO(width/2-22, 7);
  fflush(stdout);
  fprintf(stdout, "%s%s%s", COLOR_BUL, "|  |  |  | |  `--'  | |  '--'  |.----)   |   ", COLOR_NRM);
  GOTO(width/2-22, 8);
  fflush(stdout);
  fprintf(stdout, "%s%s%s", COLOR_BUL, "|__|  |__|  \\______/  |_______/ |_______/    ", COLOR_NRM);
}

void mod_loop(){
  int y = 15;
  for(int i = 0; i < n_mods; i++){
    GOTO(width/2 - (int)strlen(mods[i])/2, y+=2);
    fflush(stdout);
    fprintf(stdout, "%s%s%s", sel_mod == i ? COLOR_INV : COLOR_NRM, mods[i], COLOR_NRM);
  }
  fflush(stdout);
}

void mod_input(int k){
  if(k == K_UP || k == K_W){
    if(sel_mod > 0)
      sel_mod--;
  }
  if(k == K_DOWN || k == K_S){
    if(sel_mod < n_mods-1)
      sel_mod++;
  }
  if(k == K_RET || k == K_SPACE){
    //menu
    load(mods[sel_mod]);
    end_game(mod_clean);
    goto_menu();
    free_resources();
  }else if(k == K_Q || k == K_ESC){
    end_game(mod_clean);
  }
}

void mod_clean(){
  for(int i = 0; i < n_mods; i++)
    free(mods[i]);
  free(mods);
  n_mods = 0;
}
