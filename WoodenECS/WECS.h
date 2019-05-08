#include "stdafx.h"
#include "DComponent.h"
#include "DIndexTable.h"
#include "WComponents.h"
#include "MHandleManager.h"
#include "ECSTypeTraits.h"
#include "SSEHashMap/phmap.h"

using FUpdate = std::function<void(float)>;

WECS_BEGIN

class WECS
{
public:
	using this_type = typename WECS;

	template<typename ComponentT, typename IndexTable = DIndexTableFlat>
	void registerComponent()
	{
		WComponents* comp = new WComponents;
		comp->storage = new HComponentStorage<ComponentT>();
		comp->storage->reserve(8);

		comp->indices = new IndexTable(comp->storage);
		comp->indices->init();
		
		components.push_back(comp);
		componentsMap[&typeid(ComponentT)] = comp;
	}

	template<typename System, typename... Args>
	void registerSystem(Args&&... args)
	{
		using system_cmp_type_list = typename System::cmp_type_list;
		System* system = new System(std::forward<Args>(args)...);

		if constexpr(has_ecs_cmp_update<System, WECS, system_cmp_type_list, float>)
		{
			registerECSComponentUpdate(system, system_cmp_type_list());
		}

		if constexpr (has_cmp_update<System, system_cmp_type_list, float>)
		{
			registerComponentUpdate(system, system_cmp_type_list());
		}

		if constexpr (has_create<System, size_t, type_list_head<system_cmp_type_list>>)
		{
			registerCreate<System, type_list_head<system_cmp_type_list>>(system);
		}	

		if constexpr (has_destroy<System, type_list_head<system_cmp_type_list>>)
		{
			registerDestroy<System, type_list_head<system_cmp_type_list>>(system);
		}
	}

	template<typename F, typename HeadComponentT, typename... LeftComponentTs>
	void for_each(F&& func, type_list<HeadComponentT, LeftComponentTs...>&&)
	{
		WComponents* headCompsWrapper = getComponentsWrapper<HeadComponentT>();
		HComponentStorage<HeadComponentT>* headCompsStorage = storage_cast<HeadComponentT>(headCompsWrapper->storage);
		
		WGatherComponents<0, LeftComponentTs...> gather(*this);

		for (size_t hComp = 0; hComp < headCompsStorage->size(); ++hComp)
		{
			HeadComponentT& headComp = (*headCompsStorage)[hComp];
			size_t hEntity = headCompsStorage->getEntityHandle(hComp);
			if (gather.setupCmpsDataForEntity(hEntity))
			{
				func(headComp, gather.getComp<LeftComponentTs>()...);
			}
		}
	}

	void update(float dtime)
	{
		for (size_t i = 0; i < updateFuncs.size(); ++i)
		{
			updateFuncs[i](dtime);
		}
	}

	template<typename ComponentT>
	HComponentStorage<ComponentT>* getComponentStorage()
	{
		WComponents* compWrapper = getComponentsWrapper<ComponentT>();
		assert(compWrapper);

		return storage_cast<ComponentT>(compWrapper->storage);
	}

	inline size_t createEntity()
	{
		return handleManager.allocate();
	}

	template<typename... T>
	inline void w(T... arg){ }

	template<typename... ComponentTs>
	inline void removeEntity(size_t hEntity)
	{
		w(removeComponentVariadic<ComponentTs>(hEntity)...);

		handleManager.deallocate(hEntity);
	}

	template<typename Component, typename... Args>
	Component& addComponent(size_t hEntity, Args&&... args)
	{
		WComponents* compWrapper = getComponentsWrapper<Component>();
		compWrapper->indices->insert(hEntity);
		Component& c = compWrapper->storage->append<Component, Args...>(hEntity, std::forward<Args>(args)...);

		for (size_t i = 0; i < compWrapper->createFuncs.size(); ++i)
		{
			compWrapper->createFuncs[i](hEntity, &c);
		}

		return c;
	}
	
