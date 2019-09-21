#pragma once 

#include "stdafx.h"
#include "DComponent.h"
#include "DIndexTable.h"
#include "WComponents.h"
#include "MHandleManager.h"
#include "ECSTypeTraits.h"
#include "SSEHashMap/phmap.h"
#include "ComponentsGroup.h"
#include "GatherComponents.h"
#include "Handlers.h"
using FUpdate = std::function<void(float)>;


WECS_BEGIN



struct CompDummy
{
	bool d;
};






class WECS
{
public:
	using this_type = typename WECS;

	//template<typename System, typename... Args>
	//void registerSystem(Args&&... args)
	//{
	//	using system_cmp_type_list = typename System::cmp_type_list;
	//	System* system = new System(std::forward<Args>(args)...);

	//	if constexpr (has_ecs_cmp_update<System, WECS, system_cmp_type_list, float>)
	//	{
	//		registerECSComponentUpdate(system, system_cmp_type_list());
	//	}

	//	if constexpr (has_cmp_update<System, system_cmp_type_list, float>)
	//	{
	//		registerComponentUpdate(system, system_cmp_type_list());
	//	}

	//	if constexpr (has_create<System, size_t, type_list_head<system_cmp_type_list>>)
	//	{
	//		registerCreate<System, type_list_head<system_cmp_type_list>>(system);
	//	}

	//	if constexpr (has_destroy<System, type_list_head<system_cmp_type_list>>)
	//	{
	//		registerDestroy<System, type_list_head<system_cmp_type_list>>(system);
	//	}
	//}

	template<typename F, typename HeadComponentT, typename... LeftComponentTs>
	void for_each(F&& func, type_list<HeadComponentT, LeftComponentTs...>&&)
	{
		WGatherComponents<0, LeftComponentTs...> gather;

		for (size_t hComp = 0; hComp < HeadComponentT::ecsData.size(); ++hComp)
		{
			HeadComponentT& headComp = HeadComponentT::ecsData[hComp];
			size_t hEntity = HeadComponentT::ecsData.getEntityHandle(hComp);
			if (gather.setupCmpsDataForEntity(hEntity))
			{
				func(headComp, gather.getComp<LeftComponentTs>()...);
			}
		}
	}

	template<typename F, typename HeadComponentT>
	void for_each(F&& func, type_list<HeadComponentT>&&)
	{
		for (size_t hComp = 0; hComp < HeadComponentT::ecsData.size(); ++hComp)
		{
			HeadComponentT& headComp = HeadComponentT::ecsData[hComp];
			func(headComp);
		}
	}

	void update(float dtime)
	{
		for (size_t i = 0; i < updateFuncs.size(); ++i)
		{
			updateFuncs[i](dtime);
		}
	}


	inline HEntity createEntity()
	{
		return handleManager.allocate();
	}


	template<typename... ComponentTs>
	inline void removeEntity(HEntity hEntity)
	{
		int unused[] = { (removeComponent<ComponentTs>(hEntity), 0)... };
		handleManager.deallocate(hEntity);
	}

	template<typename Component, typename... Args>
	Component& addComponent(HEntity hEntity, Args&&... args)
	{
		Component::ecsData.indices.insert(hEntity);
		Component& c = Component::ecsData.append<Component, Args...>(hEntity, std::forward<Args>(args)...);

		//for (size_t i = 0; i < Component::ecsData.createFuncs.size(); ++i)
		//{
		//	Component::ecsData.createFuncs[i](hEntity, &c);
		//}

		return c;
	}

	/*template<typename Component, typename... Args>
	HCompRW<Component>& addSharedComponent(size_t hEntity, Args&&... args)
	{
		Component::ecsData.indices.insert(hEntity);
		HCompRW<Component> hComp = Component::ecsData.append<Component, Args...>(hEntity, std::forward<Args>(args)...);
		Component& c = hComp.get();
		for (size_t i = 0; i < Component::ecsData.createFuncs.size(); ++i)
		{
			Component::ecsData.createFuncs[i](hEntity, &c);
		}

		return hComp;
	}

	template<typename Component, typename... Args>
	Component& addSharedComponent(size_t hEntity, Args&&... args)
	{
		HCompRW<Component> hComp = addSharedComponent(hEntity, std::forward<Args>(args));
		Component& c = hComp.get();
		return c;
	}*/

	template<typename Component>
	Component& getComponent(HEntity hEntity)
	{
		return hEntity.getComponent<Component>();
	}

