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
#include <unistd.h>
#include "alarm.hpp"

Alarm::Alarm(time_t ring)
{
	wake_time = ring;
}

Alarm::~Alarm()
{

}

void Alarm::wake(Alarm *alarm)
{
	std::this_thread::sleep_for (std::chrono::seconds(alarm->sleep_period));
	std::cout << "ringing..." << std::endl;
}

void Alarm::start()
{
	time(&start_time);
	sleep_period = wake_time - start_time;
	thread = std::thread(wake, this);
}

void Alarm::stop()
{

}
