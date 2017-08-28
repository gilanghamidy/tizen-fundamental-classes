/*
 * IScroller.cpp
 *
 *  Created on: Aug 28, 2017
 *      Author: gmh
 */

#include <TFC/UI/Scroller.h>

TFC::UI::ScrollerBase::ScrollerBase() : WidgetBase(nullptr)
{
	eventScrollingInternal.Bind(widgetRoot, "scroll,drag,stop");
	//eventScrollingDownInternal.Bind(widgetRoot, "scroll,down");
	//eventScrollingUpInternal.Bind(widgetRoot, "scroll,up");

	eventScrollingInternal += EventHandler(ScrollerBase::OnScrolledInternal);
	//eventScrollingUpInternal += EventHandler(ScrollerBase::OnScrollingUpInternal);
}

void TFC::UI::ScrollerBase::OnScrolledInternal(Evas_Object* obj, void* evd)
{
	Evas_Coord_Rectangle after;
	elm_scroller_region_get(obj, &after.x, &after.y, &after.w, &after.h);

	if(after.y > currentCoordinate.y)
		eventScrolledDown(this, ScrollEventInfo { this->currentCoordinate, after });
	else
		eventScrolledUp(this, ScrollEventInfo { this->currentCoordinate, after });

	this->currentCoordinate = after;
}
LIBAPI
Evas_Coord_Size TFC::UI::ScrollerBase::GetContentSize()
{
	Evas_Coord_Size ret;
	elm_scroller_child_size_get(widgetRoot, &ret.w, &ret.h);
	return ret;
}

void TFC::UI::ScrollerBase::OnScrollingUpInternal(Evas_Object* obj, void* evd)
{
	Evas_Coord_Rectangle after;
	elm_scroller_region_get(obj, &after.x, &after.y, &after.w, &after.h);
	eventScrolledDown(this, ScrollEventInfo { this->currentCoordinate, after });
	this->currentCoordinate = after;
}
