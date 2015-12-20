/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY

#ifndef Navigation_ChannelArrayComputerStats_H
#define Navigation_ChannelArrayComputerStats_H

#include "gwnavruntime/base/memory.h"


namespace Kaim
{

class ChannelArrayComputerStats
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	ChannelArrayComputerStats():
		m_diagonalstripComputer(0.0f),
		m_diagonalstripComputer_ComputeDiagonals(0.0f),
		m_diagonalstripComputer_ComputeDiagonals_CanGo(0.0f),
		m_diagonalstripComputer_ComputeMaxRadius(0.0f),
		m_diagonalstripComputer_ComputeNeighbor(0.0f),
		m_stringPuller(0.0f),
		m_gateArrayComputer(0.0f),
		m_createAggregatePath(0.0f),
		m_visualDebug(0.0f),
		m_totalTime(0.0f){}

public: 
	KyFloat32 GetPercentage(KyFloat32 x) const { return (x/m_totalTime * 100.0f); }
	void PrintStats() const
	{
		KY_LOG_MESSAGE((" --- ChannelArrayComputer Stats ---"));
		KY_LOG_MESSAGE((" diagonalstripComputer  : %.2f ms (%.1f %%)"  , m_diagonalstripComputer                 , GetPercentage(m_diagonalstripComputer)));
		KY_LOG_MESSAGE((" \t - ComputeDiagonals    : %.2f ms (%.1f %%)", m_diagonalstripComputer_ComputeDiagonals, GetPercentage(m_diagonalstripComputer_ComputeDiagonals)));
		KY_LOG_MESSAGE((" \t - ComputeMaxRadius    : %.2f ms (%.1f %%)", m_diagonalstripComputer_ComputeMaxRadius, GetPercentage(m_diagonalstripComputer_ComputeMaxRadius)));
		KY_LOG_MESSAGE((" \t - ComputeNeighbor     : %.2f ms (%.1f %%)", m_diagonalstripComputer_ComputeNeighbor , GetPercentage(m_diagonalstripComputer_ComputeNeighbor)));
		KY_LOG_MESSAGE((" stringPuller           : %.2f ms (%.1f %%)"  , m_stringPuller                          , GetPercentage(m_stringPuller)));
		KY_LOG_MESSAGE((" gateArrayComputer      : %.2f ms (%.1f %%)"  , m_gateArrayComputer                     , GetPercentage(m_gateArrayComputer)));
		KY_LOG_MESSAGE((" createAggregatePath    : %.2f ms (%.1f %%)"  , m_createAggregatePath                   , GetPercentage(m_createAggregatePath)));
		KY_LOG_MESSAGE((" visualDebug            : %.2f ms (%.1f %%)"  , m_visualDebug                           , GetPercentage(m_visualDebug)));

		PrintTotalTime();
	}

	void PrintTotalTime() const
	{
		KY_LOG_MESSAGE((" total                    : %.2f ms", m_totalTime));
		KY_LOG_MESSAGE((" total without visualDeug : %.2f ms", m_totalTime - m_visualDebug));
	}

public: 
	KyFloat32 m_diagonalstripComputer;
	KyFloat32 m_diagonalstripComputer_ComputeDiagonals;
	KyFloat32 m_diagonalstripComputer_ComputeDiagonals_CanGo;
	KyFloat32 m_diagonalstripComputer_ComputeMaxRadius;
	KyFloat32 m_diagonalstripComputer_ComputeNeighbor;
	KyFloat32 m_stringPuller;
	KyFloat32 m_gateArrayComputer;
	KyFloat32 m_createAggregatePath;
	KyFloat32 m_visualDebug;

	KyFloat32 m_totalTime;
};


} // namespace Kaim

#endif // Navigation_ChannelArrayComputerStats_H
