/*
 * =====================================================================================
 *
 *       Filename:  alarm.cpp
 *
 *    Description:  Alarm object handling timed sleep and motion detection check
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

Alarm::Alarm(time_t ring, int repeat_cycle)
{
	thread_state = false;
	id = ring;
	wake_time = ring;
	cycle = repeat_cycle;
	pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
}

Alarm::~Alarm()
{

}

void Alarm::set(time_t wake_time)
{
	this->wake_time = wake_time;
}

void *Alarm::wake(void *args)
{
	Alarm *alarm = (Alarm*)args;
	alarm->prev_wake = alarm->wake_time;
	bool first_ring = true;

	while(true) {
		/* prepare for sleep */
		struct timespec sleep;
		sleep.tv_sec = alarm->wake_time;
		sleep.tv_nsec = 0;

		/* sleep */
		std::cout << "going to sleep: " << sleep.tv_sec << std::endl;
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleep, NULL);

		/* if movement detected during snooze period, stop, do not activate if it's the 1st ring */
		FILE *in;
		char buff[512];
		in = popen("cat /tmp/raw_state.dat | tail -n50 | paste -sd+ | bc", "r");
		fgets(buff, sizeof(buff), in);
		pclose(in);
		if(atoi(buff)>5 && first_ring==false) {
			alarm->wake_time = alarm->prev_wake + alarm->cycle;
			
			std::cout << "Movement detected, stop alarm, next wake " << alarm->wake_time << std::endl;
			first_ring = true;
			continue;
		}
		if(first_ring==true)
			alarm->prev_wake = alarm->wake_time;

		/* sleep period ended, start ringing and wait for unlock */
		std::cout << "ringing..." << std::endl;
		char cmd[512];
		sprintf(cmd, "ogg123 ringtone.ogg --repeat %ld > /dev/null 2>&1 &", alarm->id);
		system(cmd);
		pthread_spin_lock(&(alarm->lock));

		/* unlocked, stop ringing and schedule for next wakeup */
		char kill_cmd[512];
		sprintf(kill_cmd, "pkill -f \"ogg123 ringtone.ogg --repeat %ld\"", alarm->id);
		system(kill_cmd);
		first_ring = false;

		/* snooze period */
		time_t now;
		time(&now);
		alarm->wake_time = now + 300;
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleep, NULL);

	}
}

void Alarm::start()
{
	std::cout << "hello" << std::endl;
	/* do not start if thread already started */
	if(!thread_state) {
		/* define sleep period */
		time(&start_time);
		sleep_period = wake_time - start_time;
		
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
		/* set thread state to false, cancel thread, unlock spin lock */
		pthread_spin_unlock(&lock);
		pthread_cancel(thread);
		sprintf(kill_cmd, "pkill -f \"ogg123 ringtone.ogg --repeat %ld\"", alarm->id);
	}
}
