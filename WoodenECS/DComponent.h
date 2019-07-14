#pragma once

#include "stdafx.h"
#include "WoodenAllocators/AllocatorPoolSwap.h"
#include "WoodenAllocators/AllocatorPoolFreeList.h"

WECS_BEGIN


template<typename T, typename ...Args>
inline void rawConstructObj(void* obj, Args&&... args)
{
	new (obj)T(std::forward<Args>(args)...);
}

template<typename T>
inline void rawDestroyObj(void* obj)
{
	static_cast<T*>(obj)->T::~T();
}

using FConstructor = void(*)(void*);
using FDestructor = void(*)(void*);

class DComponentStorage
{
public:
	DComponentStorage()
	{
	}

	template<typename T>
	void init()
	{
		destructor = rawDestroyObj<T>;

		uint16_t objSize = sizeof(T);
		compData.init(objSize, 0, objSize);
	}

	void clear()
	{
		for (size_t i = 0; i < compData.getNumUsedBlks(); ++i)
		{
			destructor((char*)compData.get(i));
		}

		compData.reset();
	}

	void reserve(uint32_t numObjects)
	{
		uint16_t objSize = compData.getBlkSize();
		compData.init(objSize, numObjects, objSize);

		ownerIndicesSize = numObjects;
		entities = (size_t*)wal::Allocator::alignedChunkAlloc(sizeof(size_t), ownerIndicesSize * sizeof(size_t));
	}

	inline size_t size() const
	{
		return compData.getNumUsedBlks();
	}

	inline size_t sizeRaw() const
	{
		return compData.getNumUsedBlks()*compData.getBlkSize();
	}

	inline size_t capacity() const
	{
		return compData.getNumBlks();
	}

	inline size_t capacityRaw() const
	{
		return compData.getNumBlks()*compData.getBlkSize();
	}

	template<typename T, typename ...Args>
	T& append(size_t hEntity, Args&&... args)
	{
		T* obj = (T*)compData.allocMem();
		assert(obj);

		rawConstructObj<T>(obj, std::forward<Args>(args)...);

		assert(entities);

		if (compData.getNumBlks() != ownerIndicesSize)
		{
			size_t* newOwnerIndices = (size_t*)wal::Allocator::alignedChunkAlloc(sizeof(size_t), compData.getNumBlks()*sizeof(size_t));
			std::memcpy(newOwnerIndices, entities, ownerIndicesSize * sizeof(size_t));
			wal::Allocator::alignedChunkFree(entities);

			entities = newOwnerIndices;
			ownerIndicesSize = compData.getNumBlks();
		}

		size_t iObj = compData.getPos(obj);
		entities[iObj] = hEntity;
		return *obj;
	}

	size_t remove(size_t hComp)
	{
		char* obj = (char*)compData.get(hComp);
		destructor(obj);

		size_t iSwapObj = compData.freeMem(obj);
		entities[hComp] = entities[iSwapObj];
		assert(entities[iSwapObj] = (size_t)-1);

		if (hComp == compData.getNumUsedBlks())
		{
			iSwapObj = INVALID_HANDLE;
		}

		return iSwapObj;
	}

	inline size_t getEntityHandle(uint32_t hComp) const
	{
		assert(entities);
		return entities[hComp];
	}

	inline void* dataRaw(){ return compData.data();}
	inline const void* dataRaw() const{ return compData.data();}

	inline void* getRaw(size_t hComp){ return compData.get(hComp);}
	inline const void* getRaw(size_t hComp) const{ return compData.get(hComp);}

	template<typename T>
	inline T* get(size_t hComp){ return compData.get<T>(hComp);}

	template<typename T>
	inline const T* get(size_t hComp) const{ return compData.get<T>(hComp);}

protected:
	wal::AllocatorPoolSwap compData;
	size_t* entities = nullptr;
	size_t ownerIndicesSize;

	FDestructor destructor;
};


class DComponentStorageSafe
{
public:
	DComponentStorageSafe()
	{
	}

	template<typename T>
	void init()
	{
		destructor = rawDestroyObj<T>;

		uint16_t objSize = sizeof(T);
		compData.init(objSize, 0, objSize);
	}

