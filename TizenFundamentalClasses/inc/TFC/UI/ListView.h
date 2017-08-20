/*
 * ListView.h
 *
 *  Created on: Jul 21, 2017
 *      Author: gilang
 */

#ifndef TFC_UI_LISTVIEW_H_
#define TFC_UI_LISTVIEW_H_

#include <TFC/Containers/ContainerBase.h>
#include <TFC/UI/ContainerWidgetBase.h>

#define TFC_LISTITEM_DATACONTEXT "__tfc_datacontext"
#define TFC_LISTITEM_DATACONTEXTTYPE "__tfc_datacontext_type"

namespace TFC {
namespace UI {

	class ItemTemplateBase
	{
	public:
		class ItemPayload
		{
		private:
			ItemTemplateBase& itemTemplate;
			void* item;
			ContainerWidgetBase* widget;
		public:
			ItemPayload(ItemTemplateBase& tmplt, void* itm, ContainerWidgetBase* widget) :
				itemTemplate(tmplt),
				item(itm),
				widget(widget) { }

			virtual operator Elm_Gen_Item_Class*()
			{
				return &itemTemplate.itemClass;
			}

			void* GetItem() const { return item; }

			ContainerWidgetBase* GetWidget() const { return widget; }

			friend class ItemTemplateBase;
		};

		virtual ItemPayload PackPayload(ObjectClass& item, ContainerWidgetBase* widget) const
		{
			return { *const_cast<ItemTemplateBase*>(this), &item, widget };
		}

		virtual bool CanProcess(ObjectClass& obj) const = 0;

		virtual ~ItemTemplateBase() { }

	protected:
		ItemTemplateBase(std::string const& className);

		virtual char* GetText(void* obj, Evas_Object* root, char const* part) = 0;
		virtual Evas_Object* GetContent(void* obj, Evas_Object* root, char const* part) = 0;

	private:
		Elm_Gen_Item_Class itemClass;
		std::string itemClassName;

		char* GetTextInternal(void* obj, Evas_Object* root, char const* part);
		Evas_Object* GetContentInternal(void* obj, Evas_Object* root, char const* part);

		static char* StaticGetText(void *data, Evas_Object *obj, const char *part);
		static Evas_Object* StaticGetContent(void *data, Evas_Object *obj, const char *part);
	};



