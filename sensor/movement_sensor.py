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
