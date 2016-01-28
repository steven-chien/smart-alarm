import RPi.GPIO as GPIO
import time
import sys
import daemon

sensor = 17

GPIO.setmode(GPIO.BCM)
GPIO.setup(sensor, GPIO.IN, GPIO.PUD_DOWN)
i=0

def read_state():
	while True:
		time.sleep(1)
		state = GPIO.input(sensor)
		print state
		f = open('/tmp/raw_state.dat', 'a')
		f.write(str(state))
		f.write('\n')
		f.close()

def run():
	with daemon.DaemonContext():
		read_state()

if __name__ == "__main__":
	run()
