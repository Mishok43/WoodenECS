#pragma once

#include "pch.h"
#include "WoodenECS/WECS.h"
#include "WoodenECS/Job.h"
#include <vector>

using namespace wecs;

	//class TestSystem
	//{
	//public:
	//	using cmp_type_list = typename wecs::type_list<TestComponent1, TestComponent2>;

	//	TestSystem(bool* check)
	//		:check(check)
	//	{}

	//	void update(TestComponent1& c1, TestComponent2& c2, float dTime)
	//	{
	//		c2.x = c1.x;
	//		c2.y = c1.y;

	//		*check &= c1.flag;
	//	}

	//	bool* check;
	//};
namespace
{
	struct TestComponent3
	{
		float x, y;
		float xSpeed, ySpeed;

		DECL_MANAGED_DENSE_COMP_DATA(TestComponent3, 8)
	}; DECL_OUT_COMP_DATA(TestComponent3)

	struct TestComponent1
	{
		TestComponent1()
		{}


		TestComponent1(int _x, int _y, bool _old = false) :
			x(_x), y(_y), flag(_old)
		{}

		int x;
		int y;
		bool flag;

		DECL_MANAGED_DENSE_COMP_DATA(TestComponent1, 16)
	}; DECL_OUT_COMP_DATA(TestComponent1)



	/*TEST(ECS, SystemUpdateCMP)
	{
		WECS ecs;
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
	} */

	TEST(ECS, AddingEdditingComponents)
	{
		WECS ecs;

		for (size_t i = 0; i < 32; ++i)
		{
			HEntity hEntity = ecs.createEntity();
			ecs.addComponent<TestComponent1>(hEntity, 10*i, 12*i);
		}

		TestComponent1& a = ecs.getComponent<TestComponent1>(HEntity(3));
		ASSERT_EQ(a.x, 30);
		ASSERT_EQ(a.y, 36);

		a.x = 35;
		TestComponent1& a2 = ecs.getComponent<TestComponent1>(HEntity(3));
		ASSERT_EQ(a2.x, 35);

		ecs.for_each([](TestComponent1& t)
		{
			t.x = 0;
		}, type_list<TestComponent1>());

		for (size_t i = 0; i < 32; ++i)
		{
			TestComponent1& c = ecs.getComponent<TestComponent1>(HEntity(i));
			ASSERT_EQ(c.x, 0);
		}

		ecs.clearComponents<TestComponent1>();
	}

	TEST(ECS, RemoveComponents)
	{
		WECS ecs;

		for (size_t i = 0; i < 32; ++i)
		{
			ecs.addComponent<TestComponent1>(ecs.createEntity(), 10 * i, 12 * i);
		}

		TestComponent1 a31 = ecs.getComponent<TestComponent1>(HEntity(31));
		TestComponent1& a3 = ecs.getComponent<TestComponent1>(HEntity(3));
		ecs.removeComponent<TestComponent1>(3);
		
		ASSERT_EQ(a31.x, a3.x);
		ASSERT_EQ(a31.y, a3.y);
		ecs.clearComponents<TestComponent1>();
	}

	//TEST(ECS, RemoveComponentsIf)
	//{
	//	WECS ecs;

	//	for (size_t i = 0; i < 32; ++i)
	//	{
	//		ecs.addComponent<TestComponent1>(ecs.createEntity(), 10 * i, 12 * i, (bool)(i % 2 == 0));
	//	}
	//	
	//	ecs.removeComponentsIf<TestComponent1>([](TestComponent1& c)
	//	{
	//		return c.flag;
	//	});


	//	ASSERT_EQ(ecs.getNumbrComponents<TestComponent1>(), 32/2);

	//	bool check = true;
	//	ecs.for_each([&check](TestComponent1& c)
	//	{
	//		check &= c.flag == false;
	//	}, type_list<TestComponent1>());

	//	ASSERT_TRUE(check);
	//}

	//TEST(ECS, ForEach)
	//{
	//	WECS ecs;
	//	
	//	for (size_t i = 0; i < 32; ++i)
	//	{
	//		bool flag = (bool)(i % 2 == 0);
	//		ecs.addComponent<TestComponent1>(i, i, 12 * i, flag);
	//		if (flag)
	//		{
	//			ecs.addComponent<TestComponent2>(i);
	//		}
	//	}

	//	int num = 0;
	//	bool check = true;
	//	ecs.for_each([&num, &check](TestComponent1& c1, TestComponent2& c2)
	//	{
	//		++num;
	//		check &= (c1.x % 2) == 0;
	//	}, type_list<TestComponent1, TestComponent2>());

	//	ASSERT_EQ(num, 16);
	//	ASSERT_TRUE(check);
	//}

	TEST(ECS, HandleManager)
	{
		WECS ecs;
		
		HEntity hEntity = ecs.createEntity();
		ecs.addComponent<TestComponent1>(hEntity);

		ecs.removeEntity<TestComponent1>(hEntity);

		HEntity hEntity2 = ecs.createEntity();
		ASSERT_EQ(hEntity, hEntity2);
		ecs.clearComponents<TestComponent1>();
	}



	TEST(ECS, JobSystem)
	{
		WECS ecs;
		
		class JobTest : public JobParallaziblePerCompGroup<TestComponent3>
		{
			void update(WECS* ecs, HEntity hEntity, TestComponent3& c) final
			{
				c.x += c.xSpeed;
				c.y += c.ySpeed;
			}
		};

		HEntity h = ecs.createEntity();
		TestComponent3 comp;
		comp.x = 1.0f; comp.y = 2.5f; comp.xSpeed = 0.5f; comp.ySpeed = 0.7f;
		ecs.addComponent<TestComponent3>(h, comp);

		JobTest jobTest;
		jobTest.run(&ecs);


		TestComponent3& compUpdated = ecs.getComponent<TestComponent3>(h);

		comp.x += comp.xSpeed;
		comp.y += comp.ySpeed;
		ASSERT_EQ(comp.x, compUpdated.x);
		ASSERT_EQ(comp.y, compUpdated.y);
	}

	/*enum class Message : uint16_t
	{
		ACTION0,
		ACTION1
	};

	struct MessageAction0
	{
		static const uint16_t id = uint16_t(Message::ACTION0);
	};

	struct MessageAction1
	{
		static const uint16_t id = uint16_t(Message::ACTION1);
	};

	TEST(ECS, Message)
	{

		class TestSystem2
		{
			using cmp_type_list = typename type_list<TestComponent1>;


			void on(MessageAction0& message)
			{

			}

			void on(MessageAction1& message)
			{

			}
		};


		WECS ecs;
		ecs.registerComponent<TestComponent1>();
		ecs.registerSystem<TestSystem2>();


		ecs.dispa
	}*/


}

