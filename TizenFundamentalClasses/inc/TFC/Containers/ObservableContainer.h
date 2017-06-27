/*
 * ObservableContainer.h
 *
 *  Created on: Jun 21, 2017
 *      Author: gmh
 */

#ifndef TFC_CONTAINERS_OBSERVABLECONTAINER_H_
#define TFC_CONTAINERS_OBSERVABLECONTAINER_H_

#include "TFC/Containers/ContainerBase.h"
#include <list>
#include <set>

namespace TFC {
namespace Containers {

	template<typename TIterator, bool = std::is_base_of<TFC::ObjectClass, typename TIterator::value_type>::value>
	class DereferenceWrapper;

	template<typename TIterator>
	class DereferenceWrapper<TIterator, true>
	{
	public:
		void Reset(TIterator& ref) { ptr = &ref; }
		ObjectClass& Dereference() { return *(*ptr); }
	private:
		TIterator* ptr { nullptr };
	};

	template<typename TIterator>
	class DereferenceWrapper<TIterator, false>
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


	class ObservableContainerBase :
		public ContainerBase,
		public EventEmitterClass<ObservableContainerBase>
	{
	public:
		struct ItemEventArgs
		{
			ContainerBase::Iterator item;
		};

		Event<ItemEventArgs> eventItemInserted;
		Event<ItemEventArgs> eventItemRemoved;

	protected:
		void RaiseEventItemInserted(std::unique_ptr<ContainerBase::Iterator::IteratorImpl> item)
		{
			eventItemInserted(this, { { std::move(item) } });
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
			virtual ObjectClass& Dereference() { return dereferenceWrapper.Dereference(); }

			virtual bool Equal(ContainerBase::Iterator::IteratorImpl const& other) const
			{
				auto otherPtr = dynamic_cast<IteratorImpl const*>(&other);
				if(otherPtr)
					return iterator == otherPtr->iterator;
				return false;
			}

			virtual void Increment() { iterator++; dereferenceWrapper.Reset(iterator); }
			virtual void Decrement() { iterator--; dereferenceWrapper.Reset(iterator); }

			virtual std::unique_ptr<ContainerBase::Iterator::IteratorImpl> Clone() const
			{
				return std::unique_ptr<ContainerBase::Iterator::IteratorImpl> { new IteratorImpl(this->iterator) };
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

	private:
		TContainer container;
	};



}}


#endif /* TFC_CONTAINERS_OBSERVABLECONTAINER_H_ */
