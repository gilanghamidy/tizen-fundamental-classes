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
	elm_layout_theme_set(widgetRoot, "layout", "application", "default");
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
	visible = val;

	if(val)
		elm_object_signal_emit(floatingObj, "elm,state,show", "");
	else
		elm_object_signal_emit(floatingObj, "elm,state,hide", "");
}

LIBAPI
void TFC::UI::ApplicationLayout::FloatingButton::SetImage(const char* image)
{
	elm_image_file_set(imageObj, image, nullptr);
}

LIBAPI
void TFC::UI::ApplicationLayout::FloatingButton::SetImage(const std::string& ref)
{
	elm_image_file_set(imageObj, ref.c_str(), nullptr);
}

LIBAPI
std::string TFC::UI::ApplicationLayout::FloatingButton::GetImage()
{
	char const* file = nullptr;
	elm_image_file_get(imageObj, &file, nullptr);

	return { file };
}

LIBAPI
TFC::UI::ApplicationLayout::FloatingButton::FloatingButton(Evas_Object* layout)
{
	floatingObj = eext_floatingbutton_add(layout);
	elm_object_part_content_set(layout, "elm.swallow.floatingbutton", floatingObj);
	//eext_floatingbutton_movement_block_set(floatingObj, EINA_TRUE);

	buttonObj = elm_button_add(floatingObj);
	elm_object_part_content_set(floatingObj, "button1", buttonObj);

	imageObj = elm_image_add(buttonObj);
	elm_object_part_content_set(buttonObj, "icon", imageObj);
	evas_object_show(imageObj);
	evas_object_show(buttonObj);
	evas_object_show(floatingObj);
}

LIBAPI
TFC::UI::ApplicationLayout::ApplicationLayout(Evas_Object* parent) :
		Layout(parent),
		floatingButton(widgetRoot)
{

}

LIBAPI
void TFC::UI::ApplicationLayout::FloatingButton::SetPosition(Eext_Floatingbutton_Pos pos)
{
	if(pos != position)
	{
		if(pos == EEXT_FLOATINGBUTTON_LEFT_OUT || pos == EEXT_FLOATINGBUTTON_RIGHT_OUT)
		{
			eext_floatingbutton_pos_set(floatingObj, pos);
			position = pos == EEXT_FLOATINGBUTTON_LEFT_OUT ? EEXT_FLOATINGBUTTON_LEFT : EEXT_FLOATINGBUTTON_RIGHT;
		}
		else
		{
			auto currentPos = eext_floatingbutton_pos_get(floatingObj);

			if(currentPos != EEXT_FLOATINGBUTTON_LEFT_OUT || currentPos != EEXT_FLOATINGBUTTON_RIGHT_OUT)
			{
				eext_floatingbutton_pos_set(floatingObj, pos);
			}
			else
			{
				eext_floatingbutton_pos_set(floatingObj, pos == EEXT_FLOATINGBUTTON_LEFT ? EEXT_FLOATINGBUTTON_LEFT_OUT : EEXT_FLOATINGBUTTON_RIGHT_OUT);
			}

			position = pos;
		}
	}
}

LIBAPI
Eext_Floatingbutton_Pos TFC::UI::ApplicationLayout::FloatingButton::GetPosition()
{
	return position;
}

LIBAPI
void TFC::UI::ApplicationLayout::FloatingButton::SetMovable(bool val)
{
	eext_floatingbutton_movement_block_set(floatingObj, val ? EINA_FALSE : EINA_TRUE);
}

LIBAPI
bool TFC::UI::ApplicationLayout::FloatingButton::GetMovable()
{
	return eext_floatingbutton_movement_block_get(floatingObj) == EINA_FALSE ? true : false;
}

LIBAPI
void TFC::UI::ApplicationLayout::FloatingButton::BringIn()
{
	auto currentPos = eext_floatingbutton_pos_get(floatingObj);

	if(currentPos != EEXT_FLOATINGBUTTON_LEFT_OUT || currentPos != EEXT_FLOATINGBUTTON_RIGHT_OUT)
	{
		return;
	}

	bool isMovable = true;
	if(!this->Movable)
	{
		isMovable = false;
		this->Movable = true;
	}

	eext_floatingbutton_pos_bring_in(floatingObj, position);

	if(!isMovable)
	{
		this->Movable = false;
	}
}

LIBAPI
void TFC::UI::ApplicationLayout::FloatingButton::BringOut()
{
	auto currentPos = eext_floatingbutton_pos_get(floatingObj);

	if(currentPos == EEXT_FLOATINGBUTTON_LEFT_OUT || currentPos == EEXT_FLOATINGBUTTON_RIGHT_OUT)
	{
		return;
	}

	bool isMovable = true;
	if(!this->Movable)
	{
		isMovable = false;
		this->Movable = true;
	}

	eext_floatingbutton_pos_bring_in(floatingObj, position == EEXT_FLOATINGBUTTON_LEFT ? EEXT_FLOATINGBUTTON_LEFT_OUT : EEXT_FLOATINGBUTTON_RIGHT_OUT);

	if(!isMovable)
	{
		this->Movable = false;
	}
}
