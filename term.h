#ifndef __TERM__
#define __TERM__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

/*
 *Key defines
 */

#define K_A	    97
#define K_D	    100
#define K_S	    115
#define K_W	    119
#define K_LEFT	68
#define K_RIGHT 67
#define K_UP	65
#define K_DOWN	66
#define K_SPACE	' '
#define K_E     101
#define K_RET   '\r'
#define K_ESC   27
#define K_Q     113


/*
 * Key defines french layout ( if needed )
 * 
#define K_A	    113
#define K_D	    100
#define K_S	    115
#define K_W	    122
#define K_LEFT	68
#define K_RIGHT 67
#define K_UP	65
#define K_DOWN	66
#define K_SPACE	' '
#define K_E     101
#define K_RET   '\r'
#define K_ESC   27
#define K_Q     97
*/

//file types:
#define FILE_UNK 0 //unkown file type
#define FILE_DIR 1 //directory
#define FILE_REG 2 //regular file

//hide cursor
#define HIDEC() printf("\x1b[?25l");

//show cursor
#define SHOWC() printf("\x1b[?25h");

//clean screen terminal
#define CLS()   printf("\033[2J");

//coordinates of how to write on a certain position
#define GOTO(x,y)   printf("\033[%d;%dH",(y),(x));

void start_game(void (*init)(void), void (*gameloop)(void), void (*input_proc)(int));
void end_game();
void init_term();
void dispose_term();
int getWinHeight();
int getWinWidth();
//portable way to check file type
unsigned char file_type(struct dirent*);

#endif 