	void clear()
	{
		for (size_t i = 0; i < compData.getNumUsedBlks(); ++i)
		{
			destructor((char*)compData.get(i));
		}

		compData.reset();
	}

	void reserve(uint32_t numObjects)
	{
		uint16_t objSize = compData.getBlkSize();
		compData.init(objSize, numObjects, objSize);

		ownerIndicesSize = numObjects;
		entities = (size_t*)wal::Allocator::alignedChunkAlloc(sizeof(size_t), ownerIndicesSize * sizeof(size_t));
	}

	inline size_t size() const
	{
		return compData.getNumUsedBlks();
	}

	inline size_t sizeRaw() const
	{
		return compData.getNumUsedBlks()*compData.getBlkSize();
	}

	inline size_t capacity() const
	{
		return compData.getNumBlks();
	}

	inline size_t capacityRaw() const
	{
		return compData.getNumBlks()*compData.getBlkSize();
	}

	template<typename T, typename ...Args>
	T& append(size_t hEntity, Args&&... args)
	{
		T* obj = (T*)compData.allocMem();
		assert(obj);

		rawConstructObj<T>(obj, std::forward<Args>(args)...);

		assert(entities);

		if (compData.getNumBlks() != ownerIndicesSize)
		{
			size_t* newOwnerIndices = (size_t*)wal::Allocator::alignedChunkAlloc(sizeof(size_t), compData.getNumBlks() * sizeof(size_t));
			std::memcpy(newOwnerIndices, entities, ownerIndicesSize * sizeof(size_t));
			wal::Allocator::alignedChunkFree(entities);

			entities = newOwnerIndices;
			ownerIndicesSize = compData.getNumBlks();
		}

		size_t iObj = compData.getPos(obj);
		entities[iObj] = hEntity;
		return *obj;
	}

	size_t remove(size_t hComp)
	{
		char* obj = (char*)compData.get(hComp);
		destructor(obj);
		entities[hComp] = INVALID_HANDLE;

		return INVALID_HANDLE;
	}

	inline size_t getEntityHandle(uint32_t hComp) const
	{
		assert(entities);
		return entities[hComp];
	}

	inline void* dataRaw()
	{
		return compData.data();
	}
	inline const void* dataRaw() const
	{
		return compData.data();
	}

	inline void* getRaw(size_t hComp)
	{
		return compData.get(hComp);
	}
	inline const void* getRaw(size_t hComp) const
	{
		return compData.get(hComp);
	}

	template<typename T>
	inline T* get(size_t hComp)
	{
		return compData.get<T>(hComp);
	}

	template<typename T>
	inline const T* get(size_t hComp) const
	{
		return compData.get<T>(hComp);
	}

protected:
	wal::AllocatorPoolFreeList compData;
	size_t* entities = nullptr;
	size_t ownerIndicesSize;

	FDestructor destructor;
};


template<typename T, typename CompStorageT= DComponentStorage>
class HComponentStorage : public CompStorageT
{
public:
	using value_type = T;
	using iterator = T *;
	using const_iterator = const T*;
	using reference = T & ;
	using const_reference = const T&;

	HComponentStorage()
	{	
		init<T>();
	}

	inline iterator data(){ return (iterator)dataRaw(); }
	inline const_iterator data() const	{ return (const_iterator)dataRaw(); }
	
	inline reference operator[](int hComp){ return *(get<T>(hComp));}
	inline const_reference operator[](int hComp) const { return *(get<T>(hComp));}

	inline iterator begin() {return (iterator)dataRaw(); }
	inline const_iterator begin() const {return (const_iterator)dataRaw(); }

	inline iterator end() {return (iterator)dataRaw()+size(); }
	inline const_iterator end() const {return (const_iterator)dataRaw()+size(); }
};

template<typename T>
HComponentStorage<T>* storage_cast(DComponentStorage* storage)
{
	return (HComponentStorage<T>*)(storage);
}

template<typename T>
const HComponentStorage<T>* storage_cast(const DComponentStorage* storage)
{
	return (HComponentStorage<T>*)(storage);
}	


WECS_END