	template<typename Component>
	Component& getComponent(size_t hEntity)
	{
		WComponents* compWrapper = getComponentsWrapper<Component>();
		size_t hComp = compWrapper->indices->get(hEntity);
		assert(hComp != INVALID_HANDLE);

		HComponentStorage<Component>* compStorage = storage_cast<Component>(compWrapper->storage);
		return (*compStorage)[hComp];
	}

	template<typename Component>
	size_t getNumbrComponents()
	{
		WComponents* compWrapper = getComponentsWrapper<Component>();
		return compWrapper->storage->size();
	}

	template<typename Component, typename F>
	void removeComponentsIf(F&& f)
	{
		WComponents* compWrapper = getComponentsWrapper<Component>();
		HComponentStorage<Component>* compStorage = storage_cast<Component>(compWrapper->storage);

		size_t hComp = 0;
		while(hComp < compStorage->size())
		{
			if (f((*compStorage)[hComp]))
			{
				removeComponent(hComp, compWrapper);
			}
			else
			{
				++hComp;
			}
		}
	}
	
	template<typename Component>
	int removeComponentVariadic(size_t hEntity)
	{
		WComponents* compWrapper = getComponentsWrapper<Component>();
		removeComponent(compWrapper->indices->get(hEntity), compWrapper);
		return 0;
	}

	template<typename Component>
	void removeComponent(size_t hEntity)
	{
		WComponents* compWrapper = getComponentsWrapper<Component>();
		removeComponent(compWrapper->indices->get(hEntity), compWrapper);
	}

	template<typename Component>
	void removeComponentByIndex(size_t hComp)
	{
		WComponents* compWrapper = getComponentsWrapper<Component>();
		removeComponent(hComp, compWrapper);
	}

	void removeComponent(size_t hComp, WComponents* compWrapper)
	{
		void* comp = compWrapper->storage->getRaw(hComp);
		assert(comp);

		for (size_t i = 0; i < compWrapper->destroyFuncs.size(); ++i)
		{
			compWrapper->destroyFuncs[i](comp);
		}

		compWrapper->indices->removeByIndex(hComp);
	}

protected:
	template<int I, typename ...ComponentsT>
	struct WGatherComponents
	{
		static constexpr int SIZE = sizeof...(ComponentsT);
		WComponents* cmpsWraps[SIZE];
		void* cmpsData[SIZE];

		WGatherComponents(this_type& ecs)
		{
			fillCmpsWraps<0, ComponentsT...>(ecs);
		}


		bool setupCmpsDataForEntity(size_t hEntity)
		{
			for (uint8_t i = 0; i < SIZE; ++i)
			{
				cmpsData[i] = cmpsWraps[i]->getRaw(hEntity);
				if (!cmpsData[i])
				{
					return false;
				}
			}

			return true;
		}

		template<typename CompT>
		CompT& getComp()
		{
			return getComp_<0, CompT, ComponentsT...>();
		}
	protected:
		template<uint8_t CompIndex, typename CurComponentT, typename ...LeftComponentsT>
		void fillCmpsWraps(this_type& ecs)
		{
			cmpsWraps[CompIndex] = ecs.getComponentsWrapper<CurComponentT>();
			fillCmpsWraps<CompIndex + 1, LeftComponentsT...>(ecs);
		}

		template<uint8_t CompIndex>
		void fillCmpsWraps(this_type& ecs)
		{
		}

		template<uint8_t Index, typename CompT, typename CurCompT, typename ...LeftComponentsT>
		CompT& getComp_()
		{
			return getComp_<Index, CompT, CurCompT, LeftComponentsT...>(std::is_same<CompT, CurCompT>());
		}

		template<uint8_t Index, typename CompT, typename CurCompT, typename NextCompT, typename ...LeftComponentsT>
		CompT& getComp_(std::false_type&&)
		{
			return getComp_<Index + 1, CompT, NextCompT, LeftComponentsT...>(std::is_same<CompT, NextCompT>());
		}

