// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.

// This sample illustrates how to use EMG data. EMG streaming is only supported for one Myo at a time.

#define _USEGRAPHICS true

#include <array>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <ctime>
#include <sys/timeb.h>

#include <fstream>

#include <myo/myo.hpp>

// For graphing
#include "matplot\include\matplotpp.h"
#include <conio.h>
#include <Windows.h>

class DataCollector : public myo::DeviceListener {
public:
	std::array <int8_t, 8> emgSamples; // THIS IS ABSOLUTELY REQUIRED. THE MYO's sample code does not have this!
	std::vector<double> emgSamples_vector;
	std::vector<std::vector<double>> emg_break;
	std::ofstream data_file;
	myo::Pose currentPose;
	bool onArm;
	time_t currentTime;
	struct tm localTime;

	DataCollector()
		: onArm(false), currentPose(), emgSamples(), emgSamples_vector(8), emg_break(8)
	{
		
	}

	// onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
	void onUnpair(myo::Myo* myo, uint64_t timestamp)
	{
		// We've lost a Myo.
		// Let's clean up some leftover state.
		emgSamples.fill(0);
		onArm = false;
		this->data_file.close();
	}

	// onPose
	void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose) {
		currentPose = pose;
		if (pose != myo::Pose::unknown && pose != myo::Pose::rest) {
			myo->unlock(myo::Myo::unlockHold);

			myo->notifyUserAction();
		}
		else {
			myo->unlock(myo::Myo::unlockTimed);
		}
	}

	// onEmgData() is called whenever a paired Myo has provided new EMG data, and EMG streaming is enabled.
	void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
	{
		for (int i = 0; i < 8; i++) {
			emgSamples[i] = emg[i];
			emgSamples_vector[i] = (double) emg[i];
			emg_break[i].push_back((double)emg[i]);
		}
	}

	// onArmSync() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
	// arm. This lets Myo know which arm it's on and which way it's facing.
	void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection)
	{
		onArm = true;
		this->data_file.open("Myo_EMG_data.csv");
		
		this->data_file << "id,#0,#1,#2,#3,#4,#5,#6,#7,time,millisecond\n";
	}

	// onArmUnsync() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
	// it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
	// when Myo is moved around on the arm.
	void onArmUnsync(myo::Myo* myo, uint64_t timestamp)
	{
		onArm = false;
		this->data_file.close();
	}

	// There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
	// For this example, the functions overridden above are sufficient.

	// We define this function to print the current values that were updated by the on...() functions above.
	void print()
	{
		// Clear the current line
		//std::cout << '\r';

		if (onArm) {
			// Print out the lock state, the currently recognized pose, and which arm Myo is being worn on.

			// Pose::toString() provides the human-readable name of a pose. We can also output a Pose directly to an
			// output stream (e.g. std::cout << currentPose;). In this case we want to get the pose name's length so
			// that we can fill the rest of the field with spaces below, so we obtain it as a string using toString().
			std::string poseString = currentPose.toString();

			//std::cout << '[' << poseString << std::string(14 - poseString.size(), ' ') << ']';
			this->data_file << match_pose_id(currentPose) << ",";
		}
		else {
			// Print out a placeholder for the arm and pose when Myo doesn't currently know which arm it's on.
			//std::cout << '[' << std::string(8, ' ') << ']' << "[?]" << '[' << std::string(14, ' ') << ']';
			this->data_file << match_pose_id(currentPose) << ',';
		}

		// Print out the EMG data.
		for (size_t i = 0; i < emgSamples.size(); i++) {
			std::ostringstream oss;
			oss << static_cast<int>(emgSamples[i]);
			std::string emgString = oss.str();

			//std::cout << '(' << emgString << std::string(4 - emgString.size(), ' ') << ')';
			this->data_file << emgString << ',';
		}
		//std::cout << std::endl;
		this->data_file << time_stamp() << ',' << getMilliCount() << ' ';
		this->data_file << "\n";

		//std::cout << std::flush;

	}

	int match_pose_id(myo::Pose pose) {
		if (pose == myo::Pose::unknown) {
			return -1;
		}
		else if (pose == myo::Pose::rest) {
			return 0;
		}
		else if (pose == myo::Pose::doubleTap) {
			return 1;
		}
		else if (pose == myo::Pose::fingersSpread) {
			return 2;
		}
		else if (pose == myo::Pose::waveIn) {
			return 3;
		}
		else if (pose == myo::Pose::waveOut) {
			return 4;
		}
		else if (pose == myo::Pose::fist) {
			return 5;
		}
		else {
			return -2; // equivolent to ?
		}
	}

	std::string time_stamp() {
		time(&currentTime); 
		localtime_s(&localTime, &currentTime);

		int Day = localTime.tm_mday;
		int Month = localTime.tm_mon + 1;
		int Year = localTime.tm_year + 1900;
		int Hour = localTime.tm_hour;
		int Min = localTime.tm_min;
		int Sec = localTime.tm_sec;

		std::stringstream strstream; 
		strstream << Hour << ':' << Min << ':' << Sec << '@' << Day << '/' << Month << '/' << Year;

		std::string time_stamp;
		strstream >> time_stamp;

		return time_stamp;
	}

	int getMilliCount(){
		timeb tb;
		ftime(&tb);
		int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
		return nCount;
	}

};

