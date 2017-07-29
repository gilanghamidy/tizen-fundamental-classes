/*
 * ObservableContainer.h
 *
 *  Created on: Jun 21, 2017
 *      Author: gmh
 */

#ifndef TFC_CONTAINERS_OBSERVABLECONTAINER_H_
#define TFC_CONTAINERS_OBSERVABLECONTAINER_H_

#include "TFC/Containers/ContainerBase.h"
#include <TFC/Core/Metaprogramming.h>
#include <list>
#include <set>

namespace TFC {
namespace Containers {

	template<typename TIterator, typename = void>
	class DereferenceWrapper
	{
	public:
		typedef typename std::remove_reference<decltype(*std::declval<TIterator>())>::type ValueType;

		void Reset(TIterator& ref) { ptr = &ref; refWrapper.reset(); }
		ObjectClass& Dereference()
		{
			if(!refWrapper)
			{
				refWrapper.reset(new TFC::ReferenceWrapper<ValueType> { *(*ptr) });
			}
			return *refWrapper;
		}
	private:
		TIterator* ptr { nullptr };
		std::unique_ptr<TFC::ReferenceWrapper<ValueType>> refWrapper;
	};

	template<typename TIterator>
	class DereferenceWrapper<
		TIterator,
		typename std::enable_if<
			std::is_pointer<typename TIterator::value_type>::value &&
			std::is_base_of<TFC::ObjectClass, std::remove_pointer_t<typename TIterator::value_type>>::value>::type>
	{
	public:
		void Reset(TIterator& ref) { ptr = &ref; }
		ObjectClass& Dereference() { return ***ptr; } // <-- The hell of stars: 1. deref the iterator ptr, 2. call * operator of iterator object to get the ptr, 3. deref the ptr to ObjectClass
	private:
		TIterator* ptr { nullptr };
	};

	template<typename TIterator>
	class DereferenceWrapper<TIterator, typename std::enable_if<std::is_base_of<TFC::ObjectClass, typename TIterator::value_type>::value>::type>
	{
	public:
		void Reset(TIterator& ref) { ptr = &ref; }
		ObjectClass& Dereference() { return *(*ptr); }
	private:
		TIterator* ptr { nullptr };
	};

	template<typename TContainer>
	struct MapSetIteratorExtractorHelper;

	template<typename T, typename... TArgs>
	struct MapSetIteratorExtractorHelper<std::set<T, TArgs...>>
	{
		typedef decltype(std::declval<std::set<T, TArgs...>>().insert(std::declval<T>())) InsertReturnType;
		static typename std::set<T, TArgs...>::iterator GetFromInsert(InsertReturnType const& ret)
		{
			return ret.first;
		}
	};

	template<typename T, typename... TArgs>
	struct MapSetIteratorExtractorHelper<std::multiset<T, TArgs...>>
	{
		typedef decltype(std::declval<std::set<T, TArgs...>>().insert(std::declval<T>())) InsertReturnType;
		static typename std::set<T, TArgs...>::iterator GetFromInsert(InsertReturnType const& ret)
		{
			return ret;
		}
	};

	struct ItemEventArgs
	{
		ContainerBase::Iterator item;
	};

	class ObservableContainerBase :
		public ContainerBase,
		public EventEmitterClass<ObservableContainerBase>
	{
	public:


		Event<ItemEventArgs&> eventItemInserted;
		Event<ItemEventArgs&> eventItemRemoved;

	protected:
		void RaiseEventItemInserted(std::unique_ptr<ContainerBase::Iterator::IteratorImpl> item)
		{
			ItemEventArgs args { std::move(item) };
			eventItemInserted(this, args);
		}
	};

	template<typename TContainer>
	class ObservableContainer : public ObservableContainerBase
	{
	public:
		typedef typename TContainer::value_type ValueType;
		typedef typename TContainer::iterator UnderlyingIterator;

		class IteratorImpl : public ContainerBase::Iterator::IteratorImpl
		{
		public:
			virtual ObjectClass& Dereference() override { return dereferenceWrapper.Dereference(); }

			virtual bool Equal(ContainerBase::Iterator::IteratorImpl const& other) const override
			{
				auto otherPtr = dynamic_cast<IteratorImpl const*>(&other);
				if(otherPtr)
					return iterator == otherPtr->iterator;
				return false;
			}