	template<typename T>
	class ItemTemplate :
		public ItemTemplateBase
	{
	public:
		virtual ItemPayload PackPayload(ObjectClass& item, ContainerWidgetBase* widget) const override
		{
			return { *const_cast<ItemTemplate*>(this), const_cast<typename std::remove_const<T>::type*>(&wrapper_cast<T&>(item)), widget };
		}

		virtual bool CanProcess(ObjectClass& obj) const override
		{
			try
			{
				wrapper_cast<T&>(obj);
				return true;
			}
			catch(TFCException const& ex)
			{
				return false;
			}
		}

		T& GetDataContext(Evas_Object* obj)
		{
			while(obj != nullptr)
			{
				auto dataType = (char const*)evas_object_data_get(obj, TFC_LISTITEM_DATACONTEXTTYPE);
				if(dataType != nullptr)
				{
					if(strcmp(dataType, typeid(T).name()) == 0)
					{
						auto data = (T*)evas_object_data_get(obj, TFC_LISTITEM_DATACONTEXT);
						return *data;
					}
				}

				obj = elm_object_parent_widget_get(obj);
			}

			throw TFC::TFCException("Data context not available");
		}
	protected:
		typedef Evas_Object* (ItemTemplate::* ContentFunc)(T const& obj, Evas_Object* parent);
		typedef std::string (T::* StringGetterFunc)() const;
		typedef char const* (T::* CStringGetterFunc)() const;
		typedef std::string const& (T::* StringCRefGetterFunc)() const;
		typedef std::string (ItemTemplate::* StringFunc)(T const& obj);
		typedef char const* (ItemTemplate::* CStringFunc)(T const& obj);
		typedef std::string const& (ItemTemplate::* StringCRefFunc)(T const& obj);

		ItemTemplate(std::string const& className) : ItemTemplateBase(className) { }

		void RegisterPart(char const* partName, ContentFunc contentFunc) { funcTable.emplace(std::string { partName }, contentFunc); }
		void RegisterPart(char const* partName, StringGetterFunc stringFunc) { funcTable.emplace(std::string { partName }, stringFunc); }
		void RegisterPart(char const* partName, CStringGetterFunc stringFunc) { funcTable.emplace(std::string { partName }, stringFunc); }
		void RegisterPart(char const* partName, StringCRefGetterFunc stringFunc) { funcTable.emplace(std::string { partName }, stringFunc); }
		void RegisterPart(char const* partName, StringFunc stringFunc) { funcTable.emplace(std::string { partName }, stringFunc); }
		void RegisterPart(char const* partName, CStringFunc stringFunc) { funcTable.emplace(std::string { partName }, stringFunc); }
		void RegisterPart(char const* partName, StringCRefFunc stringFunc) { funcTable.emplace(std::string { partName }, stringFunc); }

		virtual char* GetText(void* obj, Evas_Object* root, char const* part) override
		{
			auto ptr = static_cast<T*>(obj);
			auto iter = funcTable.find(std::string { part });

			if(iter == funcTable.end() || iter->second.type == PartFunc::contentFunc)
				return nullptr;

			if(iter->second.type == PartFunc::stringGetterFunc)
			{
				auto funcPtr = iter->second.stringGetterFuncPtr;
				return strdup((ptr->*funcPtr)().c_str());
			}
			else if(iter->second.type == PartFunc::cstringGetterFunc)
			{
				auto funcPtr = iter->second.cstringGetterFuncPtr;
				return strdup((ptr->*funcPtr)());
			}
			else if(iter->second.type == PartFunc::stringCRefGetterFunc)
			{
				auto funcPtr = iter->second.stringCRefGetterFuncPtr;
				return strdup((ptr->*funcPtr)().c_str());
			}
			else if(iter->second.type == PartFunc::stringFunc)
			{
				auto funcPtr = iter->second.stringFuncPtr;
				return strdup((this->*funcPtr)(*ptr).c_str());
			}
			else if(iter->second.type == PartFunc::cstringFunc)
			{
				auto funcPtr = iter->second.cstringFuncPtr;
				return strdup((this->*funcPtr)(*ptr));
			}
			else if(iter->second.type == PartFunc::stringCRefFunc)
			{
				auto funcPtr = iter->second.stringCRefFuncPtr;
				return strdup((this->*funcPtr)(*ptr).c_str());
			}
			else
				return nullptr;
		}

		virtual Evas_Object* GetContent(void* obj, Evas_Object* root, char const* part) override
		{
			auto ptr = static_cast<T*>(obj);
			auto iter = funcTable.find(std::string { part });

			if(iter == funcTable.end() || iter->second.type != PartFunc::contentFunc)
				return nullptr;

			auto funcPtr = iter->second.contentFuncPtr;
			auto theContent = (this->*funcPtr)(*ptr, root);
			evas_object_data_set(theContent, TFC_LISTITEM_DATACONTEXT, obj);
			evas_object_data_set(theContent, TFC_LISTITEM_DATACONTEXTTYPE, typeid(T).name());
			return theContent;
		}

	private:
		struct PartFunc
		{
			enum {
				contentFunc,
				stringGetterFunc,
				cstringGetterFunc,
				stringCRefGetterFunc,
				stringFunc,
				cstringFunc,
				stringCRefFunc
			} type;

			union {
				ContentFunc contentFuncPtr;
				StringGetterFunc stringGetterFuncPtr;
				CStringGetterFunc cstringGetterFuncPtr;
				StringCRefGetterFunc stringCRefGetterFuncPtr;
				StringFunc stringFuncPtr;
				CStringFunc cstringFuncPtr;
				StringCRefFunc stringCRefFuncPtr;
			};

			PartFunc(ContentFunc func) : type(contentFunc), contentFuncPtr(func) { };
			PartFunc(StringGetterFunc func) : type(stringGetterFunc), stringGetterFuncPtr(func) { };
			PartFunc(CStringGetterFunc func) : type(cstringGetterFunc), cstringGetterFuncPtr(func) { };
			PartFunc(StringCRefGetterFunc func) : type(stringCRefGetterFunc), stringCRefGetterFuncPtr(func) { };
			PartFunc(StringFunc func) : type(stringFunc), stringFuncPtr(func) { };
			PartFunc(CStringFunc func) : type(cstringFunc), cstringFuncPtr(func) { };
			PartFunc(StringCRefFunc func) : type(stringCRefFunc), stringCRefFuncPtr(func) { };
		};

		std::map<std::string, PartFunc> funcTable;
	};

