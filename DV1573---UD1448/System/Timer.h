#ifndef _TIMER_h
#define _TIMER_h
#include <chrono>
#include <iostream>
class Timer {
private:

	std::chrono::steady_clock::time_point startPoint;
	std::chrono::steady_clock::time_point endPoint;
	

public:
	Timer();
	
	void start();
	void stop();
	void print();

};


#endif