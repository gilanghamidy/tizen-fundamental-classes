/*
 * Tizen Fundamental Classes - TFC
 * Copyright (c) 2016-2017 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *    Framework/ApplicationBase.cpp
 *
 * Created on:  Feb 12, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Ida Bagus Putu Peradnya Dinata (ib.putu@samsung.com)
 */

#include <dlog.h>
#include <iostream>
#include <fstream>

#include "TFC/Framework/Application.h"

using namespace TFC::Framework;

bool ApplicationBase_AppCreateHandler(void *data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	return app->ApplicationCreate();
}

void ApplicationBase_AppControlHandler(app_control_h app_control, void* data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->ApplicationControl(app_control);
}

void ApplicationBase_AppPauseHandler(void* data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->ApplicationPause();
}

void ApplicationBase_AppResumeHandler(void* data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->ApplicationResume();
}

void ApplicationBase_AppTerminateHandler(void* data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->ApplicationTerminate();
}

void ui_app_lang_changed(app_event_info_h event_info, void* data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);

	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->LanguageChanged(event_info, locale);

	free(locale);
	return;
}

void ui_app_orient_changed(app_event_info_h event_info, void *data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->OrientationChanged(event_info);
}

void ui_app_region_changed(app_event_info_h event_info, void *data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->RegionChanged(event_info);
}

void ui_app_low_battery(app_event_info_h event_info, void *data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->LowBattery(event_info);
}

void ui_app_low_memory(app_event_info_h event_info, void *data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->LowMemory(event_info);
}

/* REMOVE THIS KIND OF PROPERTY
SimpleReadOnlyProperty<ApplicationBase, ApplicationBase*> ApplicationBase::CurrentInstance;
SimpleReadOnlyProperty<ApplicationBase, char const*> ApplicationBase::ResourcePath;
*/

ApplicationBase* ApplicationBase::currentInstance = nullptr;
char const*		 ApplicationBase::resourcePath = nullptr;

std::unique_ptr<std::ofstream> tfcout;

int ApplicationBase::Main(ApplicationBase* app, int argc, char* argv[])
try
{
	tfcout.reset(new std::ofstream("/tmp/tfcerr", std::ios::out));

	ApplicationBase::currentInstance = app;
	ApplicationBase::resourcePath = app_get_resource_path();

	ecore_thread_max_set(16);

	::ui_app_lifecycle_callback_s event_callback =
	{ ApplicationBase_AppCreateHandler, ApplicationBase_AppTerminateHandler, ApplicationBase_AppPauseHandler,
		ApplicationBase_AppResumeHandler, ApplicationBase_AppControlHandler };

	::app_event_handler_h handlers[5] =
	{ NULL, };

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, app);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, app);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED,
		ui_app_orient_changed, app);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed,
		app);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED,
		ui_app_region_changed, app);

	int ret = ::ui_app_main(argc, argv, &event_callback, app);
	if (ret != APP_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
catch(TFC::TFCException& ex)
{
	dlog_print(DLOG_ERROR, argv[0], "Exception occured (%s) => %s; Stack trace %s", typeid(ex).name(), ex.what(), ex.GetStackTrace().c_str());

	*tfcout << "Unhandled Exception: " << typeid(ex).name() << '\n' << ex.what() << '\n';
	*tfcout << "Stack trace: " << '\n';
	*tfcout << ex.GetStackTrace() << '\n';
	*tfcout << "END\n";

	throw;
}

LIBAPI ApplicationBase::ApplicationBase(char const* packageName) :
	packageName(packageName)
{


}

LIBAPI void ApplicationBase::ApplicationControl(app_control_h app_control)
{
}

LIBAPI void ApplicationBase::ApplicationPause()
{
}

LIBAPI void ApplicationBase::ApplicationResume()
{
}

LIBAPI void ApplicationBase::ApplicationTerminate()
{
}

LIBAPI void ApplicationBase::LanguageChanged(app_event_info_h event_info, const char* locale)
{

}

LIBAPI void ApplicationBase::OrientationChanged(app_event_info_h event_info)
{
}

LIBAPI void ApplicationBase::RegionChanged(app_event_info_h event_info)
{
}

LIBAPI void ApplicationBase::LowBattery(app_event_info_h event_info)
{
}

LIBAPI void ApplicationBase::LowMemory(app_event_info_h event_info)
{

}

LIBAPI void ApplicationBase::Exit()
{
	ui_app_exit();
}

LIBAPI ApplicationBase::~ApplicationBase()
{

}

std::string TFC::Framework::ApplicationBase::GetResourcePath(char const* path)
{
	if (ApplicationBase::resourcePath == nullptr)
		ApplicationBase::resourcePath = app_get_resource_path(); // make sure cache exist without calling main

	std::string ret(ApplicationBase::resourcePath); // note: will give logic_error if app_get_resource_path() failed
	ret += path;
	return ret;
}

LIBAPI void TFC::Framework::INaviframeContent::RaiseAfterNaviframePush(Elm_Object_Item* naviframeItem)
{
	this->naviframeItem = naviframeItem;
	AfterNaviframePush(naviframeItem);
}

LIBAPI void TFC::Framework::INaviframeContent::SetTitle(const std::string& value)
{
	ITitleProvider::SetTitle(value);
	elm_object_item_part_text_set(naviframeItem, "elm.text.title", value.c_str());
	dlog_print(DLOG_DEBUG, LOG_TAG, "Set title on INavCont %s", value.c_str());
}

LIBAPI TFC::Framework::ITitleProvider::ITitleProvider() :
	Title(this)
{
}

LIBAPI TFC::Framework::ITitleProvider::~ITitleProvider()
{
}

LIBAPI TFC::Framework::HeadlessApplicationBase::HeadlessApplicationBase(char const* packageName) :
	ApplicationBase(packageName)
{
}

LIBAPI bool TFC::Framework::HeadlessApplicationBase::ApplicationCreate()
{
	return true;
}

LIBAPI void TFC::Framework::HeadlessApplicationBase::ApplicationControl(app_control_h app_control)
{
	OnReceiveAppControlMessage(app_control);
	Exit();
}




