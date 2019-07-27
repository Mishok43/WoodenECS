#pragma once
#include "stdafx.h"
#include "DComponent.h"
#include "DIndexTable.h"

WECS_BEGIN


#define DECL_MANAGED_FLAT_COMP_DATA(ComponentT, defNumObjects) \
public:\
using CompStorageT = typename WComponents<ComponentT, defNumObjects, DIndexTableFlat>; \
private:\
static CompStorageT ecsData; \
friend class wecs::WECS;\

#define DECL_MANAGED_DENSE_COMP_DATA(ComponentT, defNumObjects) \
public:\
using CompStorageT = typename WComponents<ComponentT, defNumObjects, DIndexTableHash>; \
private:\
static CompStorageT ecsData; \
friend class wecs::WECS;

#define DECL_UNMANAGED_FLAT_COMP_DATA(ComponentT, defNumObjects) \
public:\
using CompStorageT = typename WComponents<ComponentT, defNumObjects, DIndexTableFlat, DComponentStorageUnmanaged>;\
private:\
static CompStorageT ecsData; \
friend class wecs::WECS;

#define DECL_UNMANAGED_DENSE_COMP_DATA(ComponentT, defNumObjects)\
public:\
using CompStorageT = typename WComponents<ComponentT, defNumObjects, DIndexTableHash, DComponentStorageUnmanaged>;\
private:\
static CompStorageT ecsData; \
friend class wecs::WECS;

#define DECL_OUT_COMP_DATA(ComponentT)\
ComponentT::CompStorageT ComponentT::ecsData;


using FDestroy = std::function<void(void*)>;
using FCreate = std::function<void(size_t, void*)>;

template<typename ComponentT, uint16_t defNumObjects, typename DIntexTableT = DIndexTableFlat, typename CompStorageT = DComponentStorage>
class WComponents: public HComponentStorage<ComponentT, CompStorageT>
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