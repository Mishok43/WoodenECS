#pragma once

#include "stdafx.h"
#include "Handlers.h"
#include "WoodenAllocators/AllocatorPoolFreeList.h"
#include <vector>

WECS_BEGIN

class MHandleManager: public wal::AllocatorPoolFreeList
{
public:
	MHandleManager()
	{
		init(sizeof(HEntity), 2048, alignof(HEntity));
	}

	inline HEntity allocate()
	{
		HEntity* hEntity = (HEntity*)(allocMem(1)); 
		hEntity->h = getPos(hEntity);
		return *hEntity;
	}

	inline void deallocate(HEntity handle)
	{
		freeMem(get<HEntity>(handle.h));
	}

	inline void reset()
	{
		AllocatorPoolFreeList::reset();
	}

protected:

};


WECS_END
