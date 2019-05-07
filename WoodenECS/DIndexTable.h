#pragma once
#include "stdafx.h"
#include "DComponent.h"
#include "SSEHashMap/phmap.h"
#include <vector>

WECS_BEGIN

__declspec(novtable) class DIndexTable
{
public:
	virtual void init() = 0;
	virtual size_t insert(size_t hEntity) = 0;
	virtual inline bool exists(size_t hEntity) const = 0;
	virtual inline size_t get(size_t hEntity) const = 0;
	virtual size_t remove(size_t hEntity) = 0;
	virtual size_t removeByIndex(size_t hComp) = 0;
	virtual inline void clear() = 0;
	virtual inline size_t size() const = 0;
};

class DIndexTableFlat: public DIndexTable
{
public:
	explicit DIndexTableFlat(DComponentStorage* compStorage):
		compStorage(compStorage)
	{}

	void init() override
	{
		indices.resize(8, INVALID_HANDLE);
	}

	size_t insert(size_t hEntity) override
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

	size_t remove(size_t hEntity) override
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

	size_t removeByIndex(size_t hComp) override
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


	inline bool exists(size_t hEntity) const override
	{
		if (hEntity >= indices.size())
			return false;

		return indices[hEntity] != INVALID_HANDLE;
	}
	
	inline size_t get(size_t hEntity) const override
	{
		if (hEntity >= indices.size())
			return INVALID_HANDLE;

		return indices[hEntity];
	}

	inline void clear() override
	{
		indices.clear();
		compStorage->clear();
	}

	inline size_t size() const override
	{
		return compStorage->size();
	}

protected:
	std::vector<size_t> indices;
	DComponentStorage* compStorage = nullptr;	
};



class DIndexTableHash : public DIndexTable
{
public:
	explicit DIndexTableHash(DComponentStorage* compStorage) :
		compStorage(compStorage)
	{
	}

	void init() override
	{

	}

	size_t insert(size_t hEntity) override
	{
		assert(!exists(hEntity));

		size_t index = compStorage->size();
		indices[hEntity] = index;

		return index;
	}

	size_t remove(size_t hEntity) override
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

	size_t removeByIndex(size_t hComp) override
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


	inline bool exists(size_t hEntity) const override
	{
		return indices.find(hEntity) != indices.cend();
	}

	inline size_t get(size_t hEntity) const override
	{
		phmap::flat_hash_map<uint32_t, uint32_t>::const_iterator it = indices.find(hEntity);
		if(it == indices.cend())
			return INVALID_HANDLE;

		return it->second;
	}

	inline void clear() override
	{
		indices.clear();
		compStorage->clear();
	}

	inline size_t size() const override
	{
		return compStorage->size();
	}

protected:
	phmap::flat_hash_map<uint32_t, uint32_t> indices;
	DComponentStorage* compStorage = nullptr;
};

WECS_END