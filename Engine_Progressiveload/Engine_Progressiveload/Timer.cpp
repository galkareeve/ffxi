#include "Timer.h"
#include <Windows.h>

CTimer::CTimer(void)
{
	VirtualTimerSpeed = 1.0f;
	VirtualTimerStopCounter = 0;
	LastVirtualTime = 0;
	StartRealTime = 0;
	StaticTime = 0;
}


CTimer::~CTimer(void)
{
}

//! returns current virtual time
unsigned int CTimer::getTime()
{
	if (isStopped())
		return LastVirtualTime;

	return LastVirtualTime + (unsigned int)((StaticTime - StartRealTime) * VirtualTimerSpeed);
}

//! ticks, advances the virtual timer
void CTimer::tick()
{
	StaticTime = GetTickCount();
}

//! sets the current virtual time
void CTimer::setTime(unsigned int time)
{
	StaticTime = GetTickCount();
	LastVirtualTime = time;
	StartRealTime = StaticTime;
}

//! stops the virtual timer
void CTimer::stopTimer()
{
	if (!isStopped())
	{
		// stop the virtual timer
		LastVirtualTime = getTime();
	}
	--VirtualTimerStopCounter;
}

//! starts the virtual timer
void CTimer::startTimer()
{
	++VirtualTimerStopCounter;
	if (!isStopped())
	{
		// restart virtual timer
		setTime(LastVirtualTime);
	}
}

//! sets the speed of the virtual timer
void CTimer::setSpeed(float speed)
{
	setTime(getTime());

	VirtualTimerSpeed = speed;
	if (VirtualTimerSpeed < 0.0f)
		VirtualTimerSpeed = 0.0f;
}

//! gets the speed of the virtual timer
float CTimer::getSpeed()
{
	return VirtualTimerSpeed;
}

//! returns if the timer currently is stopped
bool CTimer::isStopped()
{
	return VirtualTimerStopCounter < 0;
}

void CTimer::initVirtualTimer()
{
	StaticTime = GetTickCount();
	StartRealTime = StaticTime;
}
