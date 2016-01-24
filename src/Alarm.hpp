/*
 * =====================================================================================
 *
 *       Filename:  alarm.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/24/2016 04:44:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Steven Chien (steven), steven.chien@connect.polyu.hk
 *   Organization:  
 *
 * =====================================================================================
 */
#include <thread>
#include <ctime>
#include <pthread.h>
#include <SFML/Audio.hpp>

class Alarm
{
	private:
		pthread_t thread;
		bool thread_state;
		sf::SoundBuffer buffer;
		sf::Sound sound;
		pthread_spinlock_t lock;
		time_t wake_time;
		time_t sleep_period;
		time_t start_time;
		static void *wake(void*);
	public:
		Alarm(time_t);
		~Alarm();
		void start();
		void stop();
		void set(time_t);
};
