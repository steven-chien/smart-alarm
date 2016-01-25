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
//#include <SFML/Audio.hpp>

Alarm::Alarm(time_t ring)
{
	thread_state = false;
	id = ring;
	wake_time = ring;
	pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
	//sound = new sf::Sound();
	//buffer = new sf::SoundBuffer();
	//if (!buffer->loadFromFile("ringtone.ogg"))
	//	exit(1);
}

Alarm::~Alarm()
{
	//stop();
	//delete buffer;
	//delete sound;
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
		//alarm->sound->play();
		system("canberra-gtk-play --file=ringtone.ogg --loop 1000 &");
		pthread_spin_lock(&(alarm->lock));

		/* unlocked, stop ringing and schedule for next wakeup */
		//alarm->sound->stop();
		system("pkill -f canberra-gtk-play");

		time_t now;
		time(&now);
		alarm->wake_time = now + 15;
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleep, NULL);

		/*  Execute SQL statement */
		char movement_detect;
		sqlite3_stmt *statement;
		if(sqlite3_prepare_v2(alarm->db, "select sum(movement) from motion", -1, &statement, 0) == SQLITE_OK) {
			int cols = sqlite3_column_count(statement);
			int result = 0;
			result = sqlite3_step(statement);
			movement_detect = *((char*)sqlite3_column_text(statement, 0));
			sqlite3_finalize(statement);
		}
		else {
			std::string error = sqlite3_errmsg(alarm->db);
			std::cout << "sqlite error " << error << std::endl;
			std::cout.flush();
		}
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
		//sound->setBuffer(*buffer);
		//sound->setLoop(true);
		
		/* configure sqlite3 */
		sqlite3_open("test.db", &db);

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
