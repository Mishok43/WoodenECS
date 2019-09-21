#pragma once
#include "stdafx.h"
#include "DComponent.h"
#include "DIndexTable.h"
#include "ECSTypeTraits.h"
WECS_BEGIN



//#define DECL_MANAGED_FLAT_COMP_DATA(ComponentT, defNumObjects) \
//public:\
//using CompStorageT = typename WComponents<ComponentT, defNumObjects, DIndexTableFlat>; \
//static constexpr bool IsManaged = true; \ 
//private:\
//static CompStorageT ecsData; \
//friend class wecs::WECS;\


using FDestroy = std::function<void(void*)>;
using FCreate = std::function<void(size_t, void*)>;

template<typename ComponentT, uint16_t defNumObjects,typename IntexTableT>
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


	void clear()
	{
		HComponentStorage<ComponentT>::clear();
		indices.clear();
	}

	void* getByEntityHandleRaw(size_t hEntity)
	{
		size_t hComp = indices.get(hEntity);
		if (hComp != INVALID_HANDLE)
		{
			return CompStorageT::getRaw(hComp);
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

	IntexTableT indices;

	std::vector<FCreate> createFuncs;
	std::vector<FDestroy> destroyFuncs;
};

#define DECL_MANAGED_DENSE_COMP_DATA(ComponentT, defNumObjects) \
public:\
using CompStorageT = typename WComponents<ComponentT, defNumObjects, DIndexTableFlat>; \
static CompStorageT ecsData;
//private:\
//friend class wecs::WECS;

//using DCompStorageT = typename DComponentStorage<has_hentity_inside_comp_v<ComponentT>>;\

#define DECL_MANAGED_FLAT_COMP_DATA(ComponentT, defNumObjects) \
public:\
using CompStorageT = typename WComponents<ComponentT, defNumObjects,DIndexTableHash>; \
static CompStorageT ecsData;
//private:\
//friend class wecs::WECS;

#define DECL_OUT_COMP_DATA(ComponentT)\
ComponentT::CompStorageT ComponentT::ecsData;



WECS_END