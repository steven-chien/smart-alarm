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
	id = ring;
	wake_time = ring;
	pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
	sound = new sf::Sound();
	buffer = new sf::SoundBuffer();
	if (!buffer->loadFromFile("ringtone.ogg"))
		exit(1);
}

Alarm::~Alarm()
{
	//stop();
	delete buffer;
	delete sound;
}

void Alarm::set(time_t wake_time)
{
	this->wake_time = wake_time;
}

void *Alarm::wake(void *args)
{
	Alarm *alarm = (Alarm*)args;

	while(true) {
		/* prepare for sleep */
		struct timespec sleep;
		sleep.tv_sec = alarm->wake_time;
		sleep.tv_nsec = 0;

		/* sleep */
		std::cout << "going to sleep: " << sleep.tv_sec << std::endl;
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleep, NULL);

		/* sleep period ended, start ringing and wait for unlock */
		std::cout << "ringing..." << std::endl;
		alarm->sound->play();
		pthread_spin_lock(&(alarm->lock));

		/* unlocked, stop ringing and schedule for next wakeup */
		alarm->sound->stop();
		time_t now;
		time(&now);
		alarm->wake_time = now + 5;
	}
	return NULL;
}

void Alarm::start()
{
	std::cout << "hello" << std::endl;
	/* do not start if thread already started */
	if(!thread_state) {
		/* define sleep period */
		time(&start_time);
		sleep_period = wake_time - start_time;

		/* configure alarm ringtone */
		sound->setBuffer(*buffer);
		sound->setLoop(true);

		/* lock spin lock and start thread */
		pthread_spin_lock(&lock);
		pthread_create(&thread, NULL, wake, this);
		thread_state = true;
	}
}

void Alarm::stop()
{
	/* do not change state if thread is not started */
	if(thread_state) {
		std::cout << "snooze..." << std::endl;
		/* unlock to stop ringing */
		pthread_spin_unlock(&lock);
	}
}

void Alarm::terminate()
{
	if(thread_state) {
		std::cout << "term thread..." << std::endl;
		/* stop ringtone */
		//sound.stop();
		//sound->resetBuffer();

		/* set thread state to false, cancel thread, unlock spin lock */
		pthread_spin_unlock(&lock);
		pthread_cancel(thread);
		//thread_state = false;
	}
}
