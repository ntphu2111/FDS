#define BUFSIZE 201
#define SERVER "113.161.61.89"
#define PORT 41883
#define GWTOPIC "/SCPCloud/DEVICE/AD54B847"		//Where to collect sensor data
#define ALARMTOPIC "/SCPCloud/DEVICE/509a4c5912d3"	//Where to push warning to server
#define SENSORID " SS000DE0A10D190"				// ID of sound sensor
//#define URL "rtsp://admin:123456@10.4.1.233/profile1"
//#define URL "rtsp://10.4.1.231/axis-media/media.amp"
//#define URL "uridecodebin uri=rtsp://10.4.1.231/axis-media/media.amp ! videoconvert ! videoscale ! appsink"
//#define URL "rtspsrc location=\"rtsp://admin:123456@10.4.1.233/profile1\" latency=0 ! decodebin ! videorate ! video/x-raw,framerate=15/1 ! videoconvert ! appsink" 
#define URL "rtspsrc location=\"rtsp://admin:123456@10.70.124.231/profile1\" latency=0 ! decodebin ! videoconvert ! appsink"
#define PUB_MESSAGE "{\"urlCam\":\"http://10.70.124.230:10090\"}"
#define POS_X 500
#define POS_Y 150
#define WIDTH 800
#define HEIGHT 400

#include <mosquittopp.h>
#include <functional>

class mymosq : public mosqpp::mosquittopp
{
	public:
	  	mymosq(const char *id, const char *host, int port);
	 	~mymosq();

		std::function<void(std::string)> tempfunct;
		void on_connect(int rc);
		void on_message(const struct mosquitto_message *message);
		void on_subscribe(int mid, int qos_count, const int *granted_qos);
		void addHandler(std::function<void(std::string)> callback);
		void publish(std::string message);
};
