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

import sqlite3
import time
import string
import os

import cherrypy
import json

class request_handler(object):
	_cp_config = {'tools.staticdir.on' : True,
		'tools.staticdir.dir' : os.path.dirname(os.path.abspath(__file__)),
		'tools.staticdir.index' : 'index.html',
	}

	# serve new alarm request
	@cherrypy.expose
	def new(self, req_time=123, repeat=60, callback=123, _=123):
		# get current time, throw if req time has passed
		now = time.time()
		alarm_time = int(req_time)
		if alarm_time < now:
			raise cherrypy.HTTPError(500)

		try:
			con = sqlite3.connect('alarms.db')
			cur = con.cursor()    
			cur.execute('insert into alarms values('+str(alarm_time)+','+str(alarm_time)+','+str(alarm_time)+','+str(repeat)+')')
			cur.execute('insert into status values('+str(alarm_time)+', 1, 0, 0)')
			con.commit()
			con.close()
		except Exception as e:
			print e
			cherrypy.response.status = 500
			return 'myJsonpCallback({ \"error\":\"'+str(e)+'\")'

		return 'myJsonpCallbcak({ \"status\": \"success\" })'

	# serve delete alarm request
	@cherrypy.expose
	def delete(self, req_time=123, callback=123, _=123):
		try:
			con = sqlite3.connect('alarms.db')
			cur = con.cursor()
			cur.execute('delete from alarms where alarm_id=?', [str(req_time)])
			cur.execute('delete from status where alarm_id=?', [str(req_time)])
			con.commit()
			con.close()
		except Exception as e:
			print e
			cherrypy.response.status = 500
			return 'myJsonpCallback({ \"error\":\"'+str(e)+'\")'

		return 'myJsonpCallbcak({ \"status\": \"success\" })'

	# serve snooze alarm request
	@cherrypy.expose
	def stop(self, req_time=123, callback=123, _=123):
		try:
			con = sqlite3.connect('alarms.db')
			cur = con.cursor()

			# check if alarm is actually ringing
			cur.execute('select first_ring, ringing, completed from status where alarm_id=?', [str(req_time)])
			alarm_state = cur.fetchone()
			if alarm_state[1] is not 1:
				raise Exception('Alarm not ringing')

			# update alarm status and next wake up time to be 5 minutes later
			cur.execute('update alarms set wake=wake+300 where alarm_id=?', [str(req_time)])
			cur.execute('update status set first_ring=0, ringing=0 where alarm_id=?', [str(req_time)])
			con.commit()
			con.close()

			# kill alarm process
			cmd = "pkill -f \"ogg123 /home/pi/smart-alarm/ringtone.ogg --repeat "+req_time+"\""
			print str(cmd)
			os.system(cmd)

		except Exception as e:
			print e
			cherrypy.response.status = 500
			return 'myJsonpCallback({ \"error\":\"'+str(e)+'\")'

		return 'myJsonpCallbcak({ \"status\": \"success\" })'

	# terminate alarm, should be determined by wakeup.py program instead
	@cherrypy.expose
	def terminate(self, req_time=123, callback=123, _=123):
		try:
			con = sqlite3.connect('alarms.db')
			cur = con.cursor()

			# update alarm wake to to next cycle, reset status fields
			cur.execute('update status set first_ring=1, ringing=0, completed=0 where alarm_id=?', [str(req_time)])
			cur.execute('update alarms set wake=prev_wake+cycle where alarm_id=?', [str(req_time)])
			con.commit()
			con.close()

			# kill program if matched
			os.system("pkill -f \"ogg123 /home/pi/smart-alarm/ringtone.ogg --repeat "+req_time+"\"")
		except Exception as e:
			print e
			cherrypy.response.status = 500
			return 'myJsonpCallback({ \"error\":\"'+str(e)+'\")'

		return 'myJsonpCallbcak({ \"status\": \"success\" })'
	
	# serve request for alarm list and their status
	@cherrypy.expose
	def alarmList(self, callback=123, _=123):
		try:
			con = sqlite3.connect('alarms.db')
			cur = con.cursor()

			# extract a list of alarm and if they are ringing
			cur.execute('select A.alarm_id, A.wake, A.cycle, S.ringing from alarms as A, status as S where S.alarm_id=A.alarm_id order by A.wake')
			rows = cur.fetchall()
			data = {}
			alarms = []
			for row in rows:
				item = {}
				item['id'] = row[0]
				item['wake'] = row[1]
				item['cycle'] = row[2]
				if int(row[3]) == 1:
					item['status'] = True
				elif int(row[3]) ==0:
					item['status'] = False
				alarms.append(item)
			data['alarms'] = alarms
			print str(data)
			return 'myJsonpCallback('+json.dumps(data)+')'
		except Exception as e:
			print e
			cherrypy.response.status = 500
			return 'myJsonpCallback({ \"error\":\"'+str(e)+'\")'

if __name__ == '__main__':
	try:
		# create tables if not already exist
		con = sqlite3.connect('alarms.db')
		cur = con.cursor()
		cur.execute('create table if not exists status(alarm_id int primary key, first_ring bool, ringing bool, completed bool)')
		cur.execute('create table if not exists alarms(alarm_id int primary key, prev_wake int, wake int, cycle int)')
		con.commit()
		con.close()
	except Exception as e:
		print e

	# start cherry py to serve http requests
	cherrypy.quickstart(request_handler())
