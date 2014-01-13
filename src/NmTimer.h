///////////////
// NMTIMER.H //
///////////////

#ifndef NMTIMER_H
#define NMTIMER_H

typedef struct
{
	__int64 frequency;
	float resolution;
	unsigned long mm_timer_start;
	unsigned long mm_timer_elapsed;
	bool performance_timer;
	__int64 performance_timer_start;
	__int64 performance_timer_elapsed;
} S_TIMER;

class CNmTimer
{
public:

	CNmTimer();
	virtual ~CNmTimer();

	void    Init();
	float   TimerGetTime();

private:

	S_TIMER m_Timer;
};

#endif