			virtual void Increment() override { iterator++; dereferenceWrapper.Reset(iterator); }
			virtual void Decrement() override { iterator--; dereferenceWrapper.Reset(iterator); }

			virtual std::unique_ptr<ContainerBase::Iterator::IteratorImpl> Clone() const override
			{
				return std::unique_ptr<ContainerBase::Iterator::IteratorImpl> { new IteratorImpl(this->iterator) };
			}

			virtual void const* GetStorageAddress() const override
			{
				return &*iterator;
			}

		private:
			IteratorImpl(UnderlyingIterator const& iter) : iterator(iter) { dereferenceWrapper.Reset(iterator); }
			UnderlyingIterator iterator;
			DereferenceWrapper<UnderlyingIterator> dereferenceWrapper;

			friend class ObservableContainer<TContainer>;
		};

		void Add(ValueType const& obj)
		{
			this->container.push_back(obj);
			std::unique_ptr<ContainerBase::Iterator::IteratorImpl> ret { new IteratorImpl(--this->container.end()) };
			RaiseEventItemInserted(std::move(ret));
		}

		void Add(ValueType&& obj)
		{
			this->container.push_back(std::move(obj));
			std::unique_ptr<ContainerBase::Iterator::IteratorImpl> ret { new IteratorImpl(--this->container.end()) };
			RaiseEventItemInserted(std::move(ret));
		}

		template<typename... TArgs>
		void Emplace(TArgs&&... args)
		{
			this->container.emplace(std::forward<TArgs>(args)...);
		}

		void AddFirst(ValueType const& obj)
		{
			this->container.push_front(obj);
			std::unique_ptr<ContainerBase::Iterator::IteratorImpl> ret { new IteratorImpl(this->container.begin()) };
			RaiseEventItemInserted(std::move(ret));
		}

		void AddFirst(ValueType&& obj)
		{
			this->container.push_front(std::move(obj));
			std::unique_ptr<ContainerBase::Iterator::IteratorImpl> ret { new IteratorImpl(this->container.begin()) };
			RaiseEventItemInserted(std::move(ret));
		}
		template<typename... TArgs>
		void EmplaceFirst(TArgs&&... args)
		{
			this->container.emplace_front(std::forward<TArgs>(args)...);
			std::unique_ptr<ContainerBase::Iterator::IteratorImpl> ret { new IteratorImpl(this->container.begin()) };
			RaiseEventItemInserted(std::move(ret));
		}

		void AddAfter(Iterator const& where, ValueType const& obj);
		void AddAfter(Iterator const& where, ValueType&& obj);
		template<typename... TArgs>
		void EmplaceAfter(Iterator const& where, TArgs&&... args);

		void Insert(ValueType const& obj)
		{
			auto iter = MapSetIteratorExtractorHelper<TContainer>::GetFromInsert(this->container.insert(obj));
			std::unique_ptr<ContainerBase::Iterator::IteratorImpl> ret { new IteratorImpl(iter) };
			RaiseEventItemInserted(std::move(ret));
		}

		void Insert(ValueType&& obj)
		{
			auto iter = MapSetIteratorExtractorHelper<TContainer>::GetFromInsert(this->container.insert(std::move(obj)));
			std::unique_ptr<ContainerBase::Iterator::IteratorImpl> ret { new IteratorImpl(iter) };
			RaiseEventItemInserted(std::move(ret));
		}

		typedef ValueType& Reference;
		Reference GetFirst();
		Reference GetLast();

		std::unique_ptr<Iterator::IteratorImpl> GetIteratorImplBegin() override
		{
			return std::unique_ptr<ContainerBase::Iterator::IteratorImpl> { new IteratorImpl(this->container.begin()) };
		}
		std::unique_ptr<Iterator::IteratorImpl> GetIteratorImplEnd() override
		{
			return std::unique_ptr<ContainerBase::Iterator::IteratorImpl> { new IteratorImpl(this->container.end()) };
		}

		virtual bool Empty() override
		{
			return this->container.empty();
		}

		auto GetUnderlyingEndIterator()	{ return this->container.end();	}

		auto GetUnderlyingBeginIterator() { return this->container.begin(); }

	private:
		TContainer container;
	};



}}


#endif /* TFC_CONTAINERS_OBSERVABLECONTAINER_H_ */
