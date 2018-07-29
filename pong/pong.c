#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include "helper.h"
#include "ledMatrix.h"
#include "joystick.h"
#include "display.h"
#include "udpclient.h"
#include "udpserver.h"
#include "ledText.h"

#define DELAY 100

typedef struct paddle {
	/* paddle variables */
	int x;
	int y;    /* y is the 'top' of the paddle */
	int len;
} paddle_t;

typedef struct ball {
	/* ball variables */
	int x;
	int y;
	int next_x;
	int next_y;
	int x_vel;
	int y_vel;
} ball_t;

typedef struct dimensions {
	int x;
	int y;
} dimensions_t;

static void draw_ball(ball_t *input);
static void draw_paddle(paddle_t *paddle);
//void draw_usr1_score(paddle_t *inpt_paddle, dimensions_t *wall);
static void paddle_collisions(ball_t *inpt_ball, paddle_t *inpt_paddle, int paddle);
static void paddle_pos(paddle_t *pddl, dimensions_t *wall, int dir);

static int wall_collisions(ball_t *usr_ball, dimensions_t *walls);
static void displayGameOver();
int kbdhit();

// matrix of x y
static int m[SCREEN_WIDTH][SCREEN_HEIGHT];

void setPixelOn(int x, int y, int colour) {
	m[y][x] = colour;
}

static paddle_t usr1_paddle = { 0 }; /* set the paddle variables */
static paddle_t usr2_paddle = { 0 };
static ball_t usr_ball = { 0 }; /* set the ball */
dimensions_t walls = {SCREEN_WIDTH, SCREEN_HEIGHT};
static int usr1_score = 0;
static int usr2_score = 0;

static pthread_t pthreadPong;
static _Bool run = false;
static _Bool playing = false;
static int playerID = 1;
static int readyCount = 0;
static int readySelf = 0;

static void* runPong();

void Pong_init(int player) {

	playerID = player;

	/* initialize curses */
//	initscr();
//	noecho();
//	curs_set(0);

	run = true;
	playing = true;
	pthread_create(&pthreadPong, NULL, &runPong, NULL);

}

static void pongGameInit() {

	usr1_score = 0;
	usr2_score = 0;
	usr1_paddle.x = 1;
	usr1_paddle.y = 10;
	usr2_paddle.x = 30;
	usr2_paddle.y = 10;
	usr1_paddle.len = walls.y / 4;
	usr2_paddle.len = walls.y / 4;

	usr_ball.x = walls.x / 2;
	usr_ball.y = walls.y / 2;
	usr_ball.next_x = 0;
	usr_ball.next_y = 0;
	usr_ball.x_vel = 1;
	usr_ball.y_vel = 1;
}

static void clearMatrix()
{
	for (int x = 0; x < SCREEN_WIDTH; x++){
		for (int y = 0; y < SCREEN_HEIGHT; y++){
			m[x][y] = 0;
		}
	}
}

void Pong_movePaddle(int player, int dir) {
	if (player == 1) {
		paddle_pos(&usr1_paddle, &walls, dir);
	}else {
		paddle_pos(&usr2_paddle, &walls, dir);
	}
}

void Pong_increaseReadyCount(){
	if (readyCount < 2){
		readyCount ++;
	}
}

void Pong_resetGame() {
	readySelf = 0;
	readyCount = 0;
}

