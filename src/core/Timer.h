/**
* \class Timer
*
* \brief Calculates frame/delta time and FPS
*
* \author Paul Robertson
*/

// timer class
// Calculates frame/delta time and FPS.

#ifndef _TIMER_H_
#define _TIMER_H_

#include <windows.h>
namespace core
{
	class Timer
	{
	public:
		Timer();
		~Timer();

		void frame();	///< Update delta time, called every frame

		float getTime();	///< Get delta time
		float getFPS();		///< Get FPS (for display)

	private:
		INT64 frequency = 0;
		float ticksPerS = 0.f;
		INT64 startTime = 0;
		float frameTime = 0.f;
		float fps = 0.f;
		float frames = 0.f;
		float elapsedTime = 0.f;
	};
}

#endif