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

#include <errno.h>
#include <getopt.h>
#include <geis/geis.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/select.h>
#include "config.h"
#include "geis_helpers.h"
#include "helpers.h"
#include "mtsd.h"


/* Private helpers */

void update_x_y_from_touch(GeisTouch touch, float *x, float *y)
{
	GeisAttr attr;
	attr = geis_touch_attr_by_name(touch, GEIS_TOUCH_ATTRIBUTE_X);
	*x = geis_attr_value_to_float(attr);
	attr = geis_touch_attr_by_name(touch, GEIS_TOUCH_ATTRIBUTE_Y);
	*y = geis_attr_value_to_float(attr);
}

void update_old_x_y_from_touch(GeisTouch touch)
{
	update_x_y_from_touch(touch, &old_x, &old_y);
}

void update_new_x_y_from_touch(GeisTouch touch)
{
	update_x_y_from_touch(touch, &new_x, &new_y);
}

void update_from_frame(GeisTouchSet touchset, GeisFrame frame)
{
	GeisAttr attr;
	GeisSize k;
	int touches;

	attr = geis_frame_attr_by_name(frame, GEIS_GESTURE_ATTRIBUTE_TOUCHES);
	touches = geis_attr_value_to_integer(attr);
	num_touches = touches > num_touches ? touches : num_touches;
	if (touches == 2 && state != MOVING) {
		system(WM_START_WINDOW_MOVE_CMD);
		state = MOVING;
	}
	if (touches == 4 && state != RESIZING) {
		system(WM_START_WINDOW_RESIZE_CMD);
		state = RESIZING;
	}
	if ((touches > 2 && state == MOVING) || (touches > 4 && state == RESIZING)) {
		system(WM_STOP_TRACKING_MOUSE_CMD);
		state = DEFAULT;
	}

	if (state == MOVING || state == RESIZING) {
		for (k = 0; k < geis_frame_touchid_count(frame); ++k) {
			GeisSize  touchid = geis_frame_touchid(frame, k);
			GeisTouch touch = geis_touchset_touch_by_id(touchset, touchid);
			update_x_y_from_touch(touch, &new_x, &new_y);
		}
		char *cmd;
		size_t needed = snprintf(
			NULL, 0, WM_UPDATE_MOUSE_CMD_FMT,
			(int) new_x, (int) new_y);
		cmd = malloc(needed);
		snprintf(
			cmd, needed, WM_UPDATE_MOUSE_CMD_FMT,
			(int) new_x, (int) new_y);
		system(cmd);
		free(cmd);
	}
}

bool run(int *fd, Geis *geis, GeisStatus *status)
{
	fd_set read_fds;
	FD_ZERO(&read_fds);
	FD_SET(0, &read_fds);
	FD_SET(*fd, &read_fds);
	int sstat = select(*fd+1, &read_fds, NULL, NULL, NULL);
	if (sstat < 0) {
		fatal("error %d in select(): %s\n", errno, strerror(errno));
	}
 	if (FD_ISSET(*fd, &read_fds)) {
		GeisEvent event;
		*status = geis_dispatch_events(*geis);
		*status = geis_next_event(*geis, &event);
		while (*status == GEIS_STATUS_CONTINUE ||
			*status == GEIS_STATUS_SUCCESS) {
			switch (geis_event_type(event)) {
			case GEIS_EVENT_DEVICE_AVAILABLE:
			case GEIS_EVENT_DEVICE_UNAVAILABLE:
				break;
			case GEIS_EVENT_GESTURE_BEGIN:
				start(event);
				break;
			case GEIS_EVENT_GESTURE_UPDATE:
				update(event);
				break;
			case GEIS_EVENT_GESTURE_END:
				end(event);
				break;
			default:
				break;
			}
			geis_event_delete(event);
			*status = geis_next_event(*geis, &event);
		}
	}
	return !(FD_ISSET(0, &read_fds));
}

int main(const int argc, const char *argv[])
{
	Geis geis;
	GeisSubscription subscription;
	GeisFilter filter;
	GeisStatus status;
	int fd;

	parse_args(argc, argv);
	setup();

	geis = geis_new(GEIS_INIT_TRACK_DEVICES, NULL);
	subscription = geis_subscription_new(geis, "mtsd", GEIS_SUBSCRIPTION_CONT);
	filter = geis_filter_new(geis, "mtsd_filter");

	status = geis_subscription_add_filter(subscription, filter);
	status = geis_subscription_activate(subscription);

	geis_get_configuration(geis, GEIS_CONFIGURATION_FD, &fd);

	while (running) {
		running = run(&fd, &geis, &status);
	}

	geis_subscription_delete(subscription);
	geis_delete(geis);

	return 0;
}

