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

#include <geis/geis.h>

void map_callback_on_touch(
	GeisTouchSet touchset,
	GeisFrame frame,
	void (*callback)(GeisTouch))
{
	GeisSize k;
	for (k = 0; k < geis_frame_touchid_count(frame); ++k) {
		GeisSize touchid = geis_frame_touchid(frame, k);
		GeisTouch touch = geis_touchset_touch_by_id(touchset, touchid);
		callback(touch);
	}
}

void geis_for_each_touch(GeisEvent event, void (*callback)(GeisTouch))
{
	GeisSize i;
	GeisTouchSet touchset;
	GeisGroupSet groupset;
	GeisAttr attr;

	attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_TOUCHSET);
	touchset = geis_attr_value_to_pointer(attr);

	attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
	groupset = geis_attr_value_to_pointer(attr);

	for (i = 0; i < geis_groupset_group_count(groupset); ++i) {
		GeisSize j;
		GeisGroup group = geis_groupset_group(groupset, i);

		for (j = 0; j < geis_group_frame_count(group); ++j) {
			GeisFrame frame = geis_group_frame(group, j);
			map_callback_on_touch(touchset, frame, callback);
		}
	}
}

void geis_for_each_frame(
	GeisEvent event,
	void (*callback)(GeisTouchSet, GeisFrame))
{
	GeisSize i;
	GeisTouchSet touchset;
	GeisGroupSet groupset;
	GeisAttr attr;

	attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_TOUCHSET);
	touchset = geis_attr_value_to_pointer(attr);

	attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
	groupset = geis_attr_value_to_pointer(attr);

	for (i = 0; i < geis_groupset_group_count(groupset); ++i) {
		GeisSize j;
		GeisGroup group = geis_groupset_group(groupset, i);

		for (j = 0; j < geis_group_frame_count(group); ++j) {
			GeisFrame frame = geis_group_frame(group, j);
			callback(touchset, frame);
		}
	}
}