/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

namespace Kaim
{

template <class BitFieldClass>
void CellFilter::AddCellFilter(const CellBox& cellBox, const BitFieldClass& bitField)
{
	ExpandCellBox(cellBox);
	Box2iIteratorInBiggerRowMajorBox boxIterator(cellBox, m_cellBox);
	while (boxIterator.IsFinished() == false)
	{
		bool isBitSet = bitField.IsBitSet(boxIterator.GetInnerBoxRowMajorIndex());
		m_cellBoxCoverage->SetBitValue(boxIterator.GetOuterBoxRowMajorIndex(), isBitSet);
		++boxIterator;
	}
}

template <class BitFieldClass>
void CellFilter::Display(ScopedDisplayList& displayList, const DatabaseGenMetrics& genMetrics, const CellBox& cellBox, const BitFieldClass& cellBoxCoverage)
{
	CoordPos64 minCoordPos = genMetrics.ComputeCellOrigin(cellBox.m_min);
	CoordPos64 maxCoordPos = genMetrics.ComputeCellOrigin(cellBox.m_max + CellPos(1,1));
	Vec3f boxMin(genMetrics.GetFloatValueFromInteger64(minCoordPos.x), genMetrics.GetFloatValueFromInteger64(minCoordPos.y), -100.f);
	Vec3f boxMax(genMetrics.GetFloatValueFromInteger64(maxCoordPos.x), genMetrics.GetFloatValueFromInteger64(maxCoordPos.y), 100.f);
	Box3f box(boxMin, boxMax);
	VisualShapeColor shapeColor;
	shapeColor.SetOnlyLineColor(VisualColor::Purple);
	displayList.PushBox(box, shapeColor);
	Box2iRowMajorIterator it(cellBox);
	while (it.IsFinished() == false)
	{
		KyUInt32 index = it.GetRowMajorIndex();
		if (cellBoxCoverage.IsBitSet(index) == false)
		{
			const CellPos& cellPos = it.GetPos();
			minCoordPos = genMetrics.ComputeCellOrigin(cellPos);
			maxCoordPos = genMetrics.ComputeCellOrigin(cellPos + CellPos(1,1));
			boxMin.Set(genMetrics.GetFloatValueFromInteger64(minCoordPos.x), genMetrics.GetFloatValueFromInteger64(minCoordPos.y), 0.f);
			boxMax.Set(genMetrics.GetFloatValueFromInteger64(maxCoordPos.x), genMetrics.GetFloatValueFromInteger64(maxCoordPos.y), 0.f);
			displayList.PushQuad(boxMin, Vec3f(boxMin.x, boxMax.y, 0.f), boxMax, Vec3f(boxMax.x, boxMin.y, 0.f), shapeColor);
		}
		++it;
	}
}


}
