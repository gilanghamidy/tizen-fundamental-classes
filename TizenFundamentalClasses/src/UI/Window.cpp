/*
 * Window.cpp
 *
 *  Created on: Aug 2, 2017
 *      Author: gmh
 */

#include <TFC/UI/Window.h>
#include <TFC/Framework/Application.h>

using namespace TFC::UI;

LIBAPI
Window::Window(char const* windowName) :
		WidgetBase(elm_win_add(nullptr, windowName, ELM_WIN_BASIC)),
		conformant(conformantPtr)
{
	Elm_Theme* defaultTheme = elm_theme_new();
	elm_theme_ref_set(defaultTheme, elm_theme_default_get());

	elm_object_theme_set(this->widgetRoot, defaultTheme);
	elm_win_conformant_set(this->widgetRoot, EINA_TRUE);

	this->conformantPtr = elm_conformant_add(this->widgetRoot);
	evas_object_size_hint_weight_set(this->conformantPtr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(this->widgetRoot, this->conformantPtr);
	evas_object_show(this->conformantPtr);

	eext_object_event_callback_add(
			this->widgetRoot,
			EEXT_CALLBACK_BACK,
			[] (void *data, Evas_Object *obj, void *event_info)
			{
				static_cast<Window*>(data)->OnBackButtonClicked();
			},
			this);

	eext_object_event_callback_add(
			this->widgetRoot,
			EEXT_CALLBACK_MORE,
			[] (void *data, Evas_Object *obj, void *event_info)
			{
				static_cast<Window*>(data)->OnMoreButtonClicked();
			},
			this);

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
	eventClosing(this, nullptr);

	ecore_job_add([](void* data) {
		evas_object_del(static_cast<Evas_Object*>(data));
	}, this->widgetRoot);
}

LIBAPI
void Window::SetExitOnClose(bool val)
{
	elm_win_autodel_set(this->widgetRoot, val ? EINA_TRUE : EINA_FALSE);
}

LIBAPI
bool Window::GetExitOnclose()
{
	return elm_win_autodel_get(this->widgetRoot);
}

LIBAPI
void TFC::UI::Window::OnBackButtonClicked()
{
	this->Close();
}

LIBAPI
void TFC::UI::Window::OnMoreButtonClicked()
{

}

LIBAPI
TFC::UI::TransparentWindow::TransparentWindow(const char* windowName) :
		Window(windowName)
{
	elm_win_alpha_set(this->widgetRoot, EINA_TRUE);
	elm_win_indicator_mode_set(this->widgetRoot, ELM_WIN_INDICATOR_HIDE);

	this->contentWrapperPtr = edje_object_add(this->conformant);
	auto path = Framework::ApplicationBase::GetResourcePath("TFC/edc/Content.edj");
	edje_object_file_set(this->contentWrapperPtr, path.c_str(), "transparent-background");
	evas_object_size_hint_align_set(this->contentWrapperPtr, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(this->contentWrapperPtr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(this->contentWrapperPtr);

	elm_object_content_set(this->conformant, this->contentWrapperPtr);

	this->eventBackgroundClicked.Bind(this->contentWrapperPtr, "elm,action,click", "");
	this->eventBackgroundClicked += EventHandler(TransparentWindow::OnBackgroundClicked);
}

LIBAPI
void TFC::UI::TransparentWindow::AttachContent(
		Evas_Object* (TransparentWindow::*contentFunc)(Evas_Object* parent))
{
	edje_object_part_swallow(this->contentWrapperPtr, "content", (this->*contentFunc)(this->contentWrapperPtr));
}

void TFC::UI::TransparentWindow::OnBackgroundClicked(Evas_Object* src, EFL::EdjeSignalInfo info)
{
	this->Close();
}
