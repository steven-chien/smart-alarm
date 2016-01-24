/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/24/2016 04:53:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Steven Chien (steven), steven.chien@connect.polyu.hk
 *   Organization:  
 *
 * =====================================================================================
 */

#include <iostream>
#include <unistd.h>
#include "Alarm.hpp"

int main(int argc, char *argv[])
{
	std::cout << "hello" << std::endl;
	time_t now;
	time(&now);
	Alarm *alarm = new Alarm(now+5);
	alarm->start();
	sleep(8);
	alarm->stop();
	sleep(3);
	time(&now);
	alarm->set(now+3);
	alarm->start();
	sleep(6);
	alarm->stop();
	sleep(1);
	time(&now);
	alarm->set(now+5);
	alarm->start();
	sleep(10);
	alarm->stop();
	delete alarm;
}
