#pragma once

#include "pch.h"
#include "WoodenECS/WECS.h"
#include <vector>

using namespace wecs;
	struct TestComponent1
	{
		TestComponent1()
		{}


		TestComponent1(int _x, int _y, bool _old=false):
			x(_x), y(_y), flag(_old)
		{ }

		int x;
		int y;
		bool flag;
	};

	struct TestComponent2
	{
		int x, y;
	};


	class TestSystem
	{
	public:
		using cmp_type_list = typename wecs::type_list<TestComponent1, TestComponent2>;

		TestSystem(bool* check)
			:check(check)
		{}

		void update(TestComponent1& c1, TestComponent2& c2, float dTime)
		{
			c2.x = c1.x;
			c2.y = c1.y;

			*check &= c1.flag;
		}

		bool* check;
	};
namespace
{

	TEST(ECS, SystemUpdateCMP)
	{


		WECS ecs;
		ecs.registerComponent<TestComponent1>();
		ecs.registerComponent<TestComponent2>();
		for (size_t i = 0; i < 32; ++i)
		{
			bool flag = (bool)(i % 2 == 0);
			ecs.addComponent<TestComponent1>(i, 10 * i, 12 * i, flag);
			if (flag)
			{
				ecs.addComponent<TestComponent2>(i);
			}
		}

		bool flag = true;
		ecs.registerSystem<TestSystem>(&flag);
		ecs.update(0.0f);

		ASSERT_TRUE(flag);

	}

	TEST(ECS, SystemCreate)
	{
		class TestSystem2
		{
		public:
			using cmp_type_list = typename wecs::type_list<TestComponent1, TestComponent2>;

			TestSystem2(int* sum) :
				sum(sum)
			{
			}

			void create(size_t hEntity, TestComponent1& c1)
			{
				*sum += c1.x;
			}

			int* sum;
		};

		WECS ecs;
		ecs.registerComponent<TestComponent1>();

		int sum = 0;
		ecs.registerSystem<TestSystem2>(&sum);

		int realSum = 0;
		for (size_t i = 0; i < 32; ++i)
		{
			bool flag = (bool)(i % 2 == 0);
			realSum += i;
			ecs.addComponent<TestComponent1>(i, i, 12 * i, flag);
		}
		ASSERT_EQ(sum, realSum);
	}

	TEST(ECS, SystemDestroy)
	{
		class TestSystem2
		{
		public:
			using cmp_type_list = typename wecs::type_list<TestComponent1>;

			TestSystem2(int* _sum) :
				sum(_sum)
			{
			}

			void destroy(TestComponent1& c1)
			{
				*sum += c1.x;
			}

			int* sum;
		};

		WECS ecs;
		ecs.registerComponent<TestComponent1>();

		int sum = 0;
		ecs.registerSystem<TestSystem2>(&sum);

		int realSum = 0;
		for (size_t i = 0; i < 32; ++i)
		{
			bool flag = (bool)(i % 2 == 0);
			realSum += i;
			ecs.addComponent<TestComponent1>(i, i, 12 * i, flag);
		}

		ASSERT_EQ(sum, 0);

		for (size_t i = 0; i < 32; ++i)
		{
			ecs.removeComponent<TestComponent1>(i);
		}

		ASSERT_EQ(sum, realSum);
	} 

	TEST(ECS, AddingEdditingComponents)
	{
		WECS ecs;
		ecs.registerComponent<TestComponent1>();	
		for (size_t i = 0; i < 32; ++i)
		{
			ecs.addComponent<TestComponent1>(i, 10*i, 12*i);
		}

		TestComponent1& a = ecs.getComponent<TestComponent1>(3);
		ASSERT_EQ(a.x, 30);
		ASSERT_EQ(a.y, 36);

		a.x = 35;
		TestComponent1& a2 = ecs.getComponent<TestComponent1>(3);
		ASSERT_EQ(a2.x, 35);

		ecs.for_each([](TestComponent1& t)
		{
			t.x = 0;
		}, type_list<TestComponent1>());

		for (size_t i = 0; i < 32; ++i)
		{
			TestComponent1& c = ecs.getComponent<TestComponent1>(i);
			ASSERT_EQ(c.x, 0);
		}
	}

	TEST(ECS, RemoveComponents)
	{
		WECS ecs;
		ecs.registerComponent<TestComponent1>();
		for (size_t i = 0; i < 32; ++i)
		{
			ecs.addComponent<TestComponent1>(i, 10 * i, 12 * i);
		}

		TestComponent1 a31 = ecs.getComponent<TestComponent1>(31);
		TestComponent1& a3 = ecs.getComponent<TestComponent1>(3);
		ecs.removeComponent<TestComponent1>(3);
		
		ASSERT_EQ(a31.x, a3.x);
		ASSERT_EQ(a31.y, a3.y);
	}

	TEST(ECS, RemoveComponentsIf)
	{
		WECS ecs;
		ecs.registerComponent<TestComponent1>();
		for (size_t i = 0; i < 32; ++i)
		{
			ecs.addComponent<TestComponent1>(i, 10 * i, 12 * i, (bool)(i % 2 == 0));
		}
		
		ecs.removeComponentsIf<TestComponent1>([](TestComponent1& c)
		{
			return c.flag;
		});


		ASSERT_EQ(ecs.getNumbrComponents<TestComponent1>(), 32/2);

		bool check = true;
		ecs.for_each([&check](TestComponent1& c)
		{
			check &= c.flag == false;
		}, type_list<TestComponent1>());

		ASSERT_TRUE(check);
	}

	TEST(ECS, ForEach)
	{
		WECS ecs;
		ecs.registerComponent<TestComponent1>();
		ecs.registerComponent<TestComponent2>();
		for (size_t i = 0; i < 32; ++i)
		{
			bool flag = (bool)(i % 2 == 0);
			ecs.addComponent<TestComponent1>(i, i, 12 * i, flag);
			if (flag)
			{
				ecs.addComponent<TestComponent2>(i);
			}
		}

		int num = 0;
		bool check = true;
		ecs.for_each([&num, &check](TestComponent1& c1, TestComponent2& c2)
		{
			++num;
			check &= (c1.x % 2) == 0;
		}, type_list<TestComponent1, TestComponent2>());

		ASSERT_EQ(num, 16);
		ASSERT_TRUE(check);
	}

	TEST(ECS, HandleManager)
	{
		WECS ecs;
		ecs.registerComponent<TestComponent1>();
		ecs.registerComponent<TestComponent2>();

		size_t hEntity = ecs.createEntity();
		ecs.addComponent<TestComponent1>(hEntity);
		ecs.addComponent<TestComponent2>(hEntity);


		ecs.removeEntity<TestComponent1, TestComponent2>(hEntity);

		size_t hEntity2 = ecs.createEntity();
		ASSERT_EQ(hEntity, hEntity2);
	}


	TEST(ECS, Init2)
	{
		WECS ecs;
		ecs.registerComponent<TestComponent1>();
	}
}

