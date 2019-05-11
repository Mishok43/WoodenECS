#pragma once
#include "stdafx.h"
#include "DComponent.h"
#include "SSEHashMap/phmap.h"
#include <vector>

WECS_BEGIN

class DIndexTableFlat
{
public:
	void init(DComponentStorage* _compStorage) 
	{
		indices.resize(8, INVALID_HANDLE);
		compStorage = _compStorage;
	}

	size_t insert(size_t hEntity) 
	{
		assert(!exists(hEntity));

		size_t index = compStorage->size();
		if (hEntity >= indices.size())
		{
			size_t newSize = indices.size();
			while (newSize <= hEntity)
			{
				newSize *= 2;
			}
			indices.resize(newSize, INVALID_HANDLE);
		}

		indices[hEntity] = index;
		return index;
	}

	size_t remove(size_t hEntity) 
	{
		size_t deadPos = indices[hEntity];

#ifdef _DEBUG
		if (deadPos >= size())
		{
			return INVALID_HANDLE;
		}
#endif

		indices[hEntity] = INVALID_HANDLE;

		size_t hSwappedEntity = compStorage->remove(deadPos);
		if (hSwappedEntity != INVALID_HANDLE)
		{
			indices[hSwappedEntity] = deadPos;
		}

		return deadPos;
	}

	size_t removeByIndex(size_t hComp) 
	{
#ifdef _DEBUG
		if (hComp >= size())
		{
			return INVALID_HANDLE;
		}
#endif
		size_t hEntity = compStorage->getEntityHandle(hComp);
		indices[hEntity] = INVALID_HANDLE;

		size_t hSwappedEntity = compStorage->remove(hComp);
		if (hSwappedEntity != INVALID_HANDLE)
		{
			indices[hSwappedEntity] = hComp;
		}

		return hComp;
	}


	inline bool exists(size_t hEntity) const 
	{
		if (hEntity >= indices.size())
			return false;

		return indices[hEntity] != INVALID_HANDLE;
	}
	
	inline size_t get(size_t hEntity) const 
	{
		if (hEntity >= indices.size())
			return INVALID_HANDLE;

		return indices[hEntity];
	}

	inline void clear() 
	{
		indices.clear();
		compStorage->clear();
	}

	inline size_t size() const 
	{
		return compStorage->size();
	}

protected:
	std::vector<size_t> indices;
	DComponentStorage* compStorage = nullptr;	
};



class DIndexTableHash 
{
public:

	void init(DComponentStorage* _compStorage) 
	{
		compStorage = _compStorage;
	}

	size_t insert(size_t hEntity) 
	{
		assert(!exists(hEntity));

		size_t index = compStorage->size();
		indices[hEntity] = index;

		return index;
	}

	size_t remove(size_t hEntity) 
	{
		phmap::flat_hash_map<uint32_t, uint32_t>::iterator iEntity = indices.find(hEntity);
		size_t deadPos = iEntity->second;
		if (deadPos >= size())
		{
			return INVALID_HANDLE;
		}

		iEntity->second = INVALID_HANDLE;
		size_t hSwappedEntity = compStorage->remove(deadPos);
		if (hSwappedEntity != INVALID_HANDLE)
		{
			indices[hSwappedEntity] = deadPos;
		}

		return deadPos;
	}

	size_t removeByIndex(size_t hComp) 
	{
		if (hComp >= size())
		{
			return INVALID_HANDLE;
		}

		size_t hEntity = compStorage->getEntityHandle(hComp);
		indices[hEntity] = INVALID_HANDLE;

		size_t hSwappedEntity = compStorage->remove(hComp);
		if (hSwappedEntity != INVALID_HANDLE)
		{
			indices[hSwappedEntity] = hComp;
		}

		return hComp;
	}


	inline bool exists(size_t hEntity) const 
	{
		return indices.find(hEntity) != indices.cend();
	}

	inline size_t get(size_t hEntity) const 
	{
		phmap::flat_hash_map<uint32_t, uint32_t>::const_iterator it = indices.find(hEntity);
		if(it == indices.cend())
			return INVALID_HANDLE;

		return it->second;
	}

	inline void clear() 
	{
		indices.clear();
		compStorage->clear();
	}

	inline size_t size() const 
	{
		return compStorage->size();
	}

protected:
	phmap::flat_hash_map<uint32_t, uint32_t> indices;
	DComponentStorage* compStorage = nullptr;
};

WECS_END