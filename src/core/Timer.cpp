// Timer object.
// Calculate delta/frame time and FPS.
#include "stdafx.hpp"
#include "Timer.h"

// Initialise timer. Check for high performance timers.
core::Timer::Timer()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	if (frequency == 0)
	{
		MessageBox(nullptr, L"No support for high performance timer", L"ERROR", MB_OK);
	}

	// Find out how many times the frequency counter ticks every second.
	ticksPerS = (double)(frequency);

	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	elapsedTime = 0.f;
	frames = 0.f;
	fps = 0.f;
}

core::Timer::~Timer()
{
}

// Once per frame calculate delta timer and update FPS calculation.
void core::Timer::frame()
{
	INT64 currentTime = 0;
	double timeDifference;

	// Query the current time.
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	timeDifference = (double)(currentTime - startTime);
	frameTime = timeDifference / ticksPerS;

	// Calc FPS
	frames += 1.f;
	elapsedTime += frameTime;
	if (elapsedTime > 1.0f)
	{
		fps = frames / elapsedTime;
		frames = 0.0f;
		elapsedTime = 0.0f;
	}

	// Restart the timer.
	startTime = currentTime;

	return;
}

double core::Timer::getTime()
{
	return frameTime;
}

double core::Timer::getFPS()
{
	return fps;
}