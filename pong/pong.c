#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include "helper.h"
#include "ledMatrix.h"
#include "joystick.h"
#include "display.h"

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

static void* runPong();

void Pong_init() {

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
	usr1_paddle.x = 3;
	usr1_paddle.y = 11;
	usr2_paddle.x = 29;
	usr2_paddle.y = 11;
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

static void* runPong()
{
	while (run) {
		pongGameInit();
		while(playing) {
			//while (kbdhit()) {
	//			clear(); /* clear screen of all printed chars */

			draw_ball(&usr_ball);
			draw_paddle(&usr1_paddle);
	//			refresh(); /* draw to term */
			LEDMatrix_update(m); // TODO scott: this should call the driver to update the matrix
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
			//}

			/* we fell out, get the key press */
			if (Joystick_getDirection() == UP){
				paddle_pos(&usr1_paddle, &walls, 1);
			} else {
				if (Joystick_getDirection() == DOWN){
					paddle_pos(&usr1_paddle, &walls, 0);
				}
			}

			Display_num(usr1_score);
		}

	//	endwin();

		printf("GAME OVER\nFinal usr1_score: %d\n", usr1_score);
		displayGameOver();
		while(Joystick_getDirection() != CENTER){}
		playing = true;
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
		return 1;
	}
	if (usr_ball->next_x > SCREEN_WIDTH) {
		return 1;
	}

	// /* check for X */
	// if (usr_ball->next_x >= walls->x) {
	// 	usr_ball->x_vel *= -1;
	// } else {
	// 	usr_ball->x += usr_ball->x_vel;
	// }

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

	if (inpt_ball->next_x == inpt_paddle->x && paddle == 1) {
		if (inpt_paddle->y <= inpt_ball->y &&
			inpt_ball->y <= 
			inpt_paddle->y + inpt_paddle->len) {
			usr1_score++;
			inpt_ball->x_vel *= -1;
		}
	}

	if(inpt_ball->next_x == inpt_paddle->x && paddle == 2) {
		if (inpt_paddle->y <= inpt_ball->y && inpt_ball->y <= inpt_paddle->y + inpt_paddle->len) {
			usr2_score++;
			inpt_ball->x_vel *= -1;
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
	// TODO: display char here
	LEDMatrix_update(m);

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
