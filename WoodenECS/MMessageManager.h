#pragma once

#include "stdafx.h"
#include "ECSTypeTraits.h"
#include <vector>

WECS_BEGIN


template<typename MessageList, uint16_t PayLoadSize = 128>
class MMessageManager
{
public:
	struct GeneralMessage
	{
		//uint16_t type;
	};

	using MessageObservers = typename std::vector<std::function<void(const GeneralMessage&)>>;

	template<typename SystemT>
	void registerObserver(SystemT* system)
	{

	}

protected:

	template<typename SystemT, template... MessageTs>
	void registerObserverVariadic(SystemT system, type_list<MessageTs...>&&)
	{
		int unused[] = { (registerObserver_<SystemT, MessageTs(system)>, 0)... };
	}

	template<typename SystemT, template MessageT>
	void registerObserver_(SystemT system)
	{
		if constexpr (has_message<SystemT, MessageT>)
		{

		}
	}

	std::vector<MessageObservers> observers;
};

WECS_END

