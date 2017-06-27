/*
 * SynchronizedObject.h
 *
 *  Created on: Jun 21, 2017
 *      Author: gmh
 */

#ifndef TFC_INFRASTRUCTURES_SYNCHRONIZEDOBJECT_H_
#define TFC_INFRASTRUCTURES_SYNCHRONIZEDOBJECT_H_

#include "TFC/Core.h"
#include <mutex>

namespace TFC {
namespace Infrastructures {


	template<typename T>
	class SynchronizedObject
	{
	public:
		class Access
		{
		public:
			~Access() { };
			T& operator*() { return ref.object; }
			T* operator->() { return &ref.object; }

			Access(Access const&) = delete;
			Access& operator=(Access const&) = delete;
			Access& operator=(Access&& other) = delete;

			Access(Access&& other) :
				ref(other.ref),
				lock(std::move(other.lock))
			{ }

		private:
			Access(SynchronizedObject<T>& ref) :
				ref(ref),
				lock(ref.accessMutex)
			{ }

			SynchronizedObject& ref;
			std::unique_lock<std::mutex> lock;
			friend class SynchronizedObject<T>;
		};

		Access GetAccess() { return { *this }; }

	private:
		T object;
		std::mutex accessMutex;
		friend class Access;
	};

}}



#endif /* TFC_INFRASTRUCTURES_SYNCHRONIZEDOBJECT_H_ */
