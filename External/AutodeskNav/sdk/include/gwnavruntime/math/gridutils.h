/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_SquarePartitionUtils_H
#define GwNavGen_SquarePartitionUtils_H

#include "gwnavruntime/math/box2i.h"


namespace Kaim
{


class GridUtils // class with static functions only
{
public:
	// ----------------------------- SQUARE FROM COORD -----------------------------
	//
	//            ----|---------------|---------------|---------------|---------------|----
	// Coord        -200            -100              0              100             200
	// LowSquare      ]      -2       ]      -1       ]       0       ]       1       ]
	// HighSquare     [      -2       [      -1       [       0       [       1       [

	static KyInt32 LowSquare(KyInt32 squareSize, KyInt32 coord)
	{
		KyInt32 is_strict_pos = -(-coord >> 31); // = coord > 0 ? 1 : 0;
		return (coord - is_strict_pos) / squareSize - 1 + is_strict_pos;
	}

	static KyInt32 HighSquare(KyInt32 squareSize, KyInt32 coord)
	{
		KyInt32 is_strict_neg = -(coord >> 31); // = coord < 0 ? 1 : 0;
		return (coord + is_strict_neg) / squareSize - is_strict_neg;
	}

	static KyInt64 HighSquare64(KyInt64 squareSize, KyInt64 coord)
	{
		KyInt64 is_strict_neg = -(coord >> 63); // = coord < 0 ? 1 : 0;
		return (coord + is_strict_neg) / squareSize - is_strict_neg;
	}

	static void LowSquare(KyInt32 squareSize, const Vec2i& pos, Vec2i& squarePos)
	{
		squarePos.x = LowSquare(squareSize, pos.x);
		squarePos.y = LowSquare(squareSize, pos.y);
	}

	static void HighSquare(KyInt32 squareSize, const Vec2i& pos, Vec2i& squarePos)
	{
		squarePos.x = HighSquare(squareSize, pos.x);
		squarePos.y = HighSquare(squareSize, pos.y);
	}

	static void LowSquare(KyInt32 squareSize, const Vec3i& pos, Vec2i& squarePos)
	{
		squarePos.x = LowSquare(squareSize, pos.x);
		squarePos.y = LowSquare(squareSize, pos.y);
	}

	static void HighSquare(KyInt32 squareSize, const Vec3i& pos, Vec2i& squarePos)
	{
		squarePos.x = HighSquare(squareSize, pos.x);
		squarePos.y = HighSquare(squareSize, pos.y);
	}

	// Get the squareBox that overlaps the coordBox (INCLUDING the coordBox BOUNDARY)
	static void SquareBox(KyInt32 squareSize, const Box2i& coordBox, Box2i& squareBox)
	{
		LowSquare(squareSize, coordBox.m_min, squareBox.m_min);
		HighSquare(squareSize, coordBox.m_max, squareBox.m_max);
		squareBox.UpdateCountXY();
	}

	// Get the squareBox that overlaps the coordBox (EXCLUDING the coordBox BOUNDARY)
	static void SquareBoxStrict(KyInt32 squareSize, const Box2i& coordBox, Box2i& squareBox)
	{
		HighSquare(squareSize, coordBox.m_min, squareBox.m_min);
		LowSquare(squareSize, coordBox.m_max, squareBox.m_max);
		squareBox.UpdateCountXY();
	}

	// ----------------------------- COORD FROM SQUARE -----------------------------
	//
	static KyInt32 MinCoord(KyInt32 squareSize, KyInt32 squareCoord)
	{
		return squareCoord * squareSize;
	}

	static KyInt32 MaxCoord(KyInt32 squareSize, KyInt32 squareCoord)
	{
		return (squareCoord + 1) * squareSize;
	}

	static void MinCoordPos(KyInt32 squareSize, const Vec2i& squarePos, Vec2i& coordPos)
	{
		coordPos.x = MinCoord(squareSize, squarePos.x);
		coordPos.y = MinCoord(squareSize, squarePos.y);
	}

	static void MaxCoordPos(KyInt32 squareSize, const Vec2i& squarePos, Vec2i& coordPos)
	{
		coordPos.x = MaxCoord(squareSize, squarePos.x);
		coordPos.y = MaxCoord(squareSize, squarePos.y);
	}

