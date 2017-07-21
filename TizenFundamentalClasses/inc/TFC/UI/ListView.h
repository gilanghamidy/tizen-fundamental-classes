/*
 * ListView.h
 *
 *  Created on: Jul 21, 2017
 *      Author: gilang
 */

#ifndef TFC_UI_LISTVIEW_H_
#define TFC_UI_LISTVIEW_H_

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

	protected:
		struct ContainerWidgetItem
		{

		};

	private:
		std::shared_ptr<Containers::ContainerBase> itemsSource;
	};

	class ListViewBase :
		public ContainerWidgetBase,
		EventEmitterClass<ListViewBase>,
		PropertyClass<ListViewBase>
	{

	};

	class ListView :
		public ListViewBase,
		EventEmitterClass<ListView>,
		PropertyClass<ListView>
	{
		using EventEmitterClass<ListView>::Event;

	public:
		ListView(Evas_Object* parent);
		virtual ~ListView();


	};

}}



#endif /* TFC_UI_LISTVIEW_H_ */
