#pragma once

#include "stdafx.h"
#include "Handlers.h"
#include <vector>

WECS_BEGIN

class MHandleManager
{
public:
	MHandleManager()
	{
		reset();
	}

	inline HEntity allocate()
	{
		if (!freeHandles.empty())
		{
			HEntity handle = freeHandles[0];
			size_t numFreeHandles = freeHandles.size();
			freeHandles[0] = freeHandles[numFreeHandles-1];
			freeHandles.resize(numFreeHandles - 1);

			return handle;
		}
		return numAllocatedHandles++;
	}

	inline void deallocate(HEntity handle)
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
	std::vector<HEntity> freeHandles;
};


WECS_END
