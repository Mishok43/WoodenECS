#pragma once

#include "stdafx.h"
#include <vector>

WECS_BEGIN

class MHandleManager
{
public:
	MHandleManager()
	{
		reset();
	}

	inline size_t allocate()
	{
		if (!freeHandles.empty())
		{
			size_t handle = freeHandles[0];
			size_t numFreeHandles = freeHandles.size();
			freeHandles[0] = freeHandles[numFreeHandles-1];
			freeHandles.resize(numFreeHandles - 1);

			return handle;
		}
		return numAllocatedHandles++;
	}

	inline void deallocate(size_t handle)
	{
#ifdef _DEBUG
		assert(handle < numAllocatedHandles);
#endif

		freeHandles.push_back(handle);
	}

	inline void reset()
	{
		numAllocatedHandles = 0;
		freeHandles.clear();
	}

protected:
	size_t numAllocatedHandles = 0;
	std::vector<size_t> freeHandles;
};


WECS_END
