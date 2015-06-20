#pragma once
class CTimer
{
public:
	CTimer(void);
	~CTimer(void);

	void setTime(unsigned int time);
	unsigned int getTime();
	void tick();
	void stopTimer();
	void startTimer();
	void setSpeed(float speed);
	float getSpeed();
	bool isStopped();
	void initVirtualTimer();

protected:
	float VirtualTimerSpeed;
	int VirtualTimerStopCounter;
	unsigned int LastVirtualTime;
	unsigned int StartRealTime;
	unsigned int StaticTime;
};

