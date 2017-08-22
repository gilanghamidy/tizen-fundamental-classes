/*
 * ContainerWidgetBase.cpp
 *
 *  Created on: Jul 25, 2017
 *      Author: gilang
 */

#include <TFC/UI/ContainerWidgetBase.h>
#include <TFC/Containers/ContainerBase.h>
#include <TFC/Containers/ObservableContainer.h>

using namespace TFC;
using namespace TFC::UI;

LIBAPI
std::shared_ptr<Containers::ContainerBase> ContainerWidgetBase::GetItemsSource() const
{
	return this->itemsSource;
}

LIBAPI
void TFC::UI::ContainerWidgetBase::CleanDataSource()
{
	// Item source is not empty, clean the list first


	if(this->observableObject)
	{
		for(auto& item : this->internalItems)
			static_cast<ObservableObjectClass*>(item.data)->eventObjectUpdated -= EventHandler(ContainerWidgetBase::OnCollectionItemUpdated);
	}

	if(this->referenceWrapped)
	{
		for(auto& item : this->internalItems)
			delete item.data;
	}

	this->internalItems.clear();
	this->indexBySource.clear();
	this->indexByObjectItem.clear();
	this->itemsSource.reset();
}


LIBAPI
void ContainerWidgetBase::SetItemsSource(std::shared_ptr<Containers::ContainerBase> const& ref)
{
	if(this->itemsSource)
	{
		CleanDataSource();
	}

	this->referenceWrappingValidated = false;
	this->referenceWrapped = false;

	this->observableObjectValidated = false;
	this->observableObject = false;

	if(ref)
	{
		this->itemsSource = ref;
		this->observableCollection = dynamic_cast<TFC::Containers::ObservableContainerBase*>(ref.get());

		if(this->observableCollection)
		{
			this->observableCollection->eventItemInserted += EventHandler(ContainerWidgetBase::OnCollectionItemInserted);
			this->observableCollection->eventItemRemoved += EventHandler(ContainerWidgetBase::OnCollectionItemRemoved);
		}

		if(!ref->Empty())
		{
			auto iter = ref->begin();
			auto iterEnd = ref->end();

			while(iter != iterEnd)
			{
				auto& item = *iter;
				if(!this->referenceWrappingValidated)
				{
					auto refWrapper = dynamic_cast<TFC::ReferenceWrapperBase*>(&item);
					if(refWrapper)
						this->referenceWrapped = true;
					else
						this->referenceWrapped = false;

					this->referenceWrappingValidated = true;
				}

				if(!this->observableObjectValidated)
				{
					auto observableObject = dynamic_cast<TFC::ObservableObjectClass*>(&item);
					if(observableObject)
						this->observableObject = true;
					else
						this->observableObject = false;

					this->observableObjectValidated = true;
				}


				ProcessInsertItem(item, iter.GetStorageAddress());
				++iter;
			}
		}
	}
}

LIBAPI
TFC::UI::ContainerWidgetBase::~ContainerWidgetBase()
{
	if(this->observableCollection)
	{
		this->observableCollection->eventItemInserted -= EventHandler(ContainerWidgetBase::OnCollectionItemInserted);
		this->observableCollection->eventItemRemoved -= EventHandler(ContainerWidgetBase::OnCollectionItemRemoved);
	}

	if(this->observableObject)
	{
		for(auto& item : this->internalItems)
			static_cast<ObservableObjectClass*>(item.data)->eventObjectUpdated -= EventHandler(ContainerWidgetBase::OnCollectionItemUpdated);
	}

	if(this->referenceWrapped)
	{
		for(auto& item : this->internalItems)
			delete item.data;
	}
}

void TFC::UI::ContainerWidgetBase::OnCollectionItemInserted(
	TFC::Containers::ObservableContainerBase* source,
	TFC::Containers::ItemEventArgs& args)
{
	ObjectClass& thisItem = *args.item;
	void const* thisItemAddress = args.item.GetStorageAddress();

	void const* itemAfter = nullptr;
	++args.item;
	if(args.item != source->end())
	{
		itemAfter = args.item.GetStorageAddress();
	}
	--args.item;

	ProcessInsertItem(thisItem, thisItemAddress, itemAfter);
}



