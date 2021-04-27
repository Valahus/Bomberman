#include "bombs.h"
#include "term.h"


#define SHIP_HP 100
//PRT_RESET = colour off 
#define PRT_RESET "\033[0m" 
#define PRT_RED "\033[31m"
#define PRT_CYAN "\033[36m"

#define P1_DISPLAY 'N'
#define P2_DISPLAY 'H'


/*
 *print buffer global vars
 */
char **print_buffer;
int b_width, b_height;

/*
 *ships and levels
 */
level** levels;

int lvl_count;
int titles_count;

/*
 *deroulement mod info (title, and levels)
 */
char *title;
char **lvl_titles;
int *map;

int offsetY, offsetX;//level offset, used to center the level

int cntlvls(){
  return lvl_count;
}

int cnttitles(){
  return titles_count;
}

void load(char *path){
  lvl_count = 0;
  char fnames[100][2];//file names array max lenght = 2 (ex: 00, 01, 02...99) => max 100 files
  int ns = 0;//names stored
  if(chdir(path) == -1){
    fprintf(stderr, "Failed to change directory to selected mod: %s", path);
    exit(1);
  }
  DIR *dir;
  struct dirent *entry;
  
  ns = 0;
  dir = opendir(LVLS_PATH);
  if(dir){
    while((entry = readdir(dir)))
      if(entry->d_type == DT_REG){
  if(ns == 99){
    perror("Too many levels added to current mod used (max: 100 supported)");
    exit(1);
  }
  if(strlen(entry->d_name) > 2){
    perror("Filename is too long, max supported filename lenght is 2");
    exit(1);
  }
  strncpy(fnames[ns], entry->d_name, 2);
  ns++;
      }
    closedir(dir);
    levels = malloc(sizeof(level) * ns);
    int offset = sizeof(LVLS_PATH);
    char* path = malloc(offset+2);
    strncpy(path, LVLS_PATH, offset);
    for(int i = ns-1; i > -1; i--){
      strncpy(path+(offset-1), fnames[i], 2);
      strncpy(path+(offset+1), "\0", 1);
      lvl_count++;
      load_lvl(path);
    }
    free(path);
  }else{
    perror("Could not open levels directory in current mod");
    exit(1);
  }
  load_der();
}

void load_lvl(char* path){
  FILE* f = fopen(path, "r");
  int index = atoi(path + (strlen(path)-2));
  if(!f){
    fprintf(stderr, "Could not open level file: %s ", path);
    exit(1);
  }
  level* lvl = (level*)malloc(sizeof(level));
  int r = fscanf(f, "%d %d\n", &lvl->h, &lvl->l);
  if(r != 2){
    perror("Wrong file format used to specify a level");
    exit(1);
  }
  char** map = (char**)malloc(sizeof(char*) * lvl->h);
  char c = '\n';
  for(int i = 0; i < lvl->h; i++){
    map[i] = (char*)malloc(lvl->l);
    for(int j = 0; j < lvl->l; j++){
      if(c == '\n')
  r = fscanf(f, "%c", &c);
      if(c == '\n'){
  if(lvl->l-j > 0)
    memset(&map[i][j], ' ', lvl->l-j);
  break;
      }
      map[i][j] = c;
      c = '\n';
      if(r != 1){
  fprintf(stderr, "Error occured when trying to read character from map section in file: %s", path);
  exit(1);
      }
    }
    fscanf(f, "%c", &c);
  }
  char** powerups = (char**)malloc(sizeof(char*) * lvl->h);
  for(int i = 0; i < lvl->h; i++){
    powerups[i] = (char*)malloc(lvl->l);
    for(int j = 0; j < lvl->l; j++){
      if(c == '\n')
  r = fscanf(f, "%c", &c);
      if(c == '\n'){
  if(lvl->l-j > 0){
    memset(&powerups[i][j], ' ', lvl->l-j);
    break;
  }
      }
      powerups[i][j] = c;
      c = '\n';
      if(r != 1){
  fprintf(stderr, "Error occured when trying to read character from powerups section in file: %s", path);
  exit(1);
      }
    }
    fscanf(f, "%c", &c);
  }
  lvl->map = map;
  lvl->powerups = powerups;
  fclose(f);
  levels[index] = lvl;
}

