#include "Timer.h"

Timer::Timer()
{
	m_curStartTime = std::chrono::high_resolution_clock::now();
	m_curStoppedTime = std::chrono::high_resolution_clock::now();
}
bool Timer::Start()
{
	if (m_isRunning == true)
		return false;

	m_curStartTime = std::chrono::high_resolution_clock::now();
	m_isRunning = true;
	return true;
}
bool Timer::Stop()
{
	if (m_isRunning == false)
		return false;

	m_curStoppedTime = std::chrono::high_resolution_clock::now();
	m_isRunning = false;
	return true;
}
void Timer::Restart()
{
	m_isRunning = true;
	m_curStartTime = std::chrono::high_resolution_clock::now();
}
double Timer::GetElapsedMiliseconds()
{
	if (m_isRunning == true)
	{
		auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - m_curStartTime);
		return elapsed.count();
	}
	else
	{
		auto elapsed = std::chrono::duration<double, std::milli>(m_curStoppedTime - m_curStartTime);
		return elapsed.count();
	}
}