static void* runPong()
{
	while (run) {
		pongGameInit();

		while(readyCount < 2) {
			Text_drawLetter(m, 'P', COLOUR_RED, 4, 0);
			Text_drawLetter(m, 'O', COLOUR_GREEN, 4, 8);
			Text_drawLetter(m, 'N', COLOUR_BLUE, 4, 16);
			Text_drawLetter(m, 'G', COLOUR_YELLOW, 4, 24);
			LEDMatrix_update(m);

			if(Joystick_getDirection() == CENTER && readySelf == 0) {
				Pong_increaseReadyCount();
				readySelf = 1;
				UDP_send_message("r");
			}
		}
		
		
		
		while(readyCount == 2) {
			draw_ball(&usr_ball);
			draw_paddle(&usr1_paddle);
			draw_paddle(&usr2_paddle);
			LEDMatrix_update(m);
			clearMatrix();
			Helper_milliSleep(DELAY);

			/* set next positions */
			usr_ball.next_x = usr_ball.x + usr_ball.x_vel;
			usr_ball.next_y = usr_ball.y + usr_ball.y_vel;

			/* check for collisions */
			paddle_collisions(&usr_ball, &usr1_paddle, 1);
			paddle_collisions(&usr_ball, &usr2_paddle, 2);
			if (wall_collisions(&usr_ball, &walls)) {
				playing = false;
				LEDMatrix_clear();
				break;
			}

			if(playerID == 1) {
				if (Joystick_getDirection() == UP){
					paddle_pos(&usr1_paddle, &walls, 1);
					UDP_send_message("1");
				} else {
					if (Joystick_getDirection() == DOWN){
						paddle_pos(&usr1_paddle, &walls, 0);
						UDP_send_message("0");
					}
				}
			}else {
				if (Joystick_getDirection() == UP){
					paddle_pos(&usr2_paddle, &walls, 1);
					UDP_send_message("1");
				} else {
					if (Joystick_getDirection() == DOWN){
						paddle_pos(&usr2_paddle, &walls, 0);
						UDP_send_message("0");
					}
				}
			}

			if (playerID == 1) {
				Display_num(usr1_score);
			} else {
				Display_num(usr2_score);
			}
		}

		printf("GAME OVER\nFinal usr1_score: %d\n", usr1_score);
		printf("GAME OVER\nFinal usr2_score: %d\n", usr2_score);
		Pong_resetGame();
		displayGameOver();

	}

	return 0;

}


/*
 * function : paddle_pos
 * purpose  : have a function that will return a proper 'y' value for the paddle
 * input    : paddle_t *inpt_paddle, dimensions_t *wall, char dir
 * output   : void
 */
static void paddle_pos(paddle_t *pddl, dimensions_t *wall, int dir)
{
	if (dir == 0) { /* moving down */
		if (pddl->y + pddl->len + 1 <= wall->y)
			pddl->y++;
	} else {          /* moving up (must be 'k') */
		if (pddl->y - 1 >= 0)
			pddl->y--;

	}

	return;
}

/*
 * function : wall_collisions
 * purpose  : to check for collisions on the terminal walls
 * input    : ball_t *, dimensions_t *
 * output   : nothing (stored within the structs)
 */
static int wall_collisions(ball_t *usr_ball, dimensions_t *walls)
{
	/* check if we're supposed to leave quick */
	if (usr_ball->next_x < 0) {
		usr1_score --;
		usr2_score ++;
		return 1;
	}
	if (usr_ball->next_x > SCREEN_WIDTH) {
		usr2_score --;
		usr1_score ++;
		return 1;
	}


	 usr_ball->x += usr_ball->x_vel;

	/* check for Y */
	if (usr_ball->next_y >= walls->y || usr_ball->next_y < 0) {
		usr_ball->y_vel *= -1;
	} else {
		usr_ball->y += usr_ball->y_vel;
	}

	return 0;
}

/* -------------------------------------------------------------------------- */

static void paddle_collisions(ball_t *inpt_ball, paddle_t *inpt_paddle, int paddle)
{
	/* 
	* simply check if next_% (because we set the next_x && next_y first) 
	* is within the bounds of the paddle's CURRENT position
	*/

	if (inpt_ball->next_x <= inpt_paddle->x && paddle == 1) {
		if (inpt_paddle->y <= inpt_ball->y && inpt_ball->y <= inpt_paddle->y + inpt_paddle->len) {
			usr1_score++;
			inpt_ball->x_vel *= -1;
			inpt_ball->y_vel *= 1;
		}
	}

	if(inpt_ball->next_x >= inpt_paddle->x && paddle == 2) {
		if (inpt_paddle->y <= inpt_ball->y && inpt_ball->y <= inpt_paddle->y + inpt_paddle->len) {
			usr2_score++;
			inpt_ball->x_vel *= -1;
			inpt_ball->y_vel *= 1;
		}
	}

	return;
}

