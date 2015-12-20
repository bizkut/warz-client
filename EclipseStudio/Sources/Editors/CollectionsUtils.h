//=========================================================================
//	Module: CollectionsUtils.h
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

const int INVALID_COLLECTION_INDEX = -1;
const int COLLECTION_ELEMENT_MAX_LOD_COUNT = 3;

//////////////////////////////////////////////////////////////////////////

namespace Collections
{
	/**	Special array with stable indices regardless of add/revove operations. */
	template <typename T>
	struct UtilArray
	{
		/**	Actual values. */
		typedef r3dTL::TArray<T> Values;
		Values values;

		/**	Unused (free) element indices. */
		typedef r3dTL::TArray<int> FreeIndices;
		FreeIndices freeIndices;

		/**	Get free element index. */
		int GetFreeIndex()
		{
			if (freeIndices.Count() > 0)
			{
				int idx = freeIndices.GetLast();
				freeIndices.PopBack();
				return idx;
			}

			//	Add new element into elements array
			int idx = static_cast<int>(values.Count());
			values.Resize(values.Count() + 1);
			return idx;
		}

		/**	Set element free. */
		void SetFree(int idx)
		{
			r3d_assert(values.Count() > static_cast<uint32_t>(idx));

			//	Prevent multiple deletion
			bool add = true;
			if (freeIndices.Count() > 0)
			{
				int *start = &freeIndices.GetFirst();
				int *end = start + freeIndices.Count();
				int *found = std::find(start, end, idx);

				add = found == end;
			}

			if (add)
			{
				freeIndices.PushBack(idx);
			}
		}

		/**	Accessor. */
		T & operator[](int idx)
		{
			r3d_assert(idx >= 0);
			return values[idx];
		}

		/**	Const accessor. */
		const T & operator[](int idx) const
		{
			r3d_assert(idx >= 0);
			return values[idx];
		}

		uint32_t Count() const
		{
			return values.Count();
		}

		void Clear()
		{
			values.Clear();
			freeIndices.Clear();
		}

		/**	Is element with given index is marked as free. */
		bool IsFree(int idx) const
		{
			for (uint32_t i = 0; i < freeIndices.Count(); ++i)
			{
				if (idx == freeIndices[i])
				{
					return true;
				}
			}
			return false;
		}
	};
}
