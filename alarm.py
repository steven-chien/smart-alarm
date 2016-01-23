#!/bin/env python

import sys
import time
import thread
import cherrypy

ring = True

def clock(alarm_time):
	alarms.sort()
	next_alarm = float(alarm_time)
	now = float(time.time())
	sleep_period = next_alarm - now
	if sleep_period < 0:
		return
	print str(next_alarm) + ' ' + str(now)
	time.sleep(sleep_period)
	print "alarm"

def main():
	thread.start_new_thread(clock, (,))

if __name__ == "__main__":
	# start the alarm polling at begining of a second
	main()
	while 1:
		pass
