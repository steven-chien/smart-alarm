#!/usr/bin/python

#Copyright (C) 2016 Steven Chien

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>



# this program should be executed by root crontab (need to read sensor data) once every min. #

import sqsqlite33
import time
import sys
import os

con = sqlite3.connect('/home/Steven/Programs/smart-alarm/backend/src/alarms.db')

with con:

	cur = con.cursor()    
	cur.execute('SELECT * FROM alarms')
	now = int(time.time())
	rows = cur.fetchall()

	for row in rows:
		alarm_id = int(row[0])
		prev_wake = int(row[1])
		wake = int(row[2])
		cycle = int(row[3])

		# extract alarm status
		cur.execute('select first_ring, ringing from status where alarm_id=?', [str(alarm_id)])
		state = cur.fetchone()
		first_ring = state[0]
		activated = state[1]

		# if this is the first ringing, ignore motion sensor feedback
		if first_ring is 1:
			cur.execute('update alarms set prev_wake=wake where alarm_id=?', [str(alarm_id)])
			con.commit()
		elif first_ring is not 1:
			# if the alarm has been snoozed, check motion sensor to see if alarm still required
			movements = sum([int(s.strip()) for s in open('foo.txt').readlines()])
			if movements > 5:
				# TODO implement cycle update
				continue

		# check if alarm qualify for ringing and media player is not already running
		if wake < now and not activated:
			print "ring "+str(wake)
			os.system("ogg123 /home/Steven/Programs/test_alrm/alarm/ringtone.ogg --repeat "+str(alarm_id)+" > /dev/null 2>&1 &")
			cur.execute('update status set first_ring=0, ringing=1 where alarm_id=?', [str(alarm_id)])
			con.commit()
