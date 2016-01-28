import sqlite3 as lite
import time
import string
import os

import cherrypy
import json

class StringGenerator(object):

	@cherrypy.expose
	def index(self):
		return "Hello world!"

	@cherrypy.expose
	def new(self, req_time=123, repeat=60):
		now = time.time()
		alarm_time = int(req_time)
		if alarm_time < now:
			raise cherrypy.HTTPError(500)

		print 'insert into alarms values('+str(alarm_time)+','+str(alarm_time)+','+str(alarm_time)+','+str(repeat)+')'
		try:
			con = lite.connect('alarms.db')
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

	@cherrypy.expose
	def delete(self, req_time=123):
		try:
			con = lite.connect('alarms.db')
			cur = con.cursor()
			cur.execute('delete from alarms where alarm_id=?', [str(req_time)])
			cur.execute('delete from status where alarm_id=?', [str(req_time)])
			con.commit()
			#con.close()
		except Exception as e:
			print e
			cherrypy.response.status = 500
			return 'myJsonpCallback({ \"error\":\"'+str(e)+'\")'
		return 'myJsonpCallbcak({ \"status\": \"success\" })'

	@cherrypy.expose
	def stop(self, req_time=123):
		try:
			con = lite.connect('alarms.db')
			cur = con.cursor()
			cur.execute('select first_ring, ringing, completed from status where alarm_id=?', [str(req_time)])
			alarm_state = cur.fetchone()
			if alarm_state[1] is not 1:
				raise Exception('Alarm not ringing')

			cur.execute('update alarms set wake=wake+120 where alarm_id=?', [str(req_time)])
			cur.execute('update status set first_ring=0, ringing=0 where alarm_id=?', [str(req_time)])
			con.commit()
			con.close()

			cmd = "pkill -f \"ogg123 /home/Steven/Programs/test_alrm/alarm/ringtone.ogg --repeat "+req_time+"\""
			print str(cmd)
			os.system(cmd)

		except Exception as e:
			print e
			cherrypy.response.status = 500
			return 'myJsonpCallback({ \"error\":\"'+str(e)+'\")'
		return 'myJsonpCallbcak({ \"status\": \"success\" })'

	@cherrypy.expose
	def terminate(self, req_time=123):
		try:
			con = lite.connect('alarms.db')
			cur = con.cursor()
			cur.execute('update status set first_ring=1, ringing=0, completed=0 where alarm_id=?', [str(req_time)])
			cur.execute('update alarms set wake=prev_wake+cycle where alarm_id=?', [str(req_time)])
			con.commit()
			con.close()
			os.system("pkill -f \"ogg123 /home/Steven/Programs/test_alrm/alarm/ringtone.ogg --repeat "+req_time+"\"")
		except Exception as e:
			print e
			cherrypy.response.status = 500
			return 'myJsonpCallback({ \"error\":\"'+str(e)+'\")'
		return 'myJsonpCallbcak({ \"status\": \"success\" })'
	
	@cherrypy.expose
	def alarmList(self):
		try:
			con = lite.connect('alarms.db')
			cur = con.cursor()
			cur.execute('select A.alarm_id, A.wake, A.cycle, S.ringing from alarms as A, status as S where S.alarm_id=A.alarm_id order by A.wake')
			rows = cur.fetchall()
			data = {}
			alarms = []
			for row in rows:
				item = {}
				item['alarm_id'] = row[0]
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
		return 'myJsonpCallbcak({ \"status\": \"success\" })'

if __name__ == '__main__':
	cherrypy.quickstart(StringGenerator())
