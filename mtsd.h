/*
 * The MIT License (MIT)
 * Copyright (c) 2014 Ian Naval
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef MTSD_H
#define MTSD_H

#include <stdbool.h>

typedef enum {
	DEFAULT,
	MOVING,
	RESIZING,
} mtsd_state;

typedef enum {
	SWIPE_UP,
	SWIPE_DOWN,
	SWIPE_LEFT,
	SWIPE_RIGHT,
} mtsd_direction;

mtsd_state state;
float old_x = 0.0;
float old_y = 0.0;
float new_x = 0.0;
float new_y = 0.0;
int num_touches = 0;
bool running;

/*
 * Parses the arguments.
 *
 * @argc: The number of arguments from main
 * @argv: Array of strings passed as argument to main
 */
void parse_args(const int argc, const char *argv[]);

/*
 * Prints the usage help text.
 * @program: String for the program name
 */
void usage(const char *program);

/*
 * Prints the version number.
 */
void version(void);

/*
 *
 */
void setup(void);

void start(GeisEvent event);
void update(GeisEvent event);
void end(GeisEvent event);


#endif