	class TemplatedContainerWidgetBase :
		public virtual ContainerWidgetBase
	{
	public:
		void SetItemTemplate(std::shared_ptr<ItemTemplateBase> const& it);
		std::shared_ptr<ItemTemplateBase> GetItemTemplate();

#ifdef TFC_HAS_PROPERTY
		__declspec(property(get = GetItemTemplate, put = SetItemTemplate))
		std::shared_ptr<ItemTemplateBase> ItemTemplate;
#endif
	protected:


		ItemTemplateBase::ItemPayload& GetTemplatedItem(ObjectClass& obj, void const* baseAddress);
		void RemoveTemplatedItem(void const* baseAddress);
	private:
		std::map<void const*, ItemTemplateBase::ItemPayload> itemPayloads;
		std::shared_ptr<ItemTemplateBase> itemTemplate;
	};

	class ListViewBase :
		public SelectorWidgetBase,
		public TemplatedContainerWidgetBase,
		EventEmitterClass<ListViewBase>
	{
	public:

	protected:
		//ListViewBase(Evas_Object* item) : SelectorWidgetBase(item) { }

		virtual Elm_Object_Item* AddItem(ObjectClass& obj, void const* baseAddress, Elm_Object_Item* itemBefore) override final;
		virtual void RemoveItem(Elm_Object_Item* item, void const* baseAddress) override final;

		virtual Elm_Object_Item* AddListItem(void* data, Elm_Gen_Item_Class* itemClass, Elm_Object_Item* itemBefore) = 0;
		virtual void RemoveListItem(Elm_Object_Item* item) = 0;
	};

	class ListView :
		public ListViewBase,
		EventEmitterClass<ListView>
	{
		using EventEmitterClass<ListView>::Event;

		EvasSmartEvent eventScrollingInternal;
		EvasSmartEvent eventScrollingDownInternal;
		EvasSmartEvent eventScrollingUpInternal;
		EvasSmartEvent eventItemRealized;

		bool overscroll { false };

		static void OnItemClickedInternal(void* data, Evas_Object* obj, void* dataItem);

	protected:
		virtual Elm_Object_Item* AddListItem(void* data, Elm_Gen_Item_Class* itemClass, Elm_Object_Item* itemBefore) override;
		virtual void RemoveListItem(Elm_Object_Item* item) override;

	public:
		class ScrollEventInfo
		{
		private:
			ListView const& ref;


		};


		ListView(Evas_Object* parent);
		virtual ~ListView();

		void ScrollToTop();
		void ScrollToBottom();

		void SetOverscroll(bool val);
		bool GetOverscroll() { return overscroll; };

		Event<ObjectClass&> eventItemLongClicked;
		Event<void*> eventScrollingUp;
		Event<void*> eventScrollingDown;

#ifdef TFC_HAS_PROPERTY
		__declspec(property(get = GetOverscroll, put = SetOverscroll))
		bool Overscroll;
#endif
	};

}}



#endif /* TFC_UI_LISTVIEW_H_ */
