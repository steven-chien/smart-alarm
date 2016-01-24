/*
 * =====================================================================================
 *
 *       Filename:  alarm.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/24/2016 04:46:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Steven Chien (steven), steven.chien@connect.polyu.hk
 *   Organization:  
 *
 * =====================================================================================
 */

#include <iostream>
#include <thread>
#include <ctime>
#include <time.h>
#include <unistd.h>
#include "Alarm.hpp"
#include <SFML/Audio.hpp>

Alarm::Alarm(time_t ring)
{
	thread_state = false;
	wake_time = ring;
	pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
	if (!buffer.loadFromFile("ringtone.ogg"))
		exit(1);
}

Alarm::~Alarm()
{
	stop();
}

void Alarm::set(time_t wake_time)
{
	this->wake_time = wake_time;
}

void *Alarm::wake(void *args)
{
	Alarm *alarm = (Alarm*)args;

	struct timespec sleep;
	sleep.tv_sec = alarm->wake_time;
	sleep.tv_nsec = 0;

	clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleep, NULL);
	std::cout << "ringing..." << std::endl;
	alarm->sound.play();
	pthread_spin_lock(&(alarm->lock));
	pthread_spin_unlock(&(alarm->lock));
//	while(alarm->thread_state);
	return NULL;
}

void Alarm::start()
{
	if(!thread_state) {
		time(&start_time);
		sleep_period = wake_time - start_time;

		sound.setBuffer(buffer);
		sound.setLoop(true);

		pthread_spin_lock(&lock);
		pthread_create(&thread, NULL, wake, this);
		thread_state = true;
	}
}

void Alarm::stop()
{
	if(thread_state) {
		std::cout << "cancel thread..." << std::endl;
		sound.stop();
		sound.resetBuffer();
		thread_state = false;
		pthread_cancel(thread);
		pthread_spin_unlock(&lock);
	}
}
