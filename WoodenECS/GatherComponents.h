#pragma once
#include "stdafx.h"

WECS_BEGIN

template<int I, typename ...ComponentsT>
struct WGatherComponents
{
	static constexpr int SIZE = sizeof...(ComponentsT);
	std::array<void*, SIZE> cmpsData;

	bool setupCmpsDataForEntity(size_t hEntity)
	{
		correctEntity = true;
		cmpsData = { getComponentData<ComponentsT>(hEntity)... };
		return correctEntity;
	}

	template<typename CompT>
	CompT& getComp()
	{
		return getComp_<0, CompT, ComponentsT...>();
	}

protected:
	template<typename curCT>
	inline void* getComponentData(size_t hEntity)
	{
		if (!correctEntity)
		{
			return nullptr;
		}

		void* res = curCT::ecsData.getByEntityHandleRaw(hEntity);
		if (!res)
		{
			correctEntity = false;
		}

		return res;
	}

	template<uint8_t iCur, uint8_t iNeed, typename curCT>
	inline void* getComponentData(size_t hEntity)
	{
		static_assert(iCur == iNeed);
		return curCT::ecsData.getByEntityHandleRaw(hEntity);
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

	bool correctEntity;
};

template<int I>
struct WGatherComponents<I>
{
	bool setupCmpsDataForEntity(size_t hEntity)
	{
		return true;
	}
};

template<int I, int NumEntities, typename ...ComponentsT>
struct WGatherGroupComponents
{
	static constexpr int SIZE = sizeof...(ComponentsT);
	std::array<std::array<void*, SIZE>, NumEntities> cmpsData;

	bool setupCmpsDataForEntity(size_t i, size_t hEntity)
	{
		correctEntity = true;
		cmpsData[i] = { getComponentData<ComponentsT>(hEntity)... };
		return correctEntity;
	}

	template<typename CompT>
	CompT& getComp(size_t i)
	{
		return getComp_<0, CompT, ComponentsT...>(i);
	}

protected:
	template<typename curCT>
	inline void* getComponentData(size_t hEntity)
	{
		if (!correctEntity)
		{
			return nullptr;
		}

		void* res = curCT::ecsData.getByEntityHandleRaw(hEntity);
		if (!res)
		{
			correctEntity = false;
		}

		return res;
	}

	template<uint8_t iCur, uint8_t iNeed, typename curCT>
	inline void* getComponentData(size_t hEntity)
	{
		static_assert(iCur == iNeed);
		return curCT::ecsData.getByEntityHandleRaw(hEntity);
	}

	template<uint8_t Index, typename CompT, typename CurCompT, typename ...LeftComponentsT>
	CompT& getComp_(size_t i)
	{
		return getComp_<Index, CompT, CurCompT, LeftComponentsT...>(i, std::is_same<CompT, CurCompT>());
	}

	template<uint8_t Index, typename CompT, typename CurCompT, typename NextCompT, typename ...LeftComponentsT>
	CompT& getComp_(size_t i, std::false_type&&)
	{
		return getComp_<Index + 1, CompT, NextCompT, LeftComponentsT...>(i, std::is_same<CompT, NextCompT>());
	}

	template<uint8_t Index, typename CompT, typename CurCompT, typename ...LeftComponentsT>
	CompT& getComp_(size_t i, std::true_type&&)
	{
		return *(CompT*)(cmpsData[i][Index]);
	}

	bool correctEntity;
};



WECS_END