	template<typename ComponentT>
	ComponentT* getComponentsRawData()
	{
		return ComponentT::ecsData.data();
	}

	/*template<typename ComponentT>
	ComponentT& getComponent(const HCompRW<ComponentT>& hComp)
	{
		return hComp.get();
	}*/

	template<typename Component>
	size_t getNumbrComponents()
	{
		return Component::ecsData.size();
	}

	template<typename Component, typename F>
	void removeComponentsIf(F&& f)
	{
		size_t hComp = 0;
		while(hComp < Component::ecsData.size())
		{
			if (f(Component::ecsData[hComp]))
			{
				removeComponentByIndex<Component>(hComp);
			}
			else
			{
				++hComp;
			}
		}
	}

	template<typename ComponentT>
	void removeComponent(HEntity hEntity)
	{
		removeComponentByIndex<ComponentT>(ComponentT::ecsData.indices.get(hEntity));
	}


	template<typename ComponentT>
	void removeComponentByIndex(size_t hComp)
	{
		void* comp = ComponentT::ecsData.getRaw(hComp);
		assert(comp);

		for (size_t i = 0; i < ComponentT::ecsData.destroyFuncs.size(); ++i)
		{
			ComponentT::ecsData.destroyFuncs[i](comp);
		}

		ComponentT::ecsData.indices.removeByIndex(hComp);
	}


	template<typename ComponentT>
	void clearComponents()
	{
		ComponentT::ecsData.clear();
	}

	template<typename ComponentT>
	void deleteEntitiesOfComponents()
	{
		for (uint16_t i = 0; i < ComponentT::ecsData.size(); i++)
		{
			HEntity hEntity = ComponentT::ecsData.getEntityHandle(i);
			removeEntity(hEntity);
		}
	}

protected:



	//template<typename System>
	//void registerECSUpdate(System* s)
	//{
	//	registerUpdate([s, this](float dTime)
	//	{
	//		s->update(*this, dTime);
	//	});
	//}

	//void registerUpdate(FUpdate&& updateFunc)
	//{
	//	updateFuncs.emplace_back(updateFunc);
	//}

	//template<typename System, typename HeadComponentT, typename... LeftComponentsT>
	//void registerECSComponentUpdate(System* s, type_list<HeadComponentT, LeftComponentsT...>&&)
	//{
	//	registerUpdate([s, this](float dTime)
	//	{
	//		WGatherComponents<0, LeftComponentsT...> gather;

	//		for (size_t hComp = 0; hComp < HeadComponentT::ecsData.size(); ++hComp)
	//		{
	//			HeadComponentT& headComp = HeadComponentT::ecsData[hComp];
	//			size_t hEntity = HeadComponentT::ecsData.getEntityHandle(hComp);
	//			if (gather.setupCmpsDataForEntity(hEntity))
	//			{
	//				s->update(*this, headComp, gather.getComp<LeftComponentsT>()..., dTime);
	//			}
	//		}
	//	});
	//}

	//template<typename System, typename HeadComponentT, typename... LeftComponentsT>
	//void registerComponentUpdate(System* s, type_list<HeadComponentT, LeftComponentsT...>&&)
	//{
	//	registerUpdate([s, this](float dTime)
	//	{
	//		WGatherComponents<0, LeftComponentsT...> gather;

	//		for (size_t hComp = 0; hComp < HeadComponentT::ecsData.size(); ++hComp)
	//		{
	//			HeadComponentT& headComp = HeadComponentT::ecsData[hComp];
	//			size_t hEntity = HeadComponentT::ecsData.getEntityHandle(hComp);
	//			if (gather.setupCmpsDataForEntity(hEntity))
	//			{
	//				s->update(headComp, gather.getComp<LeftComponentsT>()..., dTime);
	//			}
	//		}
	//	});
	//}

	//template<typename System, typename ComponentT>
	//void registerDestroy(System* s)
	//{
	//	ComponentT::ecsData.destroyFuncs.emplace_back([s](void* data)
	//	{
	//		s->destroy(*((ComponentT*)data));
	//	});
	//}

	//template<typename System, typename ComponentT>
	//void registerCreate(System* s)
	//{
	//	ComponentT::ecsData.createFuncs.emplace_back([s](size_t hEntity, void* data)
	//	{
	//		s->create(hEntity, *((ComponentT*)data));
	//	});
	//}


	std::vector<FUpdate> updateFuncs;
	MHandleManager handleManager;
};

WECS_END
