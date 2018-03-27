#define THRES 50

#include "mymosq.h"
#include "util.h"
#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaobjdetect.hpp>
#include <chrono>
#include <thread>

using namespace cv;
using namespace std;
using namespace std::chrono;

unsigned int count;
bool faceflag, alarmflag;

void task1(string msg)
{
	std::cout << "Task 1: " << std::this_thread::get_id() << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(50));
	exec(msg.c_str());
}

std::vector<std::string> split (const std::string input,char delim)
{
	std::vector<std::string> tokens;
	std::string word;
    std::stringstream stream(input);
    while( getline(stream, word, delim) ){
        tokens.push_back(word);
	}
	return tokens;
}

void test(std::string t){	// input format:{"formatType":"1","other":"$BLE04;AD54B847;171204161613;1;SS00117D8D7E070,-59,99,2,6414;200#"}
	cout << "Get: " << t << endl;
	int val = 0;
	std::vector<std::string> vect;
	vect = split(t,';');				//cout<<"Size : " << vect.size() << vect.at(4) << endl; // vect.at(4) => SS00117D8D7E070,-59,99,2,6414
	if (vect.size() > 5){																					//	^ this is Sensor ID		  ^ sensor value
		vect=split(vect.at(4),',');		// split vect.at(4) by "," into 4 elements where .at(0) is sensor ID and .at(4) is value of sensor
		if ((vect.size() > 4) & (!strcmp(vect.at(0).c_str(), SENSORID)))
		{
			val = std::stoi(vect.at(4));	cout<<"Val: " << val << endl;			
			if( (val > THRES) && faceflag ){
				alarmflag = true;
			}
		}
	}

	//for(int i=0; i < vect.size(); i++){
	//	std::cout << vect.at(i) << std::endl;
	//};
}
int main(void)
{
	class mymosq *testmosq;
	int rc = 0;
	::count = 0;
	::faceflag = false;
	::alarmflag = false;
	std::string input;
	//high_resolution_clock::time_point t1, t2;
	std::cout << "Main : " << std::this_thread::get_id() << std::endl;
    //CascadeClassifier haar_cascade;
	std::time_t result = std::time(nullptr);
    std::cout << std::asctime(std::localtime(&result)) << std::endl;// << result << " seconds since the Epoch\n";
	cout << "Cuda device:";
    cuda::printShortCudaDeviceInfo(cuda::getDevice());
    cout << endl;
	cv::Ptr<cv::cuda::CascadeClassifier> haar_cascade = cv::cuda::CascadeClassifier::create("haarcascade_frontalface_alt.xml");
    if(haar_cascade->empty()) {
        cerr << "Can't load gpu cascade from file" << endl;
        return -1;
    }

    //std::cout << "URL: ";
    //std::getline(std::cin, input);
    //if(input == "") input = URL;
	cout << "MQTT server: " << SERVER << \
			" port: " << PORT << std::endl << \
		 	"Subscribe topic: " << GWTOPIC << std::endl << \
			"Publish topic: " << ALARMTOPIC << std::endl << \
			"Sensor ID: " << SENSORID << std::endl;
    std::cout << "Connecting URL: " << URL << std::endl;
    cv::VideoCapture cap(URL, cv::VideoCaptureAPIs::CAP_GSTREAMER);
    if( !cap.isOpened() )
    {
        std::cout << "Not good, open camera failed" << std::endl;
        std::cout << "URL: " << input << std::endl;
        return 0;
    }
    //cap.set(cv::CAP_PROP_FPS, 20);
    std::cout << "FPS: " << cap.get(cv::CAP_PROP_FPS) << std::endl;
    cv::Mat frame;
	mosqpp::lib_init();
	testmosq = new mymosq("testmosq", SERVER, PORT);
	testmosq->addHandler(&test);
	testmosq->loop_start();
	while(1)
	{
		//t1 = high_resolution_clock::now();
		cap >> frame;
		if( frame.empty() ) break;
		Mat crop;
		cv::cuda::GpuMat gpuMat, grayMat, gpuFound;
		Rect roi(POS_X, POS_Y, WIDTH, HEIGHT);
		//rectangle(frame, roi, CV_RGB(255, 0, 0), 2);
		frame(roi).copyTo(crop);
		gpuMat.upload(crop);
		vector< Rect_<int> > faces;
		cv::cuda::cvtColor(gpuMat, grayMat, cv::COLOR_BGR2GRAY);
		haar_cascade->setFindLargestObject(false);
		haar_cascade->setScaleFactor(1.1);
		haar_cascade->setMinNeighbors(2);
		haar_cascade->setMinObjectSize(cv::Size(20,20));
		haar_cascade->setMaxObjectSize(cv::Size(40,40));	
		//cout << "Max size: " << haar_cascade->getMaxObjectSize() << std::endl;
		haar_cascade->detectMultiScale(grayMat, gpuFound);
		haar_cascade->convert(gpuFound, faces);
		for(unsigned int i = 0; i < faces.size(); i++){
		    Rect face_i = faces[i];
			if(faces[i].width < 40){
				if(::faceflag) rectangle(crop, face_i, CV_RGB(250,0 ,0), 2);	//Draw a rectangle for every detected face
				else rectangle(crop, face_i, CV_RGB(::count*5, 200 - (::count*5), 0), 2);	//std::cout << "Size: " << faces[i].width << " ";
			}
			
		}
		if(faces.size()){
			::count++;
			if(::count >= 5){
				if(::count >= 40) ::count = 40;	
			}
		}
		else{
			if (::count > 0) ::count--;
		}
		if(::count < 5) ::faceflag = false;
		else ::faceflag = true;
		//cout << "Count: " << ::count << " Flag: " << ::faceflag << " ";
		imshow("Live streaming...", crop);
		char c = (char)waitKey(60);
		if(alarmflag){
			std::cout << "ALARM!" << std::endl;
			testmosq->publish(PUB_MESSAGE);
			if(checkrecord()){				
				thread tak1(task1, "ls -t /home/bvbd/vid/rec1/ | xargs sh ~/join.sh &");
				tak1.detach();
			}
			else std::cout << "Error: Detected event but recording service is not running" << std::endl;
			alarmflag = false;
		}
			/*rc= testmosq->loop();
			if(rc){
				testmosq->reconnect();
			}*/
		//t2 = high_resolution_clock::now();
		// Time elapsed
		//duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
		//std::cout << "Faces :" << faces.size() << " Time: " << time_span.count() << " seconds." << std::endl;
	}
	testmosq->disconnect();
	testmosq->loop_stop();
	mosqpp::lib_cleanup();
	return 0;
}
