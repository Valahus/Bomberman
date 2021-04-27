Projet BomberMan

Guillaume Tong
Ionut-Mihai Bosiica 


*************************************************************************************************************************************************************
*************************************************************************************************************************************************************
                                                        FUNCTIONS COMPOSING OUR PROJECT:
*************************************************************************************************************************************************************
*************************************************************************************************************************************************************



*************************************************************************************************************************************************************
*************************************************************************************************************************************************************

in term.c:



unsigned char file_type()
    checks the file type and sets it to either:FILE_UNK 0 (unkown file type), FILE_DIR 1 (directory) or FILE_REG 2 (regular file);
    *param:struct dirent*               dir         directory which can be set to FILE_UNK, FILE_DIR, FILE_REG  


    
void dispose_term()
    clears the screen, makes the cursor visible, and set the current termios back to the old one;
    
    
void end_game()
    sets the running state to 0, exit the game and cleans the terminal;
    *param:void         (*clean)(void)
    
    
void start_game()
    starts the game "scene" unsing the provided function pointers for handling the required "events"
        used to switch between different "scenes" e.g the mod selection menu, the main menu and game levels...etc;
    *param:(*init)      (void)
    *param:void         (*gameloop)()
    *param:void         (*input_proc)(int)
    

void init_term()
    sets the new termios terminal;
    
    
int getWinHeight()
    returns ws_col of linux implemented structure winsize;
    
    
int getWinWidth()
    returns ws_row of linux implemented structure winsize;
    
    
    
*************************************************************************************************************************************************************
*************************************************************************************************************************************************************



*************************************************************************************************************************************************************
*************************************************************************************************************************************************************



in timeutil.c


ms_t
    is actually a 'unsigned long long' used to store time in miliseconds;

    
ms_t gettom()
    gets the current time in miliseconds;

    
ms_t ftoms() 
    conversion from minutes (float) to miliseconds (ms_t);
    *param:float                f           minutes


    
*************************************************************************************************************************************************************
*************************************************************************************************************************************************************
    
    
    
*************************************************************************************************************************************************************
*************************************************************************************************************************************************************



in linkstack.c

basic functions and struct node related to lists ( push, pop, create, etc ) - used for bombs mostly;

typedef struct node;
node* create_node();
node* push_node();
node* pop_node();
void iterate();
void delete_list();
int is_empty();


*************************************************************************************************************************************************************
*************************************************************************************************************************************************************




*************************************************************************************************************************************************************
*************************************************************************************************************************************************************
    
    
    
in bombs.c

int cntlvls()
    count loaded levels;
    


int cnttitles()
    count loaded titles;
    
    
void load()
    checks the integrity of the game ( mods/levels) and "prepares" the game by loading      the indicated mods/levels;
    *param:char*        path
    
    
void load_lvl()
    loads the selected level, reads the powerup lines and hides it;
    *param:char*        path
    
    
void load_der()
    loads the level list from deroulement file;
    
    
void free_der()
    frees the level titles;


void free_levels()
    frees the loaded levels;

    
void free_resources()
    frees titles + levels;
    

void free_level()
    frees the currently selected level;
    *param:level*       level
    
    
level* getlvl()
    gets the selected level and positions it in the middle of the screen; 
    param:int       index
    
    
char* get_title()
    returns level's title;
    
    
char* get_lvl_title()
    returns level's title;
    param:int       index
    
    
player_struct* createPlayer()
    initialise a player structure;
    param:char      sym             symbol for each player
    
    
player_struct* createPlayer1 && createPlayer2
    creates the 2 participants of the game;
    
    
int collide_map()
    function gets the player position on the map and checks for collision inside the level and  returns true if position(x,y) collides with a wall;
    param:const level*      lvl
    param:int               x
    param:int               y
    

void pick_power()
    picks powerups from the map;
    param:level*            lvl
    param:player_struct*    p
    
    
int explode_map()
    changes the tile in the level where the explosion occured;
    param:level*            lvl
    param:int               x
    param:int               y
    
    
void move()
    function that allows players to move;
    param:player_struct*    p
    param:vec2              dir
    
    
void print_player()
    prints the player;
    param:player_struct     o
    

void print_bomb()
    prints bombs;
    param:bomb              b
    
    
void print_lvl()
    prints the level and resets the explosion marks if occured;
    param:const level*      lvl
    
    
void create_buffer
    creates buffer;
    param:int               width
    param:int               height
    
    
void flush_buffer()
    flushes the buffer and also sets the explosion marks to red;
    param:int               x
    param:int               y
    
    
void free_buffer()
    frees the buffer;
    
vec2 set_player_posA(player_struct* p)
    sets player1 at the begining of the grid;
    param:player_struct*    p
    
    
vec2 set_player_posB()
    sets player at the end of the grid;
    param:level*            cur_lvl
    param:player_struct*    p
    

*************************************************************************************************************************************************************
*************************************************************************************************************************************************************




*************************************************************************************************************************************************************
*************************************************************************************************************************************************************
    
    
    
in main.c


void mod_init()
    clears screen,looks for mod directory, reads the number and name of loaded mods from directory and shows the mods ascii art nicely on the screen;


void mod_loop()
    highlights the mod the player picks;


void mod_input(int k)
    allows the player to change between mods,selects mod and exits game;


void mod_clean()
    frees the mods;



void game_init()
    Function gets the width, height and creates a buffer to stock "the game"movement 
speed, bombs, players are all initialised and makes a border of # with the specified hight and width;

    
    
void game_loop()
    keeps the game constaly updated, prints the resources and also a board with the current score,powerups, lifes etc.;



void game_input()
    allows the players to move their characters and place bombs;
    param:int       k
    
    
void game_clean()
    frees the players and deletes the bomb lists ;


void menu_init()
    gets the mod title "Ascii bomberman" and prints it on the screen;


void menu_loop()
    gets the lvl count and titles and prints them on the screen;



void menu_input(int k)
    allows the user to navigate between levels;
    param:int           k


void msg_init()
        Creates a # border on the terminal and positions win/lose messages;



void msg_input(int k)
    if a match is completed, function either goes to the next level,cleans the game and stars the new match or goes back to menu;
    param:int           k
    
    
void goto_mod()
    starts the game in the mod selection screen;



void goto_menu()
    clears resources and intitiate the menu loops;


void goto_msg()
    gets message ;


void goto_game()
    starts the game with the selected level;

    
    
int main();

void boom(int x, int y, int dirX, int dirY, int area)
    explodes the bomb in a + form and decreases player life + explode map;
    param:int           x
    param:int           y
    param:int           dirX
    param:int           dirY
    param:int           area


void update_bombs()
    sets list of bombs, calculates the timer and explodes the bomb;



void update_player()
    picks powerups for player if its the case, refils bombs and checks for colisions;
    param:player_struct*        player


void update()
    calculates delta time -"movement time";


void P_bomb()
    prints bomb;
    param:node*         n


void put_bomb()
    places the bomb at the respective position with a determinated radius;
    param:int           x
    param:int           y
    param:int           radius

    
int is_mod()
    tests if curent directory is a valid mod and returns 0 if so;
    param:char*         path


*************************************************************************************************************************************************************
*************************************************************************************************************************************************************


    
    
    
    




    
