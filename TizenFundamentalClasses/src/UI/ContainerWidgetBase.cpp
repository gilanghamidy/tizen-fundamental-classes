/*
 * ContainerWidgetBase.cpp
 *
 *  Created on: Jul 25, 2017
 *      Author: gilang
 */

#include <TFC/UI/ContainerWidgetBase.h>

using namespace TFC;
using namespace TFC::UI;

LIBAPI
std::shared_ptr<Containers::ContainerBase> ContainerWidgetBase::GetItemsSource() const
{
	return this->itemsSource;
}

LIBAPI
void ContainerWidgetBase::SetItemsSource(std::shared_ptr<Containers::ContainerBase> const& ref)
{
	if(this->itemsSource)
	{
		// Item source is not empty, clean the list first
	}

	this->itemsSource = ref;

	this->observableCollection = dynamic_cast<TFC::Containers::ObservableContainerBase*>(ref.get());

	if(this->observableCollection)
	{
		this->observableCollection->eventItemInserted += EventHandler(ContainerWidgetBase::OnCollectionItemAdded);
		this->observableCollection->eventItemRemoved += EventHandler(ContainerWidgetBase::OnCollectionItemRemoved);
	}

	if(!ref->Empty())
	{

	}
}

LIBAPI
TFC::UI::ContainerWidgetBase::~ContainerWidgetBase()
{
	if(this->observableCollection)
	{
		this->observableCollection->eventItemInserted -= EventHandler(ContainerWidgetBase::OnCollectionItemAdded);
		this->observableCollection->eventItemRemoved -= EventHandler(ContainerWidgetBase::OnCollectionItemRemoved);
	}
}

void TFC::UI::ContainerWidgetBase::OnCollectionItemInserted(
	TFC::Containers::ObservableContainerBase* source,
	TFC::Containers::ObservableContainerBase::ItemEventArgs args) {
}



void TFC::UI::ContainerWidgetBase::OnCollectionItemRemoved(
	TFC::Containers::ObservableContainerBase* source,
	TFC::Containers::ObservableContainerBase::ItemEventArgs args) {
}
