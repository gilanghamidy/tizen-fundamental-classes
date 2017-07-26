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
#include <TFC/Containers/ContainerBase.h>
#include <TFC/UI/WidgetBase.h>

namespace TFC {
namespace UI {

	class ContainerWidgetBase :
		public WidgetBase,
		EventEmitterClass<ContainerWidgetBase>,
		PropertyClass<ContainerWidgetBase>
	{
	public:
		ContainerWidgetBase();

		TFC_Property(ContainerWidgetBase, bool, SelectionMode);
		TFC_Property(ContainerWidgetBase, std::shared_ptr<Containers::ContainerBase>, ItemsSource);

	protected:
		struct ContainerWidgetItem
		{
			std::unique_ptr<TFC::ReferenceWrapperBase> data;
			Elm_Object_Item* itemHandle;
			bool selected;
		};

		virtual Elm_Object_Item* AddItem(ObjectClass& obj, Elm_Object_Item* after) = 0;
		virtual void RemoveItem(Elm_Object_Item* item) = 0;

		void SetSelected(Elm_Object_Item* theItem, bool selected);
		virtual ~ContainerWidgetBase();

	private:
		std::shared_ptr<Containers::ContainerBase> itemsSource;
		std::map<void*, ContainerWidgetItem> listItems;
		std::map<Elm_Object_Item*, ContainerWidgetItem*> itemIndex;
		std::deque<void*> selectedItems;
	};

}}



#endif /* TFC_UI_CONTAINERWIDGETBASE_H_ */
