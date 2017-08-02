/*
 * Window.cpp
 *
 *  Created on: Aug 2, 2017
 *      Author: gmh
 */

#include <TFC/UI/Window.h>

using namespace TFC::UI;

LIBAPI
Window::Window(char const* windowName) :
		WidgetBase(elm_win_add(nullptr, windowName, ELM_WIN_DIALOG_BASIC))
{
	Elm_Theme* defaultTheme = elm_theme_new();
	elm_theme_ref_set(defaultTheme, elm_theme_default_get());

	elm_object_theme_set(this->widgetRoot, defaultTheme);
}

LIBAPI
void Window::Show()
{
	this->Visible = true;
	elm_win_activate(this->widgetRoot);
}

LIBAPI
void Window::Hide()
{
	this->Visible = false;
}

LIBAPI
void Window::Close()
{
	ecore_job_add([](void* data) {
		evas_object_del(static_cast<Evas_Object*>(data));
	}, this->widgetRoot);
}

LIBAPI
void Window::SetExitOnClose(bool val)
{
	elm_win_autodel_set(this->widgetRoot, val);
}

LIBAPI
bool Window::GetExitOnclose()
{
	return elm_win_autodel_get(this->widgetRoot);
}
