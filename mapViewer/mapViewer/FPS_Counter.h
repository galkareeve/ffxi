#pragma once
class CFPS_Counter
{
public:
	CFPS_Counter(void);
	~CFPS_Counter(void);

	//! returns current fps
	int getFPS() const;

	//! to be called every frame
	void registerFrame(unsigned int now);

private:

	int FPS;
	unsigned int StartTime;
	unsigned int FramesCounted;
};

