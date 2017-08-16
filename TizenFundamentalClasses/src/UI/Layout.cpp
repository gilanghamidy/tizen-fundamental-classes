/*
 * Layout.cpp
 *
 *  Created on: Aug 16, 2017
 *      Author: gmh
 */


#include "TFC/UI/Layout.h"

using namespace TFC::UI;

LIBAPI
Evas_Object* Layout::LayoutPart::operator=(Evas_Object* object)
{
	elm_object_part_content_set(this->layoutObj, this->part, object);
	return object;
}

LIBAPI
Layout::LayoutPart::operator Evas_Object*()
{
	return elm_object_part_content_get(this->layoutObj, this->part);
}

LIBAPI
Layout::LayoutPart::LayoutPart(Layout& obj, const char* part) :
	layoutObj(obj), part(part)
{

}

LIBAPI
Layout::Layout(Evas_Object* parent) :
	WidgetBase(elm_layout_add(parent))
{

}

LIBAPI
Layout::LayoutPart Layout::operator [](const char* part)
{
	return { *this, part };
}

LIBAPI
void Layout::SetTheme(const char* group, const char* style)
{
	elm_layout_theme_set(this->widgetRoot, "layout", group, style);
}

LIBAPI
void TFC::UI::ApplicationLayout::FloatingButton::SetVisible(bool val)
{
}

LIBAPI
void TFC::UI::ApplicationLayout::FloatingButton::SetImage(const char* image)
{
}

LIBAPI
void TFC::UI::ApplicationLayout::FloatingButton::SetImage(const std::string& ref)
{
}

LIBAPI
std::string TFC::UI::ApplicationLayout::FloatingButton::GetImage()
{

}

LIBAPI
TFC::UI::ApplicationLayout::FloatingButton::FloatingButton(Evas_Object* layout)
{
}

LIBAPI
TFC::UI::ApplicationLayout::ApplicationLayout(Evas_Object* parent)
{
}
