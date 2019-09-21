#pragma once
#include "stdafx.h"
#include "WECS.h"
#include "ComponentsGroup.h"
#include "WoodenAllocators/AllocatorLinear.h"
#include "GatherComponents.h"

WECS_BEGIN


using JobHandle = typename uint16_t;


//template<typename AllocT>
//class JobSystem
//{
//	template<typename T, typename std::enable_if_t<std::is_base_of_v<Job, T>>>
//	JobHandle* createJob(uint16_t minSlicingSize = (uint16_t)-1)
//	{
//		T* job = (T*)allocator.allocMem(sizeof(T));
//		jobs.emplace_back(std::move(job));
//	}
//
//	AllocT allocator;
//	std::vector<JobHandle*, AllocT> jobs;
//};

class Job
{
public:

	virtual void run(WECS* ecs)
	{
		update(ecs);
		finish(ecs);
	}

	void setAllocator(wal::Allocator* alloc)
	{
		this->alloc = alloc;
	}

	wal::Allocator& getAllocator()
	{
		return *alloc;
	}

	void setAllocatorTemp(wal::AllocatorLinear* allocTemp)
	{
		this->allocTemp = allocTemp;
	}

	wal::AllocatorLinear& getAllocatorTemp()
	{
		return *allocTemp;
	}

	virtual void update(WECS* ecs) = 0;

	virtual void finish(WECS* ecs)
	{
	}

	virtual bool isParallazible() const
	{
		return false;
	}

	template<typename F, typename... CompTs>
	void for_each(F && func, const ComponentsGroup<CompTs...>& compGroup)
	{
		for_each(std::move(func), compGroup.cmp_type_list());
	}

	template<typename F, typename... CompTs>
	void for_each(F && func, const ComponentsGroupSlice<CompTs...>& compGroup)
	{
		using cmp_type_list_t = typename ComponentsGroupSlice<CompTs...>::cmp_type_list;
		for_each(std::move(func), cmp_type_list_t(), compGroup.slice.iStart, compGroup.slice.size);
	}

	template<typename F, typename HeadComponentT, typename... LeftComponentTs>
	void for_each(F&& func, type_list<HeadComponentT, LeftComponentTs...>&&, uint16_t iStart = 0, uint16_t size = (uint16_t)-1)
	{
		if (size == (uint16_t)-1)
		{
			size = HeadComponentT::ecsData.size();
		}

		uint16_t end = iStart + size;
		if (end > HeadComponentT::ecsData.size())
		{
			end = HeadComponentT::ecsData.size();
		}

		WGatherComponents<0, LeftComponentTs...> gather;
		for (size_t hComp = iStart; hComp < iStart + size; ++hComp)
		{
			HeadComponentT& headComp = HeadComponentT::ecsData[hComp];
			size_t hEntity = HeadComponentT::ecsData.getEntityHandle(hComp);
			if (gather.setupCmpsDataForEntity(hEntity))
			{
				func(hEntity, headComp, gather.getComp<LeftComponentTs>()...);
			}
		}
	}

	

	template<typename... CompTs>
	ComponentsGroup<CompTs...> queryComponentsGroup()
	{
		return ComponentsGroup<CompTs...>();
	}


	template<typename... CompTs>
	ComponentsGroupSlice<CompTs...> queryComponentsGroupSlice(Slice slice)
	{
		ComponentsGroupSlice compsSlice = ComponentsGroupSlice<CompTs...>(std::move(slice));
		return compsSlice;
	}

	wal::AllocatorLinear* allocTemp;
	wal::Allocator* alloc;
};



class JobParallazible: public Job
{
public:
	JobParallazible(uint16_t numThreads=0):
		nThreads(numThreads)
	{}

	virtual void run(WECS* ecs) override
	{
		updateNStartThreads(1);
		update(ecs);
		finish(ecs);
	}

	void update(WECS* ecs) override
	{
		update(ecs, 0);
	}

	virtual void update(WECS* ecs, uint8_t iThread) = 0;


	bool isParallazible() const override
	{
		return true;
	}

	virtual void updateNStartThreads(uint32_t nWorkThreads){}

	uint16_t getNumThreads() const
	{
		return nThreads;
	}

	uint8_t nThreads;
};

template<typename... CompTs>
class JobParallaziblePerCompGroup : public JobParallazible
{
public:
	JobParallaziblePerCompGroup(uint32_t sliceSize=1024)
	{
		this->sliceSize = sliceSize;
	}

	uint32_t sliceSize;

	void updateNStartThreads(uint32_t nWorkThreads) final
	{
		uint32_t nEntities = queryComponentsGroup<CompTs...>().size();
		nThreads = std::min(nWorkThreads, (nEntities+sliceSize-1)/sliceSize);
	}

	void update(WECS* ecs, uint8_t iThread) final
	{
		uint32_t nRequests = queryComponentsGroup<CompTs...>().size();
		uint32_t sliceSize = (nRequests + nThreads - 1) / nThreads;

		ComponentsGroupSlice<CompTs...> compsGroup =
			queryComponentsGroupSlice<CompTs...>(Slice(iThread * sliceSize, sliceSize));
		for_each([ecs, this](HEntity hEntity, CompTs&... comps)
		{
			this->update(ecs, hEntity, std::forward<CompTs&>(comps)...);
		}, compsGroup);
	}

	virtual void update(WECS* ecs, HEntity hEntity,CompTs&... comps) = 0;

};

WECS_END