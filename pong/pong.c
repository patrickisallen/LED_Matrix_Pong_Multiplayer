#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include "helper.h"
#include "ledMatrix.h"
#include "joystick.h"
#include "colours.h"

#define DELAY 100

typedef struct paddle {
	/* paddle variables */
	int x;
	int y;    /* y is the 'top' of the paddle */
	int len;
	int score;
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
//void draw_score(paddle_t *inpt_paddle, dimensions_t *wall);
static void paddle_collisions(ball_t *inpt_ball, paddle_t *inpt_paddle);
static void paddle_pos(paddle_t *pddl, dimensions_t *wall, int dir);

static int wall_collisions(ball_t *usr_ball, dimensions_t *walls);
int kbdhit();

// matrix of x y
static int m[SCREEN_WIDTH][SCREEN_HEIGHT];

/*
 * TODO: This should call the ledMatrix_setPixel in the ledMatrix file
 */
void setPixelOn(int x, int y, int colour) {
	m[y][x] = colour;
}

static paddle_t usr_paddle = { 0 }; /* set the paddle variables */
static ball_t usr_ball = { 0 }; /* set the ball */
dimensions_t walls = { 32, 16};
static int keypress;

static pthread_t pthreadPong;
static _Bool run = false;

static void* runPong();

void Pong_init() {


	/* initialize curses */
//	initscr();
//	noecho();
//	curs_set(0);

	usr_paddle.x = 5;
	usr_paddle.y = 11;
	usr_paddle.len = walls.y / 4;
	usr_paddle.score = 0;

	usr_ball.x = walls.x / 2;
	usr_ball.y = walls.y / 2;
	usr_ball.next_x = 0;
	usr_ball.next_y = 0;
	usr_ball.x_vel = 1;
	usr_ball.y_vel = 1;

	/* we actually have to store the user's keypress somewhere... */
	keypress = 0;
//	nodelay(stdscr, TRUE);
//	scrollok(stdscr, TRUE);

	run = true;
	pthread_create(&pthreadPong, NULL, &runPong, NULL);

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
		//while (kbdhit()) {
//			clear(); /* clear screen of all printed chars */

		draw_ball(&usr_ball);
		draw_paddle(&usr_paddle);
//			refresh(); /* draw to term */
		LEDMatrix_update(m);
		clearMatrix();
		Helper_milliSleep(DELAY);

		/* set next positions */
		usr_ball.next_x = usr_ball.x + usr_ball.x_vel;
		usr_ball.next_y = usr_ball.y + usr_ball.y_vel;

		/* check for collisions */
		paddle_collisions(&usr_ball, &usr_paddle);
		if (wall_collisions(&usr_ball, &walls)) {
			run = false;
			LEDMatrix_clear();
			break;
		}
		//}

		/* we fell out, get the key press */
		//keypress = getchar();
		if (Joystick_getDirection() == UP){
			paddle_pos(&usr_paddle, &walls, 1);
		} else {
			if (Joystick_getDirection() == DOWN){
				paddle_pos(&usr_paddle, &walls, 0);
			}
		}

		switch (keypress) {

		case 'p': /* pause functionality, because why not */
//			mvprintw(1, 0, "PAUSE - press any key to resume");
//			while (getch() == ERR) {
//				Helper_milliSleep(7);
//			}
			break;

		case 'q':
			run = false;
			break;

		}
	}

//	endwin();

	printf("GAME OVER\nFinal Score: %d\n", usr_paddle.score);

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

	/* check for X */
	if (usr_ball->next_x >= walls->x) {
		usr_ball->x_vel *= -1;
	} else {
		usr_ball->x += usr_ball->x_vel;
	}

	/* check for Y */
	if (usr_ball->next_y >= walls->y || usr_ball->next_y < 0) {
		usr_ball->y_vel *= -1;
	} else {
		usr_ball->y += usr_ball->y_vel;
	}

	return 0;
}

/* -------------------------------------------------------------------------- */

static void paddle_collisions(ball_t *inpt_ball, paddle_t *inpt_paddle)
{
	/* 
	* simply check if next_% (because we set the next_x && next_y first) 
	* is within the bounds of the paddle's CURRENT position
	*/

	if (inpt_ball->next_x == inpt_paddle->x) {
		if (inpt_paddle->y <= inpt_ball->y &&
			inpt_ball->y <= 
			inpt_paddle->y + inpt_paddle->len) {

			inpt_paddle->score++;
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
