/*
 * bounce.c - Cheesy ASCII animations involving bouncing balls.
 * Copyright (C) 2009  Emmanuel MacCaull
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_BALLS 64
#define HEIGHT    24
#define WIDTH     80
#define MAX_YPOS  (HEIGHT - 2)
#define MAX_XPOS  (WIDTH - 2)

#define HWALL      '-'
#define VWALL      '|'
#define EMPTY_CELL ' '

#define BALL_SPEED 1


/*
 * All the data we need to keep track of the position and direction of travel
 * of a single ball.
 */
typedef struct {
    int xpos;  /* Horizontal position of the ball. */
    int xdir;  /* Horizontal direction of the ball (either +1 or -1). */
    int ypos;  /* Vertical position. */
    int ydir;  /* Vertical direction (+1 == down, -1 == up). */
    char printable;  /* Char to use when drawing the ball. */
} Ball;


/*
 * A box for balls to bounce in. The dimensions of this box determine the
 * bounds for the bouncing ball.
 */
typedef struct {
    char cells[HEIGHT][WIDTH+1];
} Box;


/*
 * Return the maximum value of a and b.
 */
int max(int a, int b)
{
    return a > b ? a : b;
}


/*
 * Return the minimum value of a and b.
 */
int min(int a, int b)
{
    return a < b ? a : b;
}


/*
 * Initialize a box "object". Makes the box look something like:
 *
 * ---------
 * |       |
 * |       |
 * ---------
 *
 * A Box is actually an array of NUL terminated ASCII strings.
 */
void init_box(Box* box)
{
    int i, j;
    /* For each row of the box... */
    for (i = 0; i < HEIGHT; i++) {
        /* initialize each column of the box. */
        for (j = 0; j < WIDTH; j++) {
            if (i == 0 || i == HEIGHT - 1)
                box->cells[i][j] = HWALL;
            else if (j == 0 || j == WIDTH - 1)
                box->cells[i][j] = VWALL;
            else
                box->cells[i][j] = EMPTY_CELL;
        }
        box->cells[i][WIDTH] = '\0'; /* NUL terminate the string. */
    }
}


/*
 * Output the scene specified by box to the terminal.
 */
void draw_scene(const Box* box)
{
    int i;

    /* 
     * Since the box is an array of strings, we can simply loop over the array
     * and print the strings that represent each row of the box.
     */
    for (i = 0; i < HEIGHT; i++)
        printf("%s\n", box->cells[i]);
}


/*
 * Use some really dumb physics to move the ball from its current position 
 * in the box to its new position. There's no fancy stuff like friction.
 */
void move_ball(Ball* ball, Box* bounds)
{
    /* Calculate where the ball should go next. */
    int x = ball->xpos + ball->xdir * BALL_SPEED;
    int y = ball->ypos + ball->ydir * BALL_SPEED;

    /* Erase the cell where the ball just was. */
    bounds->cells[ball->ypos][ball->xpos] = EMPTY_CELL;

    /* Move the ball, but make sure it stays inside the bounds of the box. */
    ball->xpos = max(1, min(x, MAX_XPOS));
    ball->ypos = max(1, min(y, MAX_YPOS));

    /* Redraw the ball in its new cell. */
    bounds->cells[ball->ypos][ball->xpos] = ball->printable;

    /* Change the direction of the ball if it has hit one of the walls. */
    if (ball->xpos == 1 || ball->xpos == MAX_XPOS)
        ball->xdir *= -1;
    if (ball->ypos == 1 || ball->ypos == MAX_YPOS)
        ball->ydir *= -1;
}


/* Pause the program for the given number of milliseconds. */
void delay(long milliseconds)
{
    /* 
     * usleep is only available in Linux and Mac OS X (i.e. this won't 
     * work in Windows).
     */
    usleep(milliseconds * 1000);
}


int main(int argc, char* argv[])
{
    Box scene;              /* The box that all balls will bounce inside. */
    Ball balls[MAX_BALLS];  /* An array of balls to bounce. */
    int balls_len = 1;      /* The number of balls we will actually use. */
    int i;

    /* Check the arguments that were given to the program. */
    if (argc > 1) {
        balls_len = atoi(argv[1]);
        if (balls_len <= 0)
            balls_len = 1;
        else if (balls_len > MAX_BALLS)
            balls_len = MAX_BALLS;
    }

    /*
     * Seed the function random(). This ensures that random gives us a unique
     * sequence of random numbers each time we run the program.
     */
    srandom(time(0));

    /* Randomly initialize all the balls. */
    for (i = 0; i < balls_len; i++) {
        balls[i].xpos = random() % MAX_XPOS + 1;
        balls[i].xdir = random() % 2 == 0 ? 1 : -1;
        balls[i].ypos = random() % MAX_YPOS + 1;
        balls[i].ydir = random() % 2 == 0 ? 1 : -1;
        balls[i].printable = 'o';
    }

    init_box(&scene);

    /* The main loop. It never ends! */
    while (1) {
        draw_scene(&scene);

        /* Move all the balls in the scene to their new positions. */
        for (i = 0; i < balls_len; i++)
            move_ball(&balls[i], &scene);

        /* The delay allows us to actually see what's going on. */
        delay(66); /* A delay of 66 will give us about 15 frames per second. */
    }
    return 0;
}