extern myo::Hub hub("com.example.emg-data-sample");

class MyoPlot :public MatPlot{

public:

	myo::Myo *m_myo;
	DataCollector collector;

	std::vector<double> *emg;
	std::vector<double> emg0;
	std::vector<double> emg1;
	std::vector<double> emg2;
	std::vector<double> emg3;
	std::vector<double> emg4;
	std::vector<double> emg5;
	std::vector<double> emg6;
	std::vector<double> emg7;
	std::array<bool, 8> iemg;

	int c_type;
	bool emg_updated;
	bool emg_break_updated;

	int len;

	long long last_mili; // last execution mili time
	double freq;

	MyoPlot()
		: MatPlot(), emg_updated(false), c_type(-1), emg_break_updated(false), iemg(), last_mili(-1), freq(-1)
	{
		iemg.fill(false);

		// We catch any exceptions that might occur below -- see the catch statement for more details.
		try {

			// First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
			// publishing your application. The Hub provides access to one or more Myos.

			std::cout << "Attempting to find a Myo..." << std::endl;

			// Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
			// immediately.
			// waitForMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
			// if that fails, the function will return a null pointer.
			m_myo = hub.waitForMyo(10000);

			// If waitForMyo() returned a null pointer, we failed to find a Myo, so exit with an error message.
			if (!m_myo) {
				throw std::runtime_error("Unable to find a Myo!");
			}

			// We've found a Myo.
			std::cout << "Connected to a Myo armband!" << std::endl << std::endl;

			// Next we enable EMG streaming on the found Myo.
			m_myo->setStreamEmg(myo::Myo::streamEmgEnabled);

			// Next we construct an instance of our DeviceListener, so that we can register it with the Hub.

			// Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
			// Hub::run() to send events to all registered device listeners.
			hub.addListener(&collector);


			// If a standard exception occurred, we print out its message and exit.
		}
		catch (const std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;
			std::cerr << "Press enter to continue.";
			std::cin.ignore();

			exit(0);
		}

	}

	void DISPLAY(){

		layer("My Plot", 1);
		// Everything before the next subplot() will be on this subplot
		int h = subplot(2, 5, 1);  // first plot -- current EMG
		
		axis(0, 9, -100, 100);
		std::ostringstream ss;
		ss << freq;
		std::string freq_str = ss.str();
		text(1, 1, "Current FF: " + freq_str);
		if (emg_updated) {
			bar(*emg, 1);
			emg_updated = false;
		}
		
		//if (c_type != -1) { // current type is not 'unknown'
		h = subplot(2, 5, 2); // second subplot -- REAL TIME EMG
		axis(0, 25, -100, 100);
		text(1, 1, "#1");
		if (iemg[0]) {
			bar(emg0, 1);
			iemg[0] = false;
		}
		
		h = subplot(2, 5, 3); // third subplot -- REAL TIME EMG for doubleTap
		axis(0, 25, -100, 100);
		text(1, 1, "#2");
		if (iemg[1]) {
			bar(emg1, 1);
			iemg[1] = false;
		}
		
		h = subplot(2, 5, 4); // fourth subplot 
		axis(0, 25, -100, 100);
		text(1, 1, "#3");
		if (iemg[2]) {
			bar(emg2, 1);
			iemg[2] = false;
		}
		
		h = subplot(2, 5, 5); // fifth subplot 
		axis(0, 25, -100, 100);
		text(1, 1, "#4");
		if (iemg[3]) {
			bar(emg3, 1);
			iemg[3] = false;
		}

		h = subplot(2, 5, 6); // sixth subplot
		axis(0, 25, -100, 100);
		text(1, 1, "#5");
		if (iemg[4]) {
			bar(emg4, 1);
			iemg[4] = false;
		}

		h = subplot(2, 5, 7); // seventh subplot
		axis(0, 25, -100, 100);
		text(1, 1, "#6");
		if (iemg[5]) {
			bar(emg5, 1);
			iemg[5] = false;
		}

		h = subplot(2, 5, 8); // eighth subplot
		axis(0, 25, -100, 100);
		text(1, 1, "#7");
		if (iemg[6]) {
			bar(emg6, 1);
			iemg[6] = false;
		}
		
		h = subplot(2, 5, 9); // eighth subplot
		axis(0, 25, -100, 100);
		text(1, 1, "#8");
		if (iemg[7]) {
			bar(emg7, 1);
			iemg[7] = false;
		}
	}