void load_der(){
  FILE* f = fopen(MOD_DER_PATH, "r");
  char buff[1024];
  fscanf(f, "%[^\n]\n", buff);
  title = malloc(strlen(buff)+1);//+1 for string null terminator '\0'
  strcpy(title, buff);
  lvl_titles = malloc(100 * sizeof(char*));
  map = malloc(100 * sizeof(int));
  int rd = 1;
  int index;
  for(int i = 0; rd != EOF ; i++){
    rd = fscanf(f, "%d", &index); 
    rd = fscanf(f, "%[^\n]\n", buff);
    if(rd == EOF)
      break;
    lvl_titles[i] = malloc(strlen(buff)+1);//+1 for string null terminator '\0'
    strcpy(lvl_titles[i], buff);
    map[i] = index;
    titles_count++;
  }
  lvl_titles = realloc(lvl_titles, sizeof(char*) * titles_count);
  map = realloc(map, sizeof(int) * titles_count);
  fclose(f);
}


void free_resources(){
  free_der();
  free_levels();
 
}


void free_levels(){
  for(int i = 0; i < cntlvls(); i++)
    free_level(levels[i]);
  free(levels);
}

void free_der(){
  free(title);
  for(int i = 0; i < titles_count; i++){
    free(lvl_titles[i]);
  }
  free(lvl_titles);
  free(map);
}


void free_level(level* l){
  if(!l)
    return;
  // TODO
  free(l);
}

level* getlvl(int index){
  index = map[index];
  level* lvl = malloc(sizeof(level));
  memcpy(lvl, levels[index], sizeof(level));
  offsetY = (b_height/2) - (lvl->h/2);
  offsetX = (b_width/2) - (lvl->l/2);
  
 return lvl;
}
vec2 set_player_posA(player_struct* p){
  p->pos.x=offsetX+1;
  p->pos.y=offsetY+1;
  return p->pos;
}

vec2 set_player_posB(level* cur_lvl,player_struct* p){
  p->pos.x=(offsetX+cur_lvl->l-3);
  p->pos.y=(offsetY+cur_lvl->h-2);
  return p->pos;
}
char* get_title(){
  return title;
}

char* get_lvl_title(int index){
  return lvl_titles[index];
}
                         

player_struct* createPlayer(char sym){
  player_struct* ship = malloc(sizeof(player_struct));
  ship->symbol = sym;
  ship->hp = SHIP_HP;
  ship->pos.x = 0;
  ship->pos.y = 0;
  ship->dir.x = 0;
  ship->dir.y = 0;
  ship->speed = 100;
  ship->timeout = 0;
  ship->rspeed = 3000;
  ship->rtime = 0;
  ship->bombs = 0;
  ship->bombs_on_refill = 1;
  ship->radius = 3;
  return ship;
}

player_struct* createPlayer1(){
  return createPlayer(P1_DISPLAY);
}

player_struct* createPlayer2(){
  return createPlayer(P2_DISPLAY);
}

int collide_map(const level* lvl, int x, int y){
  //get player position in map
  int mapX = x - offsetX;
  int mapY = y - offsetY;
  //If player position is inside the level check for collision
  if(mapX >= 0 && mapY >= 0 && mapX < lvl->l && mapY < lvl->h)
    return lvl->map[mapY][mapX] != ' ';
  return 0;
}
int collide(player_struct* p1, player_struct* p2){
  if(!p1 || !p2)
    return 0;
  
  if(p1->hp > 0 && p2->hp > 0){
    int posx = p1->pos.x + p1->dir.x;
    int posy = p1->pos.y + p1->dir.y;
    return posx == p2->pos.x && posy == p2->pos.y;
  }
  return 0;
}



