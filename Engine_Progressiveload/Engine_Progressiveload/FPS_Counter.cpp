#include "FPS_Counter.h"
#include <math.h>

CFPS_Counter::CFPS_Counter(void) : FPS(60), StartTime(0)
{
}


CFPS_Counter::~CFPS_Counter(void)
{
}


//! returns current fps
int CFPS_Counter::getFPS() const
{
	return FPS;
}


//! to be called every frame
void CFPS_Counter::registerFrame(unsigned int now)
{
	++FramesCounted;

	const unsigned int milliseconds = now - StartTime;

	if (milliseconds >= 1500 )
	{
		const float invMilli = (float)(1.0f/ milliseconds);

		FPS = ceil ( ( 1000 * FramesCounted ) * invMilli );
		FramesCounted = 0;
		StartTime = now;
	}
}
