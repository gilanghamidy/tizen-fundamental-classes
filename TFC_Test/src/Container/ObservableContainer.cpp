/*
 * ObservableContainer.cpp
 *
 *  Created on: Jun 27, 2017
 *      Author: gilang
 */

#include "TFC_Test.h"
#include "TFC/Containers/ObservableContainer.h"


#include <gtest/gtest.h>
#include <mutex>
#include <chrono>
#include <thread>

using namespace TFC::Containers;

class ObservableContainerTest : public testing::Test
{
	protected:
	// virtual void SetUp() will be called before each test is run.
	// You should define it if you need to initialize the variables.
	// Otherwise, you don't have to provide it.
	virtual void SetUp()
	{

	}
	// virtual void TearDown() will be called after each test is run.
	// You should define it if there is cleanup work to do.
	// Otherwise, you don't have to provide it.
	virtual void TearDown()
	{

	}
};

TEST_F(ObservableContainerTest, BasicInstantiationList)
{
	ObservableContainer<std::list<std::string>> stringList;
	stringList.AddFirst("Satu");
	stringList.AddFirst("Dua");
	stringList.AddFirst("Tiga");

	int ctr = 0;


	for(auto& ref : stringList)
	{
		std::string& str = wrapper_cast<std::string&>(ref);

		if(ctr == 0) ASSERT_STREQ("Tiga", str.c_str());
		else if(ctr == 1) ASSERT_STREQ("Dua", str.c_str());
		else if(ctr == 2) ASSERT_STREQ("Satu", str.c_str());
		ctr++;
	}
}


TEST_F(ObservableContainerTest, BasicInstantiationSet)
{
	ObservableContainer<std::set<int>> intList;

	intList.Insert(4);
	intList.Insert(5);
	intList.Insert(1);
	intList.Insert(3);
	intList.Insert(2);

	int ctr = 1;

	for(auto& ref : intList)
	{
		int const& val = wrapper_cast<int const&>(ref);
		ASSERT_EQ(ctr, val) << "Value in iterator invalid";
		ctr++;
	}
}

namespace {

	class TestObject
	{
	public:
		int GetIntValue()
		{
			std::cout << "Accessor called\n";
			return mIntValue;
		}

		void SetIntValue(int val)
		{
			std::cout << "Mutator called\n";
			mIntValue = val;
		}

		void SetIntValue(int const* val)
		{
			std::cout << "Mutator overload called\n";
			mIntValue = *val;
		}

#if __has_declspec_attribute(property)
		__declspec(property(get = GetIntValue, put = SetIntValue))
		int IntValue;
#endif

	private:
		int mIntValue;
	};

	class ObserverClassList
	{
	public:
		ObservableContainer<std::list<std::string>> list;

		ObserverClassList()
		{
			list.eventItemInserted += EventHandler(ObserverClassList::OnItemAdd);
		}

	private:
		void OnItemAdd(ObservableContainerBase* source, ItemEventArgs& args)
		{
			decltype(auto) res = wrapper_cast<std::string&>(*args.item);
			std::cout << res << '\n';
		}
	};

	class ObserverClassSet
	{
	public:
		ObservableContainer<std::set<int>> list;

		ObserverClassSet()
		{
			list.eventItemInserted += EventHandler(ObserverClassSet::OnItemAdd);
		}

	private:
		void OnItemAdd(ObservableContainerBase* source, ItemEventArgs& args)
		{
			decltype(auto) res = wrapper_cast<int const&>(*args.item);

			std::cout << "This: " << res << '\n';
			auto nextIter = args.item;
			++nextIter;
			if(nextIter != list.end())
			{
				decltype(auto) nextInt = wrapper_cast<int const&>(*nextIter);
				std::cout << "After that is: " << nextInt << '\n';
			}
			else
			{
				std::cout << "Is on the end of the list.\n";
			}

			if(args.item != list.begin())
			{
				auto prevIter = args.item;
				--prevIter;
				decltype(auto) prevInt = wrapper_cast<int const&>(*prevIter);
				std::cout << "Before that is: " << prevInt << '\n';
			}
			else
			{
				std::cout << "Is on the front of the list.\n";
			}
			std::cout << '\n';
		}
	};

	class TestObjectClass : public TFC::ObjectClass
	{
	public:
		int var1;
		std::string var2;

		TestObjectClass(int val, char const* val2) : var1(val), var2(std::move(val2)) {}
	};
}

TEST_F(ObservableContainerTest, BasicInstantiationObjectClass)
{
	ObservableContainer<std::list<TestObjectClass>> standardContainer;

	standardContainer.Emplace(standardContainer.GetUnderlyingEndIterator(), 10, "Strangest");
	standardContainer.Emplace(standardContainer.GetUnderlyingEndIterator(), 20, "Feeling");
	standardContainer.Emplace(standardContainer.GetUnderlyingEndIterator(), 30, "Derilium");

	ObservableContainer<std::list<TestObjectClass*>> pointerContainer;

	pointerContainer.Add(new TestObjectClass(40, "Strangest"));
	pointerContainer.Add(new TestObjectClass(50, "Feeling"));
	pointerContainer.Add(new TestObjectClass(60, "Derilium"));



	int ctr = 1;
	for(auto& obj : standardContainer)
	{
		auto casted = static_cast<TestObjectClass*>(&obj);
		ASSERT_TRUE(ctr++ * 10 == casted->var1) << "Object in standard container is invalid";
	}

	for(auto& obj : pointerContainer)
	{
		auto casted = static_cast<TestObjectClass*>(&obj);
		ASSERT_TRUE(ctr++ * 10 == casted->var1) << "Object in pointer container is invalid";
	}
}

TEST_F(ObservableContainerTest, List_OnItemAdd)
{
	ObserverClassList oc;
	oc.list.AddFirst("Satu");
	oc.list.AddFirst("Dua");
	oc.list.AddFirst("Tiga");

	int a = 123;
	TestObject val;
	std::cout << "sizeof(TestObject) = " << sizeof(TestObject) << '\n';
	val.IntValue = 100;
	std::cout << "IntValue now = " << val.IntValue << '\n';
	val.IntValue++;
	std::cout << "IntValue now = " << val.IntValue << '\n';
	val.IntValue = &a;
	std::cout << "IntValue now = " << val.IntValue << '\n';
}

TEST_F(ObservableContainerTest, Set_OnItemAdd)
{
	ObserverClassSet oc;
	oc.list.Insert(4);
	oc.list.Insert(5);
	oc.list.Insert(1);
	oc.list.Insert(3);
	oc.list.Insert(2);
}