		template<uint8_t Index, typename CompT, typename CurCompT, typename ...LeftComponentsT>
		CompT& getComp_(std::true_type&&)
		{
			return *(CompT*)(cmpsData[Index]);
		}


	};

	template<int I>
	struct WGatherComponents<I>
	{
		WGatherComponents(this_type& ecs)
		{}

		bool setupCmpsDataForEntity(size_t hEntity)
		{
			return true;
		}
	};


	template<typename System>
	void registerECSUpdate(System* s)
	{
		registerUpdate([s, this](float dTime)
		{
			s->update(*this, dTime);
		});
	}

	void registerUpdate(FUpdate&& updateFunc)
	{
		updateFuncs.emplace_back(updateFunc);
	}

	template<typename System, typename HeadComponentT, typename... LeftComponentsT>
	void registerECSComponentUpdate(System* s, type_list<HeadComponentT, LeftComponentsT...>&&)
	{
		WComponents* headCompsWrapper = getComponentsWrapper<HeadComponentT>();
		HComponentStorage<HeadComponentT>* headCompsStorage = storage_cast<HeadComponentT>(headCompsWrapper->storage);
		registerUpdate([s, this, headCompsStorage](float dTime)
		{
			WGatherComponents<0, LeftComponentsT...> gather(*this);

			for (size_t hComp = 0; hComp < headCompsStorage->size(); ++hComp)
			{
				HeadComponentT& headComp = (*headCompsStorage)[hComp];
				size_t hEntity = headCompsStorage->getEntityHandle(hComp);
				if (gather.setupCmpsDataForEntity(hEntity))
				{
					s->update(*this, headComp, gather.getComp<LeftComponentsT>()..., dTime);
				}
			}
		});
	}

	template<typename System, typename HeadComponentT, typename... LeftComponentsT>
	void registerComponentUpdate(System* s, type_list<HeadComponentT, LeftComponentsT...>&&)
	{
		WComponents* headCompsWrapper = getComponentsWrapper<HeadComponentT>();
		HComponentStorage<HeadComponentT>* headCompsStorage = storage_cast<HeadComponentT>(headCompsWrapper->storage);
		registerUpdate([s, this, headCompsStorage](float dTime)
		{
			WGatherComponents<0, LeftComponentsT...> gather(*this);

			for (size_t hComp = 0; hComp < headCompsStorage->size(); ++hComp)
			{
				HeadComponentT& headComp = (*headCompsStorage)[hComp];
				size_t hEntity = headCompsStorage->getEntityHandle(hComp);
				if (gather.setupCmpsDataForEntity(hEntity))
				{
					s->update(headComp, gather.getComp<LeftComponentsT>()..., dTime);
				}
			}
		});
	}

	template<typename System, typename Component>
	void registerDestroy(System* s)
	{
		
		WComponents* components = componentsMap[&typeid(Component)];
		assert(components);

		components->destroyFuncs.emplace_back([s](void* data)
		{
			s->destroy(*((Component*)data));
		});
	}

	template<typename System, typename Component>
	void registerCreate(System* s)
	{
		WComponents* components = componentsMap[&typeid(Component)];
		assert(components);

		components->createFuncs.emplace_back([s](size_t hEntity, void* data)
		{
			s->create(hEntity, *((Component*)data));
		});
	}

	template<typename Component>
	inline const WComponents* getComponentsWrapper() const
	{
		return componentsMap[&typeid(Component)];
	}

	template<typename Component>
	inline WComponents* getComponentsWrapper()
	{
		return componentsMap[&typeid(Component)];
	}

	std::vector<WComponents*> components;
	std::vector<FUpdate> updateFuncs;
	MHandleManager handleManager;
	phmap::flat_hash_map<const std::type_info*, WComponents*> componentsMap;
};

WECS_END