void TFC::UI::ContainerWidgetBase::OnCollectionItemRemoved(
	TFC::Containers::ObservableContainerBase* source,
	TFC::Containers::ItemEventArgs& args)
{
	void const* thisItemAddress = args.item.GetStorageAddress();

	auto objectItem = GetObjectItemByStorageAddress(thisItemAddress);
	RemoveItem(objectItem, thisItemAddress);

	internalItems.erase(indexBySource.at(thisItemAddress));
	indexBySource.erase(thisItemAddress);
	indexByObjectItem.erase(objectItem);
}

Elm_Object_Item* TFC::UI::ContainerWidgetBase::GetObjectItemByStorageAddress(void const* itemDataAddress)
{
	if(!itemDataAddress)
		return nullptr;

	auto itemIter = indexBySource.find(itemDataAddress);
	if (itemIter != indexBySource.end())
		return itemIter->second->itemHandle;
	else
		return nullptr;
}

void TFC::UI::ContainerWidgetBase::ProcessInsertItem(ObjectClass& obj,
		void const* itemAddress, void const* itemAddressAfter)
{
	ObjectClass* dataPtr = nullptr;

	// Get the actual data address
	if(this->referenceWrapped)
	{
		auto refWrapper = static_cast<TFC::ReferenceWrapperBase*>(&obj);
		dataPtr = refWrapper->Clone().release();
	}
	else
	{
		dataPtr = &obj;

		if(this->observableObject)
		{
			ObservableObjectClass* obj = static_cast<ObservableObjectClass*>(dataPtr);
			obj->eventObjectUpdated += EventHandler(ContainerWidgetBase::OnCollectionItemUpdated);
		}
	}

	// Insert the new row in internal list
	auto newItemIter = internalItems.emplace(internalItems.end(), dataPtr);
	indexBySource.emplace(itemAddress, newItemIter);

	// Perform add item to the actual widget
	newItemIter->itemHandle = AddItem(obj, itemAddress, GetObjectItemByStorageAddress(itemAddressAfter));
	indexByObjectItem.emplace(newItemIter->itemHandle, &*newItemIter);
}

LIBAPI
SelectionMode TFC::UI::SelectorWidgetBase::GetSelectionMode()
{
	return this->selectionMode;
}

LIBAPI
void TFC::UI::SelectorWidgetBase::SetSelectionMode(enum SelectionMode mode)
{
	if(mode == SelectionMode::Multiple)
	{
		if(selectionMode == SelectionMode::Multiple)
			return;

		selectionMode = mode;

		if(singleSelectedItem)
		{
			SetSelected(singleSelectedItem->itemHandle, false);
			singleSelectedItem = nullptr;
		}
	}
	else if(mode == SelectionMode::Single)
	{
		if(selectionMode == SelectionMode::Single)
			return;

		selectionMode = mode;
		singleSelectedItem = nullptr;

		for(auto item : selectedItems)
		{
			SetSelected(item->itemHandle, false);
		}
		selectedItems.clear();
	}
	else
	{
		selectionMode = mode;

		if(singleSelectedItem)
		{
			SetSelected(singleSelectedItem->itemHandle, false);
			singleSelectedItem = nullptr;
		}

		for(auto item : selectedItems)
		{
			SetSelected(item->itemHandle, false);
		}
		selectedItems.clear();
	}
}

LIBAPI
std::vector<ObjectClass*> TFC::UI::SelectorWidgetBase::GetSelectedItems()
{
	std::vector<ObjectClass*> ret;

	for(auto item : selectedItems)
		ret.push_back(item->data);

	return ret;
}

LIBAPI
ObjectClass& TFC::UI::SelectorWidgetBase::GetSelectedItem()
{
	return *singleSelectedItem->data;
}

LIBAPI
void TFC::UI::SelectorWidgetBase::SetSelected(Elm_Object_Item* theItem, bool selected)
{

}

LIBAPI
void TFC::UI::SelectorWidgetBase::RemoveItem(Elm_Object_Item* item, const void* baseAddress)
{

}

LIBAPI
void TFC::UI::ContainerWidgetBase::OnItemClicked(ObjectClass& item)
{
	eventItemClicked(this, item);
}

void TFC::UI::ContainerWidgetBase::OnCollectionItemUpdated(TFC::ObservableObjectClass* source,
	void*)
{
	auto objectItem = GetObjectItemByStorageAddress(source);
	UpdateItem(objectItem);
}
