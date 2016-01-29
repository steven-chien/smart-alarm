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
import os
import time
import sys

sensor = 17

GPIO.setmode(GPIO.BCM)
GPIO.setup(sensor, GPIO.IN, GPIO.PUD_DOWN)

while True:
	time.sleep(1)
	state = GPIO.input(sensor)
	if state==1:
		os.system("curl http://localhost:8080/\"delete?req_time=`ps -ef | grep ogg123 |head -n 1 | awk '{print $NF}'`\"")
		break
