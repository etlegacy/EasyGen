/////////////////
// NMTIMER.CPP //
/////////////////
#include "NmTimer.h"

////////

#include <windows.h>
#include <mmsystem.h>

#pragma comment (lib,"winmm.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNmTimer::CNmTimer()
{

}

CNmTimer::~CNmTimer()
{

}

void CNmTimer::Init()
{
	memset(&m_Timer, 0, sizeof(m_Timer));

	// Check To See If A Performance Counter Is Available
	// If One Is Available The Timer Frequency Will Be Updated
	if (!QueryPerformanceFrequency((LARGE_INTEGER *) &m_Timer.frequency))
	{
		// No Performace Counter Available
		m_Timer.performance_timer = false;
		m_Timer.mm_timer_start    = timeGetTime();
		m_Timer.resolution        = 1.0f / 1000.0f;
		m_Timer.frequency         = 1000;
		m_Timer.mm_timer_elapsed  = m_Timer.mm_timer_start;
	}
	else
	{
		// Performance Counter Is Available, Use It Instead Of The Multimedia Timer
		// Get The Current Time And Store It In performance_timer_start
		QueryPerformanceCounter((LARGE_INTEGER *) &m_Timer.performance_timer_start);
		m_Timer.performance_timer = true;
		// Calculate The Timer Resolution Using The Timer Frequency
		m_Timer.resolution = (float) (((double)1.0f) / ((double)m_Timer.frequency));
		// Set The Elapsed Time To The Current Time
		m_Timer.performance_timer_elapsed = m_Timer.performance_timer_start;
	}
}

float CNmTimer::TimerGetTime()
{
	__int64 time;

	if (m_Timer.performance_timer)
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &time);
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return ((float) (time - m_Timer.performance_timer_start) * m_Timer.resolution) * 1000.0f;
	}
	else
	{
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return ((float) (timeGetTime() - m_Timer.mm_timer_start) * m_Timer.resolution) * 1000.0f;
	}
}