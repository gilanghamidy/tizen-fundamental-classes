/*
 * ContainerBase.h
 *
 *  Created on: Jun 21, 2017
 *      Author: gmh
 */

#ifndef TFC_CONTAINERS_CONTAINERBASE_H_
#define TFC_CONTAINERS_CONTAINERBASE_H_

#include "TFC/Core.h"

namespace TFC {
namespace Containers {

	class ContainerBase
	{
	public:
		class Iterator
		{
		public:
			class IteratorImpl
			{
			public:
				virtual ~IteratorImpl() { }
			protected:
				virtual ObjectClass& Dereference() = 0;
				ObjectClass const& Dereference() const { return const_cast<IteratorImpl*>(this)->Dereference(); };
				virtual bool Equal(IteratorImpl const& other) const = 0;
				virtual void Increment() = 0;
				virtual void Decrement() = 0;
				virtual std::unique_ptr<IteratorImpl> Clone() const = 0;

				friend class Iterator;
			};

			ObjectClass& operator*() const { return impl->Dereference(); };
			Iterator& operator++() { impl->Increment(); return *this; }
			Iterator& operator--() { impl->Decrement(); return *this; }

			bool operator!=(Iterator const& other) { return !this->impl->Equal(*other.impl); }

			Iterator(Iterator const& other) : impl(other.impl->Clone()) { }
			Iterator(Iterator&& other) : impl(std::move(other.impl)) { }


			Iterator& operator=(Iterator const& other) { this->impl.reset(other.impl->Clone().release()); return *this; }
			Iterator& operator=(Iterator&& other) { this->impl.reset(other.impl.release()); return *this; }


			Iterator(std::unique_ptr<IteratorImpl>&& iteratorImpl) : impl(std::move(iteratorImpl)) { }

		private:
			std::unique_ptr<IteratorImpl> impl;

			friend class ContainerBase;
		};

		virtual ~ContainerBase() { }

		Iterator begin() { return { GetIteratorImplBegin() }; }
		Iterator end() { return { GetIteratorImplEnd() }; }
		virtual bool Empty() = 0;

	protected:
		virtual std::unique_ptr<Iterator::IteratorImpl> GetIteratorImplBegin() = 0;
		virtual std::unique_ptr<Iterator::IteratorImpl> GetIteratorImplEnd() = 0;

	private:
	};

}}



#endif /* TFC_CONTAINERS_CONTAINERBASE_H_ */
