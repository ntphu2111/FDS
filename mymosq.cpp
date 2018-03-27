#include <cstdio>
#include <cstring>
#include "mymosq.h"
#include <mosquittopp.h>

mymosq::mymosq(const char *id, const char *host, int port) : mosquittopp(id)
{
	
	int keepalive = 60;
	/* Connect immediately. This could also be done by calling
	 * mqtt_tempconv->connect(). */
	connect(host, port, keepalive);
};

mymosq::~mymosq(){}

void mymosq::on_connect(int rc)
{
	printf("MQTT Connected with code %d.\n", rc);
	if(rc == 0)
	{
		/* Only attempt to subscribe on a successful connect. */
		subscribe(NULL, GWTOPIC);
	}
	
}

void mymosq::on_message(const struct mosquitto_message *message)
{
	char buf[BUFSIZE];
	if(!strcmp(message->topic, GWTOPIC)){
		memset(buf, 0, BUFSIZE*sizeof(char));
		/* Copy N-1 bytes to ensure always 0 terminated. */
		memcpy(buf, message->payload, 200*sizeof(char));
		//printf("Buffer: %s\n", buf);
		tempfunct(buf);
		//publish(NULL, "temperature/farenheit", strlen(buf), buf);
	}
}

void mymosq::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
	//printf("MQTT Subscription succeeded topic: %s", GWTOPIC);
}

void mymosq::addHandler(std::function<void(std::string)> callback)
{
	//printf("Added callback");
	tempfunct = callback;
}

void mymosq::publish(std::string message){
	char buf[BUFSIZE];
	char *cstr = new char[message.length() + 1];
	strcpy(cstr, message.c_str());
	mosquittopp::publish(NULL, ALARMTOPIC, strlen(cstr), cstr);
}
