#pragma once
#include "stdafx.h"
#include "DComponent.h"
#include "DIndexTable.h"

WECS_BEGIN


#define COMPONENT_DATA_INCLASS(ComponentT, defNumObjects) private:\
static WComponents<ComponentT, defNumObjects> ecsData; \
friend class wecs::WECS;

#define COMPONENT_DATA_OUTCLASS(ComponentT, defNumObjects)\
WComponents<ComponentT, defNumObjects> ComponentT::ecsData;

#define COMPONENT_INDEXTABLEISHASH_DATA_INCLASS(ComponentT, defNumObjects) private:\
static WComponents<ComponentT, defNumObjects, DIndexTableHash> ecsData; \
friend class wecs::WECS;

#define COMPONENT_INDEXTABLEISHASH_DATA_OUTCLASS(ComponentT, defNumObjects)\
WComponents<ComponentT, defNumObjects, DIndexTableHash> ComponentT::ecsData;

using FDestroy = std::function<void(void*)>;
using FCreate = std::function<void(size_t, void*)>;

template<typename ComponentT, uint16_t defNumObjects, typename DIntexTableT = DIndexTableFlat>
class WComponents: public HComponentStorage<ComponentT>
{
public:
	WComponents()
	{
		indices.init(this);
		DComponentStorage::reserve(defNumObjects);
	}

	WComponents(const WComponents& wc) = delete;
	WComponents operator =(const WComponents& wc) = delete;
	WComponents& operator =(WComponents wc) = delete;

	void* getByEntityHandleRaw(size_t hEntity)
	{
		size_t hComp = indices.get(hEntity);
		if (hComp != INVALID_HANDLE)
		{
			return DComponentStorage::getRaw(hComp);
		}
		else
		{
			return nullptr;
		}
	}

	const void* getByEntityHandleRaw(size_t hEntity) const
	{
		size_t hComp = indices.get(hEntity);
		if (hComp != INVALID_HANDLE)
		{
			return getRaw(hComp);
		}
		else
		{
			return nullptr;
		}
	}

	ComponentT* getByEntityHandle(size_t hEntity)
	{
		size_t hComp = indices.get(hEntity);
		if (hComp != INVALID_HANDLE)
		{
			return get(hComp);
		}
		else
		{
			return nullptr;
		}
	}

	ComponentT* getByEntityHandle(size_t hEntity) const
	{
		size_t hComp = indices.get(hEntity);
		if (hComp != INVALID_HANDLE)
		{
			return get(hComp);
		}
		else
		{
			return nullptr;
		}
	}

	DIntexTableT indices;

	std::vector<FCreate> createFuncs;
	std::vector<FDestroy> destroyFuncs;
};

WECS_END