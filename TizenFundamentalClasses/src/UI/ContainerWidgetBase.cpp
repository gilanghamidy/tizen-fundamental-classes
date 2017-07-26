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
	this->itemsSource = ref;
}
