#pragma once
#include <chrono>

class Timer
{
public:
	Timer();
	double GetElapsedMiliseconds();
	void Restart();
	bool Stop();
	bool Start();

private:
	bool isrunning = false;
#ifdef _WIN32
	std::chrono::time_point<std::chrono::steady_clock> curStartTime;
	std::chrono::time_point<std::chrono::steady_clock> curStoppedTime;
#else
	std::chrono::time_point<std::chrono::system_clock> curStartTime;
	std::chrono::time_point<std::chrono::system_clock> curStoppedTime;
#endif
};