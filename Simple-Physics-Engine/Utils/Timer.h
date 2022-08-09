#pragma once
#include <chrono>

class Timer
{
private:
	bool m_isRunning = false;

public:
	Timer();
	bool Start();
	bool Stop();
	void Restart();
	double GetElapsedMiliseconds();

	#ifdef _WIN32
	std::chrono::time_point<std::chrono::steady_clock> m_curStartTime;
	std::chrono::time_point<std::chrono::steady_clock> m_curStoppedTime;
	#else
	std::chrono::time_point<std::chrono::system_clock> m_curStartTime;
	std::chrono::time_point<std::chrono::system_clock> m_curStoppedTime;
	#endif
};