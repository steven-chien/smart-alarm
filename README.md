# Smart Alarm

An alarm clock that uses PIR sensor to make sure that you have got out of your room before stopping. Intended to run on Raspberry Pi.

### Software Dependencies
  - CherryPy
  - GPIO
  - python-daemon

### Running the sensor daemon
> sudo python movement_sensor.py

### Running the server
> python server.py (abs path of the folder you put smart-alarm)

### Alarm clock
Go to your browser and type:
> localhost:8080/

### Tech
Smart alarm uses the following libraires and hardware:

* [Bootstrap] - CSS library
* [CherryPy] - A Minimalist Python Web Framework
* [Python GPIO] - The library used for reading GPIO pins on the Pi
* [ogg123] - A Linux Ogg Vorbis audio player
* [jQuery] - A cross-platform JavaScript library

### Installation
Install Vorbis tools
> # apt-get install vorbis-tools

On Mac use hombrew
> brew install vorbis-tools

Put the files somewhere, fixes all the absolute paths.Call the wakeup.py every minute with crontab:
> \* \* \* \* \* /usr/bin/python (abs path to your location)/wakeup.py (abs path to your location) >> /tmp/wakeup.log 2>&1 &

To limit the size of the sensor log file to most recent 300 seconds, add the following oneliner to cron:
> sed -e :a -e '$q;N;301,$D;ba' -i /tmp/raw_states.dat

### Plugins
Install CherryPi with:
> sudo pip install cherrypy

### Version
v0.1

### License
Copyright (C) 2016 Anson Kwan, Carson Yan, Star Poon and Steven Chien

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
