#pragma once
#include "stdafx.h"
#include "DComponent.h"
#include "DIndexTable.h"

WECS_BEGIN

using FDestroy = std::function<void(void*)>;
using FCreate = std::function<void(size_t, void*)>;
class WComponents
{
public:
	WComponents()
	{}

	WComponents(const WComponents& wc) = delete;
	WComponents operator =(const WComponents& wc) = delete;
	WComponents& operator =(WComponents wc) = delete;

	WComponents(WComponents&& wc) :
		storage(wc.storage),
		indices(wc.indices),
		createFuncs(std::move(wc.createFuncs)),
		destroyFuncs(std::move(wc.destroyFuncs))
	{}

	WComponents operator =(WComponents&& wc)
	{
		WComponents wcomp(std::move(wc));
		return wcomp;
	}

	~WComponents()
	{
		delete storage;
		delete indices;
	}

	void* getRaw(size_t hEntity)
	{
		size_t hComp = indices->get(hEntity);
		if (hComp != INVALID_HANDLE)
		{
			return storage->getRaw(hComp);
		}
		else
		{
			return nullptr;
		}
	}

	const void* getRaw(size_t hEntity) const
	{
		size_t hComp = indices->get(hEntity);
		if (hComp != INVALID_HANDLE)
		{
			return storage->getRaw(hComp);
		}
		else
		{
			return nullptr;
		}
	}

	DComponentStorage* storage;
	DIndexTable* indices;

	std::vector<FCreate> createFuncs;
	std::vector<FDestroy> destroyFuncs;
};

WECS_END