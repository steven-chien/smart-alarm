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
#include <list>
#include <unistd.h>
#include <signal.h>
#include <onion/onion.h>
#include <onion/log.h>
#include <sqlite3.h>
#include "json/json.h"
#include "json/json-forwards.h"
#include "Alarm.hpp"

/* data structure for alarm clocks and web server */
std::list<std::string> alarm_list;
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
	std::cout.flush();
	/* get request flag */
	const onion_request_flags flags = onion_request_get_flags(req);

	try {
		if((flags & OR_METHODS)==OR_GET) {
			/* get parameters */
			const char *req_time =  onion_request_get_query(req, "time");
			const char *req_action =  onion_request_get_query(req, "action");
			const char *req_repeat = onion_request_get_query(req, "repeat");

			/* get current time */
			time_t now;
			time(&now);

			/* check if parameters are present */
			if(req_time!=NULL && req_action!=NULL) {
				/* convert c string to std string */
				std::string action(req_action);
				std::string wakeup_time(req_time);

				/* perform actions */
				if(action.compare("new")==0) {
					/* check if requested time has passed */
					if(now > atol(req_time))
						throw "Requested time smaller than current time!";

					/* instantiate new alarm and start sleeping */
					Alarm *alarm = alarm_clocks[wakeup_time];
					if(alarm!=NULL)
						throw "Requested clock does not exist!";

					/* create new alarm clock */
					alarm = new Alarm(atol(req_time));
					alarm->start();
					alarm_clocks[wakeup_time] = alarm;
					alarm_list.push_back(wakeup_time);
					alarm_list.sort();

					std::cout << "New alarm created!" << std::endl;

					/* write response */
					onion_response_printf(res, "<p>Alarm added</p>");
				}
				else if(action.compare("stop")==0) {
					/* look for requested alarm and check */
					Alarm *alarm = alarm_clocks[wakeup_time];
					if(alarm==NULL) {
						throw "Alarm not found!";
					}
					if(now<alarm->wake_time) {
						throw "Alarm not ringing, cannot stop!";
					}

					/* stop alarm */
					alarm->stop();
					std::cout << "Alarm " << wakeup_time << " stoped!" << std::endl;

					/* write response */
					onion_response_printf(res, "<p>Alarm stopped</p>");
				}
				else if(action.compare("terminate")==0) {
					/* look for requested alarm */
					Alarm *alarm = alarm_clocks[wakeup_time];
					if(alarm==NULL) {
						throw "Requested clock does not exist!";
					}

					/* cancel thread and delete object */
					alarm->terminate();
					delete alarm;

					/* reset data structure */
					alarm_clocks[wakeup_time] = NULL;
					alarm_list.remove(wakeup_time);
					std::cout << "Alarm " << wakeup_time << " terminated!" << std::endl;

					/* write response */
					onion_response_printf(res, "<p>Alarm deleted</p>");
				}
				else {
					throw "Action not found!";
				}
			}
			else if(req_action!=NULL) {
				/* convert c string to std string */
				std::string action(req_action);
				Json::Value jobj;
				Json::Value vec(Json::arrayValue);

				/* print list of alarm and send JSON object */
				if(action.compare("alarmList")==0) {
					std::list<std::string>::const_iterator iterator;
					for (iterator = alarm_list.begin(); iterator != alarm_list.end(); ++iterator) {
						Alarm *alarm = alarm_clocks[(*iterator)];
						Json::Value obj;
						obj["id"] = (*iterator);
						obj["wakeup"] = Json::Value((int)(alarm->wake_time));
						vec.append(obj);
					}
					jobj["alarms"] = vec;
					Json::FastWriter fastWriter;
					std::string jsonMsg = fastWriter.write(jobj);
					onion_response_printf(res, "%s", jsonMsg.c_str());
				}
			}
			else {
				throw "Acation not found!";
			}
		}
	}
	catch(char const* err) {
		std::cout << "Caught error: " << err << std::endl;
		goto fail;
	}
	std::cout.flush();
	return OCS_PROCESSED;
fail:
	onion_response_set_code(res, 400);
	onion_response_printf(res, "<p>Bad request!");
	std::cout.flush();
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