void sig_handler(int sig)
{
	if (sig == SIGTERM || sig == SIGINT) {
		running = false;
	}
}


/* Public */

void parse_args(const int argc, const char *argv[])
{
	char opt;
	while ((opt = getopt(argc, (char**) argv, "dvh:")) != (char)-1) {
		switch (opt) {
		case 'd':
			daemonize();
			break;
		case 'v':
			version();
			exit(EXIT_SUCCESS);
			break;
		case 'h':
			usage(argv[0]);
			exit(EXIT_SUCCESS);
		}
	}
}

void usage(const char *program)
{
	printf("Usage: %s [-d|-h|-v]\n", program);
}

void version(void)
{
	printf("%s\n", VERSION);
}

void setup(void)
{
	state = DEFAULT;
	old_x = 0.0;
	old_y = 0.0;
	new_x = 0.0;
	new_y = 0.0;
	num_touches = 0;
	running = true;

	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
}

void start(GeisEvent event)
{
	geis_for_each_touch(event, update_old_x_y_from_touch);
}

void update(GeisEvent event)
{
	geis_for_each_frame(event, update_from_frame);
}

bool check_gesture(
	bool is_swipe,
	int direction,
	int correct_direction,
	bool gesture_is_swipe,
	int gesture_num_touches)
{
	bool correct_is_swipe = (is_swipe == gesture_is_swipe);
	bool correct_swipe = (is_swipe && direction == correct_direction);
	bool correct_gesture = (correct_swipe || !is_swipe);
	bool correct_num_touches = (num_touches == gesture_num_touches);
	return correct_is_swipe && correct_gesture && correct_num_touches;
}

void end(GeisEvent event)
{
	float dx, dy;
	geis_for_each_touch(event, update_new_x_y_from_touch);

	dx = new_x - old_x;
	dy = new_y - old_y;
	PRINTF("dx, dy: (%f %f)\nnum_touches: %d\n", dx, dy, num_touches);
	if (state == MOVING || state == RESIZING) {
		system(WM_STOP_TRACKING_MOUSE_CMD);
		state = DEFAULT;
	}
	bool is_swipe = !(abs(dx) < SWIPE_THRESHOLD && abs(dy) < SWIPE_THRESHOLD);
	int direction;
	if (abs(dy) > abs(dx)) {
		if (dy > 0) {
			direction = SWIPE_DOWN;
		} else {
			direction = SWIPE_UP;
		}
	} else {
		if (dx > 0) {
			direction = SWIPE_RIGHT;
		} else {
			direction = SWIPE_LEFT;
		}
	}
 	if (check_gesture(
		is_swipe, direction, CLOSE_SWIPE_DIRECTION,
		CLOSE_SWIPE, CLOSE_NUM_TOUCHES)) {
		system(WM_CLOSE_WINDOW_CMD);
	}
	if (check_gesture(
		is_swipe, direction, FULLSCREEN_ON_SWIPE_DIRECTION,
		FULLSCREEN_ON_SWIPE, FULLSCREEN_ON_NUM_TOUCHES)) {
		system(WM_FULLSCREEN_ON_CMD);
	}
	if (check_gesture(
		is_swipe, direction, FULLSCREEN_OFF_SWIPE_DIRECTION,
		FULLSCREEN_OFF_SWIPE, FULLSCREEN_OFF_NUM_TOUCHES)) {
		system(WM_FULLSCREEN_OFF_CMD);
	}
	if (check_gesture(
		is_swipe, direction, PREV_DESKTOP_SWIPE_DIRECTION,
		PREV_DESKTOP_SWIPE, PREV_DESKTOP_NUM_TOUCHES)) {
		system(WM_PREV_DESKTOP_CMD);
	}
	if (check_gesture(
		is_swipe, direction, NEXT_DESKTOP_SWIPE_DIRECTION,
		NEXT_DESKTOP_SWIPE, NEXT_DESKTOP_NUM_TOUCHES)) {
		system(WM_NEXT_DESKTOP_CMD);
	}

	old_x = old_y = new_x = new_y = num_touches = 0;
}
