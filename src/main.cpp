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

// utils for reading config file
#include "utils.h"
#include "config.h"


double emgSamples_arr[mcfg::SAMPLE_SIZE]; // for current
double emgv[mcfg::SAMPLE_SIZE][mcfg::REFRESH_LIMIT];
int m_count[mcfg::SAMPLE_SIZE];

int roll_w, pitch_w, yaw_w;
double accel_data[3][mcfg::REFRESH_LIMIT]; // unit is g
int accl_count[mcfg::SAMPLE_SIZE];
double gyro_data[3][mcfg::REFRESH_LIMIT]; // unit is deg
int gyro_count[mcfg::SAMPLE_SIZE];

std::string freq_str;
std::string current_pose_str;
double current_pose[mcfg::REFRESH_LIMIT];
int pose_count = 0;


// Returns a double pointer which represents an array that is resulted
// from scootching the array given forward by 1 element
double* scootch_forward(double *arr, int len, double new_value) {
	double *first = arr;
	std::cout << "address of arr + 1" << (arr + 1) << std::endl;
	arr = arr + 1;
	arr[len - 1] = new_value;
	return arr;
}

class DataCollector : public myo::DeviceListener {
public:
	std::array <int8_t, 8> emgSamples; // THIS IS ABSOLUTELY REQUIRED. THE MYO's sample code does not have this!
	
	std::ofstream data_file;
	myo::Pose currentPose;
	bool onArm;
	time_t currentTime;
	struct tm localTime;
	
	bool constants_initialized;

	DataCollector()
		: onArm(false), currentPose(), emgSamples(), constants_initialized(false)
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
			current_pose_str = pose.toString();
		}
		else {
			myo->unlock(myo::Myo::unlockTimed);
		}
	}

	void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat) {
		using std::atan2;
		using std::asin;
		using std::sqrt;
		using std::max;
		using std::min;

		// Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
		float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
			1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
		float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
		float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
			1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));

		// Convert the floating point angles in radians to a scale from 0 to 18.
		roll_w = static_cast<int>((roll + (float)M_PI) / (M_PI * 2.0f) * 18);
		pitch_w = static_cast<int>((pitch + (float)M_PI / 2.0f) / M_PI * 18);
		yaw_w = static_cast<int>((yaw + (float)M_PI) / (M_PI * 2.0f) * 18);
	}

	void onAccelerometerData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& accel) {

		for (int i = 0; i < 3; i++) {
			if (accl_count[i] >= mcfg::REFRESH_LIMIT) {
				accl_count[i] = 0;
			}
		}
		
		accel_data[0][accl_count[0]] = accel.x();
		accel_data[1][accl_count[1]] = accel.y();
		accel_data[2][accl_count[2]] = accel.z();

		for (int i = 0; i < 3; i++) {
			if (accl_count[i] >= mcfg::REFRESH_LIMIT) {
				accl_count[i]++;
			}
		}
	}

	void onGyroscopeData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& gyro) {

		for (int i = 0; i < 3; i++) {
			if (gyro_count[i] >= mcfg::REFRESH_LIMIT) {
				gyro_count[i] = 0;
			}
		}

		gyro_data[0][gyro_count[0]] = gyro.x();
		gyro_data[1][gyro_count[1]] = gyro.y();
		gyro_data[2][gyro_count[2]] = gyro.z();

		for (int i = 0; i < 3; i++) {
			if (gyro_count[i] >= mcfg::REFRESH_LIMIT) {
				gyro_count[i]++;
			}
		}

	}

	// onEmgData() is called whenever a paired Myo has provided new EMG data, and EMG streaming is enabled.
	void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
	{
		for (int i = 0; i < 8; i++) {
			emgSamples[i] = emg[i];
			emgSamples_arr[i] = (double) emg[i];

			if (m_count[i] >= mcfg::REFRESH_LIMIT) {
				m_count[i] = 0;
			}
			emgv[i][m_count[i]] = emg[i];
			m_count[i]++;

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
		int current_pose_id = match_pose_id(currentPose);
		current_pose[pose_count] = current_pose_id;
		pose_count = pose_count >= mcfg::REFRESH_LIMIT ? 0 : (pose_count + 1);

		if (onArm) {
			// Print out the lock state, the currently recognized pose, and which arm Myo is being worn on.

			// Pose::toString() provides the human-readable name of a pose. We can also output a Pose directly to an
			// output stream (e.g. std::cout << currentPose;). In this case we want to get the pose name's length so
			// that we can fill the rest of the field with spaces below, so we obtain it as a string using toString().
			std::string poseString = currentPose.toString();

			//std::cout << '[' << poseString << std::string(14 - poseString.size(), ' ') << ']';
			this->data_file << current_pose	 << ",";
		}
		else {
			// Print out a placeholder for the arm and pose when Myo doesn't currently know which arm it's on.
			//std::cout << '[' << std::string(8, ' ') << ']' << "[?]" << '[' << std::string(14, ' ') << ']';
			this->data_file << current_pose << ',';
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
int main(int argc, char** argv)
{

	bool graphics_exit = FALSE;
	int i;
	double t = 0;

	long long last_mili = 0;
	double diff;

	std::cout << "Main:>\t Starting graphics" << std::endl;
	
	printf("\nMain:>\t Starting graphics");
	if (_USEGRAPHICS)
		Graphics_init(argc, argv, 20, 10, 1700, 1100, &graphics_exit, "MyoEMG visualization"); //Set up graphics
	
	// We catch any exceptions that might occur below -- see the catch statement for more details.
	try {

		// First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
		// publishing your application. The Hub provides access to one or more Myos.
		myo::Hub hub("com.example.emg-data-sample");

		std::cout << "Attempting to find a Myo..." << std::endl;

		// Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
		// immediately.
		// waitForMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
		// if that fails, the function will return a null pointer.
		myo::Myo *myo = hub.waitForMyo(10000);

		// If waitForMyo() returned a null pointer, we failed to find a Myo, so exit with an error message.
		if (!myo) {
			throw std::runtime_error("Unable to find a Myo!");
		}

		// We've found a Myo.
		std::cout << "Connected to a Myo armband!" << std::endl << std::endl;

		DataCollector collector;
		// Next we enable EMG streaming on the found Myo.
		myo->setStreamEmg(myo::Myo::streamEmgEnabled);

		// Next we construct an instance of our DeviceListener, so that we can register it with the Hub.

		// Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
		// Hub::run() to send events to all registered device listeners.
		hub.addListener(&collector);
		
		t = 0;
		// Finally we enter our main loop.
		while (!graphics_exit && !_kbhit()) {

			if (last_mili == 0) {
				last_mili = collector.getMilliCount();
			}
			else {
				long long mili = collector.getMilliCount();
				diff = (double)mili - (double)last_mili;
				
				double freq = 1.0 / diff * 1000;
				std::ostringstream ss;
				ss << freq;
				freq_str = ss.str();

				last_mili = mili;
			}

			// In each iteration of our main loop, we run the Myo event loop for a set number of milliseconds.
			// In this case, we wish to update our display 20 times a second, so we run for 1000/20 milliseconds.
			hub.run(1000 / 500);
			// After processing events, we call the print() member function we defined above to print out the values we've
			// obtained from any events that have occurred.
			collector.print();


			// Do something
			Sleep(1);
		}
		printf("\nMain:>\t User functions exited");

		if (_USEGRAPHICS)
			Graphics_Close();

		printf("\nMain:>\t Press enter to exit");
	}
	// If a standard exception occurred, we print out its message and exit.
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();

		exit(0);
	}

	// Done.

	
}