	static void GetCoordBox(KyInt32 squareSize, const Box2i& squareBox, Box2i& coordBox)
	{
		MinCoordPos(squareSize, squareBox.m_min, coordBox.m_min);
		MaxCoordPos(squareSize, squareBox.m_max, coordBox.m_max);
		coordBox.UpdateCountXY();
	}

	// ----------------------------- SMALL SQUARE FROM BIG SQUARE -----------------------------
	//
	// SmallSquare    | -6  | -5  | -4  | -3  | -2  | -1  |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |
	// BigSquare      |       -2        |       -1        |        0        |        1        |        2        |
	//
	static KyInt32 MinSmallSquareCoord(KyInt32 nbSmallsOnBigSide, KyInt32 bigCoord)
	{
		return bigCoord * nbSmallsOnBigSide;
	}

	static KyInt32 MaxSmallSquareCoord(KyInt32 nbSmallsOnBigSide, KyInt32 bigCoord)
	{
		return (bigCoord + 1) * nbSmallsOnBigSide - 1;
	}

	static void MinSmallSquarePos(KyInt32 nbSmallsOnBigSide, const Vec2i& bigPos, Vec2i& smallPos)
	{
		smallPos.x = MinSmallSquareCoord(nbSmallsOnBigSide, bigPos.x);
		smallPos.y = MinSmallSquareCoord(nbSmallsOnBigSide, bigPos.y);
	}

	static void MaxSmallSquarePos(KyInt32 nbSmallsOnBigSide, const Vec2i& bigPos, Vec2i& smallPos)
	{
		smallPos.x = MaxSmallSquareCoord(nbSmallsOnBigSide, bigPos.x);
		smallPos.y = MaxSmallSquareCoord(nbSmallsOnBigSide, bigPos.y);
	}

	static void SmallSquareBox(KyInt32 nbSmallsOnBigSide, const Box2i& bigBox, Box2i& smallBox)
	{
		MinSmallSquarePos(nbSmallsOnBigSide, bigBox.m_min, smallBox.m_min);
		MaxSmallSquarePos(nbSmallsOnBigSide, bigBox.m_max, smallBox.m_max);
		smallBox.UpdateCountXY();
	}

	static void SmallSquareBox(KyInt32 nbSmallsOnBigSide, const Vec2i& bigPos, Box2i& smallBox)
	{
		MinSmallSquarePos(nbSmallsOnBigSide, bigPos, smallBox.m_min);
		MaxSmallSquarePos(nbSmallsOnBigSide, bigPos, smallBox.m_max);
		smallBox.UpdateCountXY();
	}

	// ----------------------------- BIG SQUARE FROM SMALL SQUARE -----------------------------
	//
	// SmallSquare    | -6  | -5  | -4  | -3  | -2  | -1  |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |
	// BigSquare      |       -2        |       -1        |        0        |        1        |        2        |
	//
	static KyInt32 BigSquareCoord(KyInt32 nbSmallsOnBigSide, KyInt32 smallCoord)
	{
		KyInt32 is_strict_neg = -(smallCoord >> 31); // = smallCoord < 0 ? 1 : 0;
		return (smallCoord + is_strict_neg) / nbSmallsOnBigSide - is_strict_neg;
		// ((-1 + 1) / 3 - 1)  =  ( 0 / 3 - 1)  =  -1
		// ((-2 + 1) / 3 - 1)  =  (-1 / 3 - 1)  =  -1
		// ((-3 + 1) / 3 - 1)  =  (-2 / 3 - 1)  =  -1
		// ((-4 + 1) / 3 - 1)  =  (-3 / 3 - 1)  =  -2
	}

	static void BigSquarePos(KyInt32 nbSmallsOnBigSide, const Vec2i& smallPos, Vec2i& bigPos)
	{
		bigPos.x = BigSquareCoord(nbSmallsOnBigSide, smallPos.x);
		bigPos.y = BigSquareCoord(nbSmallsOnBigSide, smallPos.y);
	}

	static void BigSquareBox(KyInt32 nbSmallsOnBigSide, const Box2i& smallBox, Box2i& bigBox)
	{
		BigSquarePos(nbSmallsOnBigSide, smallBox.m_min, bigBox.m_min);
		BigSquarePos(nbSmallsOnBigSide, smallBox.m_max, bigBox.m_max);
		bigBox.UpdateCountXY();
	}
};


}


#endif

