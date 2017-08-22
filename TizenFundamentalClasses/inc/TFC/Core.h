/*
 * Tizen Fundamental Classes - TFC
 * Copyright (c) 2016-2017 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *    Core.h
 *
 * Core infrastructure for Tizen Fundamental Classes, consisting of base classes
 * for virtual classes, event objects, exceptions, and property objects
 *
 * Created on: 	 Feb 16, 2016
 * Author: 		 Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */
#pragma once

#ifndef TFC_CORE_H_
#define TFC_CORE_H_

#ifdef LIBBUILD
#define LIBAPI __attribute__((__visibility__("default")))
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TFC-Debug"
#else
#define LIBAPI
#endif


#if defined(__CDT_PARSER__) || __has_declspec_attribute(property)
#	define TFC_HAS_PROPERTY
#endif



#include <memory>
#include <exception>
#include <string>
#include <type_traits>

// Forward declaration of TFC Core Language Features
namespace TFC {

/**
 * EventClass is an attribute class marking that the class inheriting this class may have
 * a member function as an event handler. Member function with correct signature may be
 * registered as event handler using event handler registration syntax as follow:
 */
class EventClass;

/**
 * EventClass is an attribute class marking that the class inheriting this class may have
 * polymorphic behavior which can be downcasted during the runtime. This class has virtual
 * destructor which enables the polymorphic and RTTI feature on this class and subclass.
 */
class LIBAPI ObjectClass
{
public:
	virtual ~ObjectClass();
};

/**
 * PropertyClass is an attribute class marking that the class can implement Property object
 * which is an accessor and mutator that can behaves like a normal field. PropertyClass should be
 * inherited as private inheritance because it acts as mixin which introduces Property name inside
 * the class scope.
 * ```
 *    class ClassA : TFC::PropertyClass<ClassA>
 *    {
 *    public:
 *        int valA;
 *
 *        int GetA() const
 *        {
 *            return valA;
 *        }
 *
 *        void SetA(int const& val)
 *        {
 *            this->valA = val;
 *        }
 *
 *        Property<int>::Get<&ClassA::GetA>::Set<&ClassA::SetA> A;
 *     };
 * ```
 * When using PropertyClass attribute alongside, with a base class that already declare
 * PropertyClass attribute, Property name must be reintroduced in its subclass scope by calling
 * `using TFC::PropertyClass<CLASS_NAME>::Property`.
 *
 * ```
 *     class ClassB : public ClassA, TFC::PropertyClass<ClassB>
 *     {
 *         using TFC::PropertyClass<ClassB>::Property;
 *     public:
 *         int valB;
 *         int GetB() const
 *         {
 *             return valB;
 *         }
 *
 *         void SetB(int const& val)
 *         {
 *             this->valB = val;
 *         }
 *
 *         Property<int>::Get<&ClassB::GetB>::Set<&ClassB::SetB> B;
 *     };
 * ```
 *
 * @tparam TClass The class itself
 */
template<typename TClass>
class PropertyClass;

class ObjectDeletedException;

class ManagedClass
{
public:
	class SharedHandle;

	class SafePointer
	{
	public:
		bool TryAccess() const;
		SafePointer();
		~SafePointer();
		SafePointer(SafePointer&& that);
		SafePointer(SafePointer const& that);
		operator bool() const { return TryAccess(); }
		bool Empty() const { return !handle; }
		void ThrowIfUnsafe() const;

		SafePointer& operator=(SafePointer const& that);
		SafePointer& operator=(SafePointer&& that);
	private:
		friend class ManagedClass;
		SafePointer(SharedHandle* handle);


		SharedHandle* handle;
	};

	template<typename T>
	static SafePointer GetSafePointerFrom(T* what);

	template<typename T, bool = std::is_convertible<T*, ManagedClass*>::value>
	struct SafePointerGetter;

