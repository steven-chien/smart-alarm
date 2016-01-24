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

class Alarm
{
	public:
		time_t wake_time;
		time_t sleep_period;
		time_t start_time;
		std::thread thread;
		Alarm(time_t);
		~Alarm();
		static void wake(Alarm*);
		void start();
		void stop();
};
