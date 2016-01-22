import RPi.GPIO as GPIO
import sqlite3
import time
import sys
import daemon

sensor = 17

GPIO.setmode(GPIO.BCM)
GPIO.setup(sensor, GPIO.IN, GPIO.PUD_DOWN)

def read_state():
	try:
		conn = sqlite3.connect('/tmp/movement.db')
		cursor = conn.cursor()
		cursor.execute("PRAGMA journal_mode=OFF")
		cursor.execute("create table if not exists motion(timestamp datetime default CURRENT_TIMESTAMP primary key, movement int)")
	except sqlite3.Error, e:
		print "Error %s: " % e.args[0]
		sys.exit(1)

	while True:
		time.sleep(1)
		state = GPIO.input(sensor)
		print state
		cursor.execute("delete from motion where timestamp<datetime('now','-5 minute')")
		cursor.execute("insert into motion(movement) values(?)", str(state))
		conn.commit()

def run():
	with daemon.DaemonContext():
		read_state()

if __name__ == "__main__":
	run()