	// type:
	// 0 for rest
	// 1 for doubleTap
	// 2 for fingerSpread
	// 3 for waveIn
	// 4 for waveOut
	// 5 for fist
	// -1 for unknown
	void feed_emg(std::vector<double> *emg_src) {
		emg = emg_src;
		emg_updated = true;

		if (emg0.size() >= 25) {
			emg0.clear();
		}
		emg0.push_back(emg_src->at(0));
		iemg[0] = true;
		
		if (emg1.size() >= 25) {
			emg1.clear();
		}
		emg1.push_back(emg_src->at(1));
		iemg[1] = true;
		
		if (emg2.size() >= 25) {
			emg2.clear();
		}
		emg2.push_back(emg_src->at(2));
		iemg[2] = true;

		if (emg3.size() >= 25) {
			emg3.clear();
		}
		emg3.push_back(emg_src->at(3));
		iemg[3] = true;
		
		if (emg4.size() >= 25) {
			emg4.clear();
		}
		emg4.push_back(emg_src->at(4));
		iemg[4] = true;

		if (emg5.size() >= 25) {
			emg5.clear();
		}
		emg5.push_back(emg_src->at(5));
		iemg[5] = true;


		if (emg6.size() >= 25) {
			emg6.clear();
		}
		emg6.push_back(emg_src->at(6));
		iemg[6] = true;


		if (emg7.size() >= 25) {
			emg7.clear();
		}
		emg7.push_back(emg_src->at(7));
		iemg[7] = true;
		
	}

	// feed in time the loop is executed again;
	void feed_time(long long mili) {
		if (last_mili == -1) {
			last_mili = mili;
		}
		else {
			// diff = mili - last_mili = miliseconds for one execution (1 exec / diff miliseconds)
			// 1 / diff = M executions / millisecond
			// M * 1000 = FF executions / second
			double diff = (double)mili - (double)last_mili;
			freq = 1.0 / diff * 1000;
			last_mili = mili;
		}
	}
};

extern MyoPlot *mp = new MyoPlot();

// function for glutDisplayFunc to call back
void display(){ 


	// Finally we enter our main loop.
	try {
		while (1) {
			mp->feed_time(mp->collector.getMilliCount()); // it is a bit wierd to pass such a parameter - getMilliCount may ought not to be member function of Collecor
			std::vector<double> *emg = &mp->collector.emgSamples_vector;
			mp->feed_emg(emg);
			//mp->feed_emg_break(emg_break);
			mp->display();
			// In each iteration of our main loop, we run the Myo event loop for a set number of milliseconds.
			// In this case, we wish to update our display 50 times a second, so we run for 1000/20 milliseconds.
			hub.run(1000 / 100);
			// After processing events, we call the print() member function we defined above to print out the values we've
			// obtained from any events that have occurred.
			mp->collector.print();
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();

		exit(0);
	}
}
// reshape
void reshape(int w, int h){ 
	mp->reshape(w, h); 
}

int main(int argc, char** argv)
{

	bool graphics_exit = FALSE;

	std::cout << "Main:>\t Starting graphics" << std::endl;
	
	glutInit(&argc, argv);
	glutCreateWindow(0, 0, 1800, 1000);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();

	
	// Done.


}
