/*
 * ContainerWidgetBase.h
 *
 *  Created on: Jul 25, 2017
 *      Author: gilang
 */

#ifndef TFC_UI_CONTAINERWIDGETBASE_H_
#define TFC_UI_CONTAINERWIDGETBASE_H_

#include <memory>
#include <deque>
#include <map>
#include <list>
#include <TFC/UI/WidgetBase.h>

namespace TFC {

namespace Containers {
	class ContainerBase;
	class ObservableContainerBase;
	struct ItemEventArgs;
}

namespace UI {

	class ContainerWidgetBase :
		public WidgetBase,
		EventEmitterClass<ContainerWidgetBase>
	{
		using EventEmitterClass<ContainerWidgetBase>::Event;

	public:
		std::shared_ptr<Containers::ContainerBase> GetItemsSource() const;
		void SetItemsSource(std::shared_ptr<Containers::ContainerBase> const& value);

		Event<ObjectClass&> eventItemClicked;

#ifdef TFC_HAS_PROPERTY
		__declspec(property(get = GetItemsSource, put = SetItemsSource))
		std::shared_ptr<Containers::ContainerBase> ItemsSource;
#endif

	protected:
		struct ContainerWidgetItem
		{
			ObjectClass* data;
			Elm_Object_Item* itemHandle { nullptr };

			ContainerWidgetItem(ObjectClass* data) : data(data) { }
		};

		ContainerWidgetBase(Evas_Object* item) : WidgetBase(item) { };
		virtual Elm_Object_Item* AddItem(ObjectClass& obj, void const* baseAddress, Elm_Object_Item* itemBefore) = 0;
		virtual void RemoveItem(Elm_Object_Item* item, void const* baseAddress) = 0;

		ContainerWidgetItem* GetWidgetItemByData(void* data) { return indexBySource.at(data); }
		ObjectClass& GetDataByItemHandle(Elm_Object_Item* item) { return *indexByObjectItem.at(item)->data; }
		virtual ~ContainerWidgetBase();

		void OnItemClicked(ObjectClass& item);
	private:
		std::shared_ptr<Containers::ContainerBase> itemsSource;
		std::deque<ContainerWidgetItem> internalItems;
		std::map<void const*, ContainerWidgetItem*> indexBySource;
		std::map<Elm_Object_Item*, ContainerWidgetItem*> indexByObjectItem;

		void CleanDataSource();

		TFC::Containers::ObservableContainerBase* observableCollection { nullptr };
		bool referenceWrapped { false };
		bool referenceWrappingValidated { false };

		void OnCollectionItemInserted(TFC::Containers::ObservableContainerBase* source, TFC::Containers::ItemEventArgs& args);
		void OnCollectionItemRemoved(TFC::Containers::ObservableContainerBase* source, TFC::Containers::ItemEventArgs& args);

		void ProcessInsertItem(ObjectClass& obj, void const* itemAddress, void const* itemAddressAfter = nullptr);
		Elm_Object_Item* GetObjectItemByStorageAddress(void const* itemBefore);
	};

	enum class SelectionMode {
		None = 0,
		Single,
		Multiple
	};

	class SelectorWidgetBase :
		public virtual ContainerWidgetBase,
		EventEmitterClass<SelectorWidgetBase>
	{
	public:
		SelectionMode GetSelectionMode();
		void SetSelectionMode(SelectionMode mode);

		template<typename T>
		void AddToSelection(T& item)
		{

		}

		template<typename T>
		void RemoveFromSelection(T& item)
		{

		}

		std::vector<ObjectClass*> GetSelectedItems();

		ObjectClass& GetSelectedItem();

#ifdef TFC_HAS_PROPERTY
		__declspec(property(get = GetSelectionMode, put = SetSelectionMode))
		SelectionMode SelectionMode;

		__declspec(property(get = GetSelectedItems))
		std::vector<ObjectClass*> SelectedItems;

		__declspec(property(get = GetSelectedItem))
		ObjectClass& SelectedItem;
#endif

	protected:
		//SelectorWidgetBase(Evas_Object* item) : ContainerWidgetBase(item) { }
		void SetSelected(Elm_Object_Item* theItem, bool selected);

		virtual void RemoveItem(Elm_Object_Item* item, void const* baseAddress) override;
	private:
		std::deque<ContainerWidgetItem*> selectedItems;
		ContainerWidgetItem* singleSelectedItem { nullptr };
		enum SelectionMode selectionMode { SelectionMode::Single };
	};
}}



#endif /* TFC_UI_CONTAINERWIDGETBASE_H_ */
