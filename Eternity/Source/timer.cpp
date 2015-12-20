#include "r3dPCH.h"
#include "timer.h"
/***************************************************************************
 * Timer::construtor -- sets timeer                                        *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 * INPUT:                                                                  *
 *                                                                         *
 * OUTPUT:                                                                 *
 *                                                                         *
 * WARNINGS:                                                               *
 *                                                                         *
 * HISTORY:                                                                *
 *   05/25/1996  BP : Created.                                             *
 *=========================================================================*/
Timer::Timer()
{
	Count = GetTickCount();
}


/***************************************************************************
 * Timer::reset -- reset the timer                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 * INPUT:                                                                  *
 *                                                                         *
 * OUTPUT:                                                                 *
 *                                                                         *
 * WARNINGS:                                                               *
 *                                                                         *
 * HISTORY:                                                                *
 *   05/25/1996  BP : Created.                                             *
 *=========================================================================*/
void Timer::Reset()
{
	Count = GetTickCount();
}


/***************************************************************************
 * Timer::Time -- get the time                                             *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 * INPUT:                                                                  *
 *                                                                         *
 * OUTPUT:                                                                 *
 *                                                                         *
 * WARNINGS:                                                               *
 *                                                                         *
 * HISTORY:                                                                *
 *   05/25/1996  BP : Created.                                             *
 *=========================================================================*/
int Timer::Time() const
{
	int ticks = GetTickCount() - Count;
	return ticks;
}



int Timer::Seconds() const
{
	int ticks = GetTickCount() - Count;
	return ticks / 1000;
}
