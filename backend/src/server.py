import sqlite3 as lite
import time
import random
import string
import os

import cherrypy

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
			return '<p>'+str(e)+'</p>'

		return str(req_time)

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
			return '<p>'+str(e)+'</p>'

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
			return '<p>'+str(e)+'</p>'

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
			return '<p>'+str(e)+'</p>'



if __name__ == '__main__':
	cherrypy.quickstart(StringGenerator())