void pick_power(level* lvl, player_struct* p){
  int mapX = p->pos.x - offsetX;
  int mapY = p->pos.y - offsetY;
  if(mapX >= 0 && mapY >= 0 && mapX < lvl->l && mapY < lvl->h){
    switch(lvl->powerups[mapY][mapX]){
    case '@':
      p->bombs_on_refill++;
      break;
    case '+':
      p->speed = p->speed > 10 ? p->speed - 10 : 1;
      break;
    case '*':
      p->radius++;
      break;
    }
    lvl->powerups[mapY][mapX] = ' ';
  }
}

int explode_map(level* lvl, int x, int y){
  int mapX = x - offsetX;
  int mapY = y - offsetY;
  if(mapX >= 0 && mapY >= 0 && mapX < lvl->l && mapY < lvl->h){
    if(lvl->map[mapY][mapX] == 'X')
      return 1;
    if(lvl->map[mapY][mapX] == ' '){
      lvl->map[mapY][mapX] = 'X';
    }
    else {
      unsigned int val = lvl->map[mapY][mapX] - '0';
      if(val == 0)
  return 1;
      if(val == 1)
  lvl->map[mapY][mapX] = 'X';
      else
  lvl->map[mapY][mapX] = '0' + (val - 1);
      return 1;
    }
  }
  return 0;
}


void move(player_struct* p,vec2 dir){
p->dir=dir;
 //sets pos.x and pos.y    
}
void print_player(player_struct o){
  if(o.hp <= 0)
    return;
  int y = o.pos.y;
  int x = o.pos.x;
  if(y >= 0 && y < b_height && x >= 0 && x < b_width)
       print_buffer[y][x] = o.symbol;
 // fprintf(stdout, "%s%c%s",PRT_RED, print_buffer[y][x], PRT_CYAN); 
        
    
}

void print_bomb(bomb b){
  if(b.pos.y >= 0 && b.pos.y+1 < b_height && b.pos.x >= 0 && b.pos.x + 1 < b_width)
    memcpy(&print_buffer[b.pos.y][b.pos.x], &b.sym, 1);
}

void print_lvl(const level* lvl){
  int i, j;
  for(i = 0; i < lvl->h; i++)
    for(j = 0; j < lvl->l; j++){
      int posY = offsetY + i;
      int posX = offsetX + j;
      if(posY >= 0 && posY < b_height && posX >= 0 && posX < b_width){
  print_buffer[posY][posX] = lvl->map[i][j];
  if(lvl->map[i][j] == ' ')
    print_buffer[posY][posX] = lvl->powerups[i][j];
      }
      //reset explosion marks
      if(lvl->map[i][j] == 'X')
  lvl->map[i][j] = ' ';
    }
}


void create_buffer(int width, int height){
  free_buffer();
  print_buffer = malloc(sizeof(char*) * height);
  for(int i = 0; i < height; i++){
    print_buffer[i] = malloc(width);
    memset(print_buffer[i], ' ', width);
  }
  b_width = width;
  b_height = height;
}

void flush_buffer(int x, int y){
  for(int i = y; i < b_height; i++){
    GOTO(x, i);
    fflush(stdout);
    write(STDOUT_FILENO, print_buffer[i], b_width);
    for(int j = 0; j < b_width; j++){
      if(print_buffer[i][j] == 'X'){
  GOTO(x+j, i);
  fflush(stdout);
  fprintf(stdout, "%s%c%s",PRT_RED, print_buffer[i][j], PRT_RESET); 
      }
    }
    memset(print_buffer[i], ' ', b_width);
  }
}

void free_buffer(){
  if(!print_buffer)
    return;
  for(int i = 0; i < b_height; i++)
    free(print_buffer[i]);
  free(print_buffer);
  b_height = 0;
  b_width = 0;
}
