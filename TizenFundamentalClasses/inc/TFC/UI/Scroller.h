/*
 * IScroller.h
 *
 *  Created on: Aug 28, 2017
 *      Author: gmh
 */

#ifndef TFC_UI_SCROLLER_H_
#define TFC_UI_SCROLLER_H_

#include <TFC/UI/WidgetBase.h>

namespace TFC {
namespace UI {

	class ScrollerBase :
			public virtual WidgetBase,
			EventEmitterClass<ScrollerBase>
	{
		using EventEmitterClass<ScrollerBase>::Event;
	public:
		struct ScrollEventInfo
		{
			Evas_Coord_Rectangle before;
			Evas_Coord_Rectangle after;
		};

		Evas_Coord_Size GetContentSize();

		Event<ScrollEventInfo const&> eventScrolledUp;
		Event<ScrollEventInfo const&> eventScrolledDown;

#ifdef TFC_HAS_PROPERTY
		__declspec(property(get = GetContentSize))
		Evas_Coord_Size ContentSize;
#endif

	protected:
		ScrollerBase();

		EvasSmartEvent eventScrollingInternal;
		EvasSmartEvent eventScrollingDownInternal;
		EvasSmartEvent eventScrollingUpInternal;

	private:
		Evas_Coord_Rectangle currentCoordinate;

		void OnScrolledInternal(Evas_Object* obj, void* evd);
		void OnScrollingUpInternal(Evas_Object* obj, void* evd);
	};

}}


#endif /* TFC_UI_SCROLLER_H_ */
