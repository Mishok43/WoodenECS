#pragma once

#include "stdafx.h"
#include "ECSTypeTraits.h"
#include "Handlers.h"
WECS_BEGIN

struct Slice
{
	Slice() = default;

	Slice(uint32_t iStart, uint32_t size):
		iStart(iStart),
		size(size)
	{ }

	uint32_t iStart;
	uint32_t size;
};

template<typename HeadCompT, typename... LeftCompTs>
class ComponentsGroup
{

public:
	using cmp_type_list = typename wecs::type_list<HeadCompT, LeftCompTs...>;

	template<typename CompT>
	const CompT* getRawData() const
	{
		return CompT::ecsData.data();
	}

	template<typename CompT=HeadCompT>
	HEntity getEntity(uint32_t i) const
	{
		return  HEntity(CompT::ecsData.getEntityHandle(i));
	}

	template<typename CompT = HeadCompT>
	const uint32_t size() const
	{
		return CompT::ecsData.size();
	}
};

template<typename HeadCompT, typename... LeftCompTs>
class ComponentsGroupSlice: public ComponentsGroup<HeadCompT, LeftCompTs...>
{
public:
	ComponentsGroupSlice() = default;

	ComponentsGroupSlice(Slice slice):
		slice(std::move(slice))
	{}

	Slice slice;
};



WECS_END