	ManagedClass();
	~ManagedClass();
	SafePointer GetSafePointer();

private:
	SharedHandle* handle;

};

template<typename T, bool>
struct ManagedClass::SafePointerGetter
{
	static SafePointer GetSafePointer(T* what)
	{
		return what->GetSafePointer();
	}
};

template<typename T>
struct ManagedClass::SafePointerGetter<T, false>
{
	static SafePointer GetSafePointer(T* what)
	{
		return {};
	}
};

/**
 * EventEmitter is an attribute class marking that the class inheriting this class may has
 * Event object declaration with this object as the source.
 */
template<typename TClass>
class EventEmitterClass;

class LIBAPI TFCException : public std::exception
{
public:
	explicit TFCException() { BuildStackTrace(); }
	explicit TFCException(char const* message);
	explicit TFCException(std::string&& message);
	explicit TFCException(std::string const& message);
	virtual char const* what() const throw () final;
	virtual ~TFCException();

	std::string const& GetMessage() const { return msg; }
protected:
	void SetMessage(std::string const& what) { msg = what; }

private:
	std::string msg;
	int symbolCount { 0 };

	std::shared_ptr<char*> symbols;
	//char** symbols { nullptr };

	void BuildStackTrace();

public:
	std::string GetStackTrace() const;
};

struct Color
{
	uint8_t r, g, b, a;
	static inline Color FromRGBA(uint32_t val) { return *(reinterpret_cast<Color*>(&val)); }
};

template<typename T>
inline bool IsNull(T* ptr)
{
	return ptr == nullptr ? true : false;
}

template<typename T>
inline T* Coalesce(T* ptr, T* valueIfNull)
{
	return IsNull(ptr) ? ptr : valueIfNull;
}

template<typename TExcept, typename T>
inline T* ThrowIfNull(T* ptr)
{
	if(ptr == nullptr)
		throw TExcept{};

	return ptr;
}

template<typename TExcept = TFC::TFCException>
inline void TFCAssert(bool value, char const* message = "")
{
	if(!value)
		throw TExcept(message);
}

template<typename TExcept = TFC::TFCException>
inline void TFCAssertZero(int value, char const* message = "")
{
	if(value != 0)
	{
		std::string msg(message);
		msg += " (Assertion failed: ";
		msg += std::to_string(value);
		msg += ")";
		throw TExcept(std::move(msg));
	}
}

long long GetCurrentTimeMillis();

class ReferenceWrapperBase : public ObjectClass
{
public:
	virtual void const* GetAddress() const = 0;
	virtual ~ReferenceWrapperBase() { }
	virtual std::unique_ptr<ReferenceWrapperBase> Clone() = 0;
};

template<typename T>
class ReferenceWrapper : public ReferenceWrapperBase
{
public:
	ReferenceWrapper(T& ref) : ref(ref) { }
	T& Get() { return ref; }
	virtual ~ReferenceWrapper() { }
	virtual std::unique_ptr<ReferenceWrapperBase> Clone() override
	{
		return std::unique_ptr<ReferenceWrapperBase> { new ReferenceWrapper<T> { ref } };
	}

	T* operator&()
	{
		return &ref;
	}

	virtual void const* GetAddress() const override
	{
		return &ref;
	}
private:
	T& ref;
};

class ObjectReferenceWrapper : public ReferenceWrapperBase
{
public:
	ObjectReferenceWrapper(ObjectClass& ref) : ref(ref) { }

	template<typename T>
	T& Get() { return dynamic_cast<T&>(ref); }

	virtual ~ObjectReferenceWrapper() { }
	virtual std::unique_ptr<ReferenceWrapperBase> Clone() override
	{
		return std::unique_ptr<ReferenceWrapperBase> { new ObjectReferenceWrapper { ref } };
	}

	virtual void const* GetAddress() const override
	{
		return &ref;
	}

	ObjectClass* operator&()
	{
		return &ref;
	}

private:
	ObjectClass& ref;
};




namespace Core {

/**
 * EventObject class is the core infrastructure implementing event handling mechanism within
 * the Tizen Fundamental Classes library. It implements delegation internally which can direct
 * event handling call to the registered event handler.
 *
 * It implements += operator to register handler to the event, and -= operator to unregister.
 */
template<typename = void*, typename = void*>
class EventObject;

/**
 * SharedEventObject class is safe wrapper for EventObject class which lives outside of the
 * instance of the object declares it. It uses std::shared_ptr internally to manages the
 * instance by reference counting, thus making this EventObject is safe to be transferred
 * across stack-frame and thread boundary.
 *
 * SharedEventObject is mostly used to implement asynchronous mechanism safely which the thread
 * might not be finished when the initiator is destroyed. By using basic EventObject, it will
 * create a possible fault because EventObject lifetime is tied to the object declares it.
 */
template<typename = void*, typename = void*>
class SharedEventObject;

class PropertyObjectBase;

template<typename TDefining, typename TValue>
struct PropertyObject;

template<typename TDefining, typename TValue>
struct PropertyGetterFunction;

template<typename TDefining, typename TValue>
struct PropertySetterFunction;

template<typename T>
struct ObjectClassPackage : public ObjectClass
{
	T value;

	template<typename TArgs>
	ObjectClassPackage(TArgs param) : value(std::forward<TArgs>(param))
	{

	}
};

template<typename T>
ObjectClassPackage<T>* PackInObjectClass(T value)
{
	return new ObjectClassPackage<typename std::remove_reference<T>::type> { std::forward<T>(value) };
}

template<typename T>
T UnpackFromObjectClass(ObjectClass* obj)
{
	auto ptrRaw = dynamic_cast<ObjectClassPackage<T>*>(obj);
	if(ptrRaw == nullptr)
		throw TFCException("Invalid package retrieved in UnpackFromObjectClass");
	std::unique_ptr<ObjectClassPackage<T>> ptr(ptrRaw);

	return ptrRaw->value;
}

template<typename T, typename = void>
struct WrapperCastHelper
{
	static T& Cast(ObjectClass& ref)
	{
		auto tmpPtr = dynamic_cast<TFC::ReferenceWrapper<typename std::remove_reference<T>::type>*>(&ref);
		if(tmpPtr)
		{
			return tmpPtr->Get();
		}
		else
		{
			return CastObjectClass(ref);
		}
	}

	static T& CastObjectClass(ObjectClass& ref)
	{
		auto ptr = dynamic_cast<typename std::remove_reference<T>::type*>(&ref);

		if(ptr)
			return *ptr;
		else
		{
			std::string msg { "Expected type: " };
			msg.append(typeid(T).name());
			msg.append(", Provided: ");
			msg.append(typeid(ref).name());
			throw TFC::TFCException(std::move(msg));
		}
	}
};

}}

#define TFC_ExceptionDeclare(NAME, PARENT)\
class NAME : public PARENT\
{\
public:\
	explicit inline NAME() : PARENT() { }\
	explicit inline NAME(char const* message) : PARENT(message) { }\
	explicit inline NAME(std::string&& message) : PARENT(message) { }\
	explicit inline NAME(std::string const& message) : PARENT(message) { }\
}

#define TFC_ExceptionDeclareWithMessage(NAME, PARENT, DEFAULT_MESSAGE)\
class NAME : public PARENT\
{\
public:\
	explicit inline NAME() : PARENT(DEFAULT_MESSAGE) { }\
	explicit inline NAME(char const* message) : PARENT(message) { }\
	explicit inline NAME(std::string&& message) : PARENT(message) { }\
	explicit inline NAME(std::string const& message) : PARENT(message) { }\
}


#include "TFC/Core/Event.inc.h"
#include "TFC/Core/Property.inc.h"

namespace TFC {

TFC_ExceptionDeclare	(RuntimeException, TFCException);
TFC_ExceptionDeclare	(ArgumentException, TFCException);
TFC_ExceptionDeclare	(ObjectDeletedException, TFCException);

class LIBAPI ObservableObjectClass :
		public ObjectClass,
		EventEmitterClass<ObservableObjectClass>
{
public:
	Event<void*> eventObjectUpdated;
	Event<std::string const&> eventFieldUpdated;

	LIBAPI ObservableObjectClass() = default;
	LIBAPI ObservableObjectClass(ObservableObjectClass const& obj)
	{

	}

protected:
	void OnObjectUpdated();
	void OnFieldUpdated(std::string const& fieldName);
	virtual ~ObservableObjectClass();
};

namespace Core {

TFC_ExceptionDeclare	(InvocationException, RuntimeException);

}}



template<typename T>
TFC::ManagedClass::SafePointer TFC::ManagedClass::GetSafePointerFrom(T* what)
{
	return TFC::ManagedClass::SafePointerGetter<T>::GetSafePointer(what);
}

template<typename T>
T wrapper_cast(TFC::ObjectClass& obj)
{
	return TFC::Core::WrapperCastHelper<T>::Cast(obj);
}

#endif /* CORE_NEW_H_ */
