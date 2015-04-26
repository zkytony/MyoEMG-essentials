//#include "Graphics.h"
#include "matplotpp.h"
#include "config.h"
#include <vector>

extern double emgSamples_arr[mcfg::SAMPLE_SIZE];
extern double emgv[mcfg::SAMPLE_SIZE][mcfg::REFRESH_LIMIT];
extern std::string freq_str;
extern int roll_w, pitch_w, yaw_w;
extern double accel_data[3][mcfg::REFRESH_LIMIT]; // unit is g
extern double gyro_data[3][mcfg::REFRESH_LIMIT]; // unit is deg
extern std::string current_pose_str;
extern double current_pose[mcfg::REFRESH_LIMIT];

void Plot::DISPLAY()
{	
	dvec emg_vec(emgSamples_arr, emgSamples_arr + mcfg::SAMPLE_SIZE);
	dvec c_pose(current_pose, current_pose + mcfg::REFRESH_LIMIT);

	layer("myo");
	title("Myo EMG");

	int hy;
	hy = subplot(2, 5, 1);
	axis(0, mcfg::SAMPLE_SIZE + 1, -100, 100);
	title("Current Samples (" + freq_str + " Hz)");
	ylabel("EMG");
	xlabel("time");
	bar(emg_vec); 
	set("EdgeColor", "k");
	set("FaceColor", "g");

	hy = subplot(2, 5, 2);
	axis(0, mcfg::REFRESH_LIMIT, -100, 100);
	title("#1");
	ylabel("EMG");
	plot(emgv[1], mcfg::REFRESH_LIMIT);

	hy = subplot(2, 5, 3);
	axis(0, mcfg::REFRESH_LIMIT, -100, 100);
	title("#1");
	ylabel("EMG");
	plot(emgv[1], mcfg::REFRESH_LIMIT);

	hy = subplot(2, 5, 4);
	axis(0, mcfg::REFRESH_LIMIT, -100, 100);
	title("#2");
	ylabel("EMG");
	plot(emgv[2], mcfg::REFRESH_LIMIT);;

	hy = subplot(2, 5, 5);
	axis(0, mcfg::REFRESH_LIMIT, -100, 100);
	title("#3");
	ylabel("EMG");
	plot(emgv[3], mcfg::REFRESH_LIMIT);

	hy = subplot(2, 5, 6);
	axis(0, mcfg::REFRESH_LIMIT, -100, 100);
	title("#4");
	ylabel("E4MG");
	plot(emgv[4], mcfg::REFRESH_LIMIT);

	hy = subplot(2, 5, 7);
	axis(0, mcfg::REFRESH_LIMIT, -100, 100);
	title("#5");
	ylabel("EMG");
	plot(emgv[5], mcfg::REFRESH_LIMIT);

	hy = subplot(2, 5, 8);
	axis(0, mcfg::REFRESH_LIMIT, -100, 100);
	title("#6");
	ylabel("EMG");
	plot(emgv[6], mcfg::REFRESH_LIMIT);

	hy = subplot(2, 5, 9);
	axis(0, mcfg::REFRESH_LIMIT, -100, 100);
	title("#7");
	ylabel("EMG");
	plot(emgv[7], mcfg::REFRESH_LIMIT);

	hy = subplot(2, 5, 10);
	axis(0, mcfg::REFRESH_LIMIT, -2, 7);
	title(current_pose_str);
	ylabel("Pose ID");
	xlabel("time");
	bar(c_pose);

	layer(" Acceleration data & Gyroscope data & Orientation Data", 0);
	//double x[mcfg::REFRESH_LIMIT];
	//for (int i = 0; i < mcfg::REFRESH_LIMIT; i++) {
	//	x[i] = 10.0*i / (mcfg::REFRESH_LIMIT - 1);
	//}

	hy = subplot(2, 2, 1);
	title("Gyroscop data");
	axis(0, 5, -150, 150);
	ylabel("deg");
	plot(gyro_data[0], mcfg::REFRESH_LIMIT);
	set("r"); set(2);
	plot(gyro_data[1], mcfg::REFRESH_LIMIT);
	set("g"); set(2);
	plot(gyro_data[2], mcfg::REFRESH_LIMIT);
	set("b"); set(2);
	text(0, 1.5, "Red: x; Green: y; Blue: z", false);

	hy = subplot(2, 2, 2);
	title("Accelerator data");
	ylabel("g");
	axis(0, 5, -5, 5);
	plot(accel_data[0], mcfg::REFRESH_LIMIT);
	set("r"); set(2);
	plot(accel_data[1], mcfg::REFRESH_LIMIT);
	set("g"); set(2);
	plot(accel_data[2], mcfg::REFRESH_LIMIT);
	set("b"); set(2);
	text(0, 1.5, "Red: x; Green: y; Blue: z", false);

	hy = subplot(2, 2, 3);
	title("Orientation");

	// plot3

}