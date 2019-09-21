#pragma once

#include "stdafx.h"

WECS_BEGIN


struct HEntity
{
	uint32_t h = INVALID_HANDLE;

	HEntity()
	{

	}

	HEntity(uint32_t h) :
		h(h)
	{
	}

	//template<typename Component>
	//HCompRW<Component> getComponentHandleRW()
	//{
	//	assert(h != INVALID_HANDLE);
	//	size_t hComp = Component::ecsData.indices.get(h);
	//	assert(hComp != INVALID_HANDLE);
	//	return HCompRW(hComp);
	//}

	//template<typename Component>
	//HCompR<Component> getComponentHandleR() const
	//{
	//	assert(h != INVALID_HANDLE);
	//	size_t hComp = Component::ecsData.indices.get(h);
	//	assert(hComp != INVALID_HANDLE);
	//	return HCompR(hComp);
	//}

	template<typename Component>
	bool hasComponent() const
	{
		size_t hComp = Component::ecsData.indices.get(h);
		return hComp != INVALID_HANDLE;
	}

	template<typename Component>
	Component& getComponent()
	{
		assert(h != INVALID_HANDLE);
		size_t hComp = Component::ecsData.indices.get(h);
		assert(hComp != INVALID_HANDLE);

		return Component::ecsData[hComp];
	}

	template<typename Component>
	const Component& getComponent() const
	{
		assert(h != INVALID_HANDLE);
		size_t hComp = Component::ecsData.indices.get(h);
		assert(hComp != INVALID_HANDLE);

		return Component::ecsData[hComp];
	}

	bool operator==(const HEntity& hEntity1) const
	{
		return h == hEntity1.h;
	}

	operator uint32_t()
	{
		return h;
	}

	//operator bool()
	//{
	//	return h != INVALID_HANDLE;
	//}
};


template<typename CompT>
struct HCompBase
{
	size_t handle = INVALID_HANDLE;

	HCompBase()
	{
	}

	HCompBase(size_t handle) :
		handle(std::move(handle))
	{
	}

	constexpr bool isCompManaged() const
	{
		return CompT::IsManaged;
	}

	operator bool()
	{
		return handle != INVALID_HANDLE;
	}

};
/*

template<typename CompT, bool IsManaged>
struct HCompBaseT
{
};

template<typename CompT>
struct HCompBaseT<CompT, true> : public HCompBase<CompT>
{
	HCompBaseT()
	{
	}

	HCompBaseT(size_t handle) :
		HCompBase(std::move(handle))
	{
	}

protected:
	size_t getCompPos() const
	{
		assert(handle != INVALID_HANDLE);
		size_t pos = Component::ecsData.indices.get(handle);
		assert(pos != INVALID_HANDLE);
		return pos;
	}
};

template<typename CompT>
struct HCompBaseT<CompT, false> : public HCompBase<CompT>
{
	HCompBaseT()
	{
	}

	HCompBaseT(size_t handle) :
		HCompBase(std::move(handle))
	{
	}

protected:
	size_t getCompPos() const
	{
		assert(handle != INVALID_HANDLE);
		return handle;
	}
};*/
//
//template<typename CompT>
//struct HCompRW: public HCompBaseT<CompT, CompT::IsManaged>
//{	
//	HCompRW(){ }
//
//	HCompRW(size_t handle) :
//		HCompBaseT(std::move(handle))
//	{}
//
//	CompT& get()
//	{
//		return Component::ecsData[getCompPos()];
//	}
//	
//	const CompT& get() const
//	{
//		return Component::ecsData[getCompPos()];
//	}
//
//	HCompR readOnly() const
//	{
//		return HCompR<CompT>(*this);
//	}
//};
//
//template<typename CompT>
//struct HCompR : public HCompBaseT<CompT, CompT::IsManaged>
//{
//	HCompR()
//	{}
//
//	HCompR(size_t handle) :
//		HCompBaseT(std::move(handle))
//	{}
//
//	const CompT& get() const
//	{
//		return Component::ecsData[getCompPos()];
//	}
//};
//



//
//template<typename... Ts>
//struct HCompUniversal
//{
//	using cmp_type_list = typename type_list<Ts...>;
//
//	HCompUniversal(){ }
//
//	HCompUniversal(size_t handle) :
//		handle(handle)
//	{}
//
//	
//	HCompUniversal(const HCompUniversal<CompDummy>& dummy):
//		handle(dummy.handle)
//	{}
//
//	template<typename T>
//	explicit HCompUniversal(const HCompRW<T>& hComp) :
//		handle(hComp.handle)
//	{}
//
//	size_t handle = INVALID_HANDLE;
//
//	template<typename CompT>
//	CompT& get()
//	{
//		assert(handle != INVALID_HANDLE);
//		return Component::ecsData[handle];
//	}
//
//	template<typename CompT>
//	const CompT& get() const
//	{
//		assert(handle != INVALID_HANDLE);
//		return Component::ecsData[handle];
//	}
//
//	operator bool() const
//	{
//		return handle != INVALID_HANDLE;
//	}
//};
//
//struct HCompUniversal<CompDummy>
//{
//	HCompUniversal(){ }
//
//	HCompUniversal(size_t handle) :
//		handle(handle)
//	{
//	}
//
//	size_t handle = INVALID_HANDLE;
//
//	operator bool() const
//	{
//		return handle != INVALID_HANDLE;
//	}
//};
WECS_END

