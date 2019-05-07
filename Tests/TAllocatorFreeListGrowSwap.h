#pragma once

#include "pch.h"
#include "WoodenAllocators/AllocatorPoolGrowSwap.h"

using namespace wal;

namespace
{
	TEST(AllocatorPoolGrowSwap, Init)
	{
		struct A
		{
			int a, b;
		};

		wal::AllocatorPoolGrowSwap al;
		al.init(sizeof(A), 10, sizeof(A));

		ASSERT_EQ(al.getSize(), 10 * sizeof(A));
	}

	TEST(AllocatorPoolGrowSwap, Add)
	{
		struct A
		{
			int a, b;
		};

		wal::AllocatorPoolGrowSwap al;
		al.init(sizeof(A), 10, sizeof(A));
		A* a0 = (A*)al.allocMem();
		A* a1 = al.get<A>(0);
		ASSERT_EQ(a0, a1);
		ASSERT_EQ(al.getNumUsedBlks(), 1);

		al.freeMem(a0);
		ASSERT_EQ(al.getNumUsedBlks(), 0);
	}
}

