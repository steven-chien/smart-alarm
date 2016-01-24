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
#include <unordered_map>
#include <unistd.h>
#include <signal.h>
#include <onion/onion.h>
#include <onion/log.h>
#include "Alarm.hpp"

/* data structure for alarm clocks and web server */
std::unordered_map<std::string, Alarm*> alarm_clocks;
onion *o = NULL;

/* gradefully shutdown server */
void shutdown_server(int _)
{
	if(o)
		onion_listen_stop(o);
}

/* parse HTTP request */
int parse_request(void *p, onion_request *req, onion_response *res)
{
	/* get request flag */
	const onion_request_flags flags = onion_request_get_flags(req);

	/* check if request is get */
	if((flags & OR_METHODS)==OR_GET) {
		/* get parameters */
		const char *req_time =  onion_request_get_query(req, "time");
		const char *req_action =  onion_request_get_query(req, "action");

		/* check if parameters are present */
		if(req_time!=NULL && req_action!=NULL) {
			/* convert c string to std string */
			std::string action(req_action);
			std::string wakeup_time(req_time);
			std::cout << "Received action request: " << action << " at time: " << wakeup_time << std::endl;
			/* perform actions */
			if(action.compare("new")==0) {
				/* check if requested time has passed */
				time_t now;
				time(&now);
				if(now > atol(req_time))
					goto fail;

				/* instantiate new alarm and start sleeping */
				Alarm *alarm = new Alarm(atol(req_time));
				alarm->start();
				alarm_clocks[wakeup_time] = alarm;
			}
			else if(action.compare("stop")==0) {
				Alarm *alarm = alarm_clocks[wakeup_time];
			 	if(alarm==NULL) {
					std::cout << "Alarm clock " << wakeup_time << " not found!" << std::endl;
					goto fail;
				}
				alarm->stop();
			}
			else if(action.compare("terminate")==0) {
				Alarm *alarm = alarm_clocks[wakeup_time];
		  		if(alarm==NULL) {
					std::cout << "Alarm clock " << wakeup_time << " not found!" << std::endl;
					goto fail;
				}
				alarm->terminate();
				delete alarm;
				alarm_clocks[wakeup_time] = NULL;
			}
		}

		/* write response */
		onion_response_printf(res, "<p>Client description: %s, %s, %s", onion_request_get_client_description(req), req_action, req_time);
	}
	else {
		goto fail;
	}
	return OCS_PROCESSED;
fail:
	onion_response_set_code(res, 400);
	onion_response_printf(res, "<p>Bad request!");
	return OCS_PROCESSED;
}

int main(int argc, char *argv[])
{
	/* install signal handlers for shutdown */
	signal(SIGINT,shutdown_server);
	signal(SIGTERM,shutdown_server);

	/* configure new server */
	o=onion_new(O_POOL);
	onion_set_timeout(o, 5000);
	onion_set_hostname(o,"0.0.0.0");
	onion_set_port(o, "8888");
	onion_url *urls=onion_root_url(o);

	/* add request pattern */
	onion_url_add(urls, "", (void*)parse_request);
	onion_url_add(urls, "^(.*)$", (void*)parse_request);

	/* start listening */
	onion_listen(o);
	onion_free(o);

	return 0;
}
