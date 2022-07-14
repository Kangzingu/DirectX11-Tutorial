#include "Timer.h"

Timer::Timer()
{
	curStartTime = std::chrono::high_resolution_clock::now();
	curStoppedTime = std::chrono::high_resolution_clock::now();
}

double Timer::GetElapsedMiliseconds()
{
	if (isrunning)
	{
		auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - curStartTime);
		return elapsed.count();
	}
	else
	{
		auto elapsed = std::chrono::duration<double, std::milli>(curStoppedTime - curStartTime);
		return elapsed.count();
	}
}

void Timer::Restart()
{
	isrunning = true;
	curStartTime = std::chrono::high_resolution_clock::now();
}

bool Timer::Stop()
{
	if (!isrunning)
	{
		return false;
	}
	else
	{
		curStoppedTime = std::chrono::high_resolution_clock::now();
		isrunning = false;
		return true;
	}
}

bool Timer::Start()
{
	if (isrunning)
	{
		return false;
	}
	else
	{
		curStartTime = std::chrono::high_resolution_clock::now();
		isrunning = true;
		return true;
	}
}