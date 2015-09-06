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


typedef struct {
    int xpos;  /* Horizontal position of the ball. */
    int xdir;  /* Horizontal direction (+1 == right, -1 == left). */
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


int max(int a, int b)
{
    return a > b ? a : b;
}


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
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (i == 0 || i == HEIGHT - 1)
                box->cells[i][j] = HWALL;
            else if (j == 0 || j == WIDTH - 1)
                box->cells[i][j] = VWALL;
            else
                box->cells[i][j] = EMPTY_CELL;
        }
        box->cells[i][WIDTH] = '\0';
    }
}


/*
 * Output the scene specified by box to the terminal.
 */
void draw_scene(const Box* box)
{
    int i;
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
    /* This won't work in Windows; would use Sleep() instead. */
    usleep(milliseconds * 1000);
}


int main(int argc, char* argv[])
{
    Box scene;              /* The box that all balls will bounce inside. */
    Ball balls[MAX_BALLS];
    int balls_len = 1;      /* The number of balls we will actually use. */
    int i;

    /* Did the user specify the number of balls to use? */
    if (argc > 1) {
        int user_len = atoi(argv[1]);
        /* Keep the number of balls in the valid range. */
        balls_len = max(1, min(user_len, MAX_BALLS));
    }

    srandom(time(0));
    for (i = 0; i < balls_len; i++) {
        balls[i].xpos = random() % MAX_XPOS + 1;
        balls[i].xdir = random() % 2 == 0 ? 1 : -1;
        balls[i].ypos = random() % MAX_YPOS + 1;
        balls[i].ydir = random() % 2 == 0 ? 1 : -1;
        balls[i].printable = 'o';
    }

    init_box(&scene);

    while (1) {
        draw_scene(&scene);

        /* Move all the balls in the scene to their new positions. */
        for (i = 0; i < balls_len; i++)
            move_ball(&balls[i], &scene);

        /* A delay of 66 will give us about 15 frames per second. */
        delay(66);
    }
    return 0;
}