/* -------------------------------------------------------------------------- */

/*
 * functions : draw_ball && draw_paddle
 * purpose   : condense the drawing functions to functions
 * input     : ball_t * && paddle_t *
 * output    : void
 */
static void draw_ball(ball_t *input)
{
	setPixelOn(input->y, input->x, COLOUR_WHITE);
	return;
}

static void draw_paddle(paddle_t *paddle)
{
	int i;

	for (i = 0; i < paddle->len; i++) {
		setPixelOn(paddle->y + i, paddle->x, COLOUR_GREEN);
	}

	return;
}

static void displayGameOver()
{
	LEDMatrix_clear();
	clearMatrix();
	Text_drawLetter(m, 'G', COLOUR_GREEN, 0, 0);
	Text_drawLetter(m, 'A', COLOUR_GREEN, 0, 8);
	Text_drawLetter(m, 'M', COLOUR_GREEN, 0, 16);
	Text_drawLetter(m, 'E', COLOUR_GREEN, 0, 24);
	Text_drawLetter(m, 'O', COLOUR_RED, 8, 0);
	Text_drawLetter(m, 'V', COLOUR_RED, 8, 8);
	Text_drawLetter(m, 'E', COLOUR_RED, 8, 16);
	Text_drawLetter(m, 'R', COLOUR_RED, 8, 24);
	LEDMatrix_update(m);
	sleep(1);
	LEDMatrix_clear();
	clearMatrix();

	int win = 0;

	if (usr1_score > usr2_score && playerID == 1) {
		win = 1;
	}
	if (usr2_score > usr1_score && playerID == 2) {
		win = 1;
	}
	if (usr2_score == usr1_score) {
			win = 2;
	}

	if (win == 1) {
		Text_drawLetter(m, 'Y', COLOUR_GREEN, 0, 0);
		Text_drawLetter(m, 'O', COLOUR_GREEN, 0, 8);
		Text_drawLetter(m, 'U', COLOUR_GREEN, 0, 16);
		Text_drawLetter(m, 'W', COLOUR_GREEN, 8, 0);
		Text_drawLetter(m, 'I', COLOUR_GREEN, 8, 8);
		Text_drawLetter(m, 'N', COLOUR_GREEN, 8, 16);
	} else if (win == 0) {
		Text_drawLetter(m, 'Y', COLOUR_RED, 0, 0);
		Text_drawLetter(m, 'O', COLOUR_RED, 0, 8);
		Text_drawLetter(m, 'U', COLOUR_RED, 0, 16);
		Text_drawLetter(m, 'L', COLOUR_RED, 8, 0);
		Text_drawLetter(m, 'O', COLOUR_RED, 8, 8);
		Text_drawLetter(m, 'S', COLOUR_RED, 8, 16);
		Text_drawLetter(m, 'E', COLOUR_RED, 8, 24);
	} else if (win == 2) {
		Text_drawLetter(m, 'Y', COLOUR_WHITE, 0, 0);
		Text_drawLetter(m, 'O', COLOUR_WHITE, 0, 8);
		Text_drawLetter(m, 'U', COLOUR_WHITE, 0, 16);
		Text_drawLetter(m, 'T', COLOUR_WHITE, 8, 0);
		Text_drawLetter(m, 'I', COLOUR_WHITE, 8, 8);
		Text_drawLetter(m, 'E', COLOUR_WHITE, 8, 16);
	}
	LEDMatrix_update(m);
	sleep(2);
	LEDMatrix_clear();
	clearMatrix();

}

/*
 * function : kbdhit
 * purpose  : find out if we've got something in the input buffer
 * input    : void
 * output   : 0 on none, 1 on we have a key
 */

int kbdhit()
{
	int key = getchar();

	if (key != 0) {
		return 0;
	} else {
		return 1;
	}
}
