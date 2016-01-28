#!/usr/bin/python
# -*- coding: utf-8 -*-

import sqlite3 as lite
import time
import sys
import os

con = lite.connect('/home/Steven/Programs/test_alrm/pyalarm/alarms.db')

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

		cur.execute('select * from status')
		state = cur.fetchone()
		first_ring = state[1]
		activated = state[2]
		completed = state[3]
		print state
		if first_ring is 1:
			cur.execute('update alarms set prev_wake=wake where alarm_id=?', [str(alarm_id)])
			con.commit()
		elif first_ring is not 1:
			movements = sum([int(s.strip()) for s in open('foo.txt').readlines()])
			if movements > 5:
				continue

		if wake < now and not activated and not completed:
			print "ring "+str(wake)
			os.system("ogg123 /home/Steven/Programs/test_alrm/alarm/ringtone.ogg --repeat "+str(alarm_id)+" > /dev/null 2>&1 &")
			cur.execute('update status set first_ring=0, ringing=1 where alarm_id=?', [str(alarm_id)])
			con.commit()
