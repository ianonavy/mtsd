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

#include <stdbool.h>
#include "mtsd.h"

#define WM_START_WINDOW_MOVE_CMD "bspc pointer -g move"
#define WM_START_WINDOW_RESIZE_CMD "bspc pointer -g resize_corner"
#define WM_STOP_TRACKING_MOUSE_CMD "bspc pointer -u"
#define WM_UPDATE_MOUSE_CMD_FMT "bspc pointer -t %d %d"
#define WM_CLOSE_WINDOW_CMD "bspc window -c"
#define WM_FULLSCREEN_ON_CMD "bspc window -t fullscreen=on"
#define WM_FULLSCREEN_OFF_CMD "bspc window -t fullscreen=off"
#define WM_PREV_DESKTOP_CMD "bspc desktop -f prev"
#define WM_NEXT_DESKTOP_CMD "bspc desktop -f next"

#define SWIPE_THRESHOLD (128)

/* post-lift gestures */

#define CLOSE_NUM_TOUCHES (5)
#define CLOSE_SWIPE false
#define CLOSE_SWIPE_DIRECTION SWIPE_DOWN

#define FULLSCREEN_ON_NUM_TOUCHES (3)
#define FULLSCREEN_ON_SWIPE true
#define FULLSCREEN_ON_SWIPE_DIRECTION SWIPE_UP

#define FULLSCREEN_OFF_NUM_TOUCHES (3)
#define FULLSCREEN_OFF_SWIPE true
#define FULLSCREEN_OFF_SWIPE_DIRECTION SWIPE_DOWN

#define PREV_DESKTOP_NUM_TOUCHES (3)
#define PREV_DESKTOP_SWIPE true
#define PREV_DESKTOP_SWIPE_DIRECTION SWIPE_RIGHT

#define NEXT_DESKTOP_NUM_TOUCHES (3)
#define NEXT_DESKTOP_SWIPE true
#define NEXT_DESKTOP_SWIPE_DIRECTION SWIPE_LEFT
