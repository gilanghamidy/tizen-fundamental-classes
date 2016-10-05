/*
 * ApplicationBase.cpp
 *
 * Source file for class ApplicationBase
 *
 *  Created on: Feb 12, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        ib.putu (ib.putu@samsung.com)
 */

#include "SRIN/Framework/Application.h"

using namespace SRIN::Framework;

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


SimpleReadOnlyProperty<ApplicationBase, ApplicationBase*> ApplicationBase::CurrentInstance;
SimpleReadOnlyProperty<ApplicationBase, CString> ApplicationBase::ResourcePath;

int ApplicationBase::Main(ApplicationBase* app, int argc, char* argv[])
{
	ApplicationBase::CurrentInstance = app;
	ApplicationBase::ResourcePath = app_get_resource_path();

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

LIBAPI ApplicationBase::ApplicationBase(CString packageName) :
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

std::string SRIN::Framework::ApplicationBase::GetResourcePath(CString path)
{
	std::string ret = CString(ApplicationBase::ResourcePath);
	ret += path;
	return ret;
}

LIBAPI void SRIN::Framework::INaviframeContent::RaiseAfterNaviframePush(Elm_Object_Item* naviframeItem)
{
	this->naviframeItem = naviframeItem;
	AfterNaviframePush(naviframeItem);
}

LIBAPI void SRIN::Framework::INaviframeContent::SetTitle(const std::string& value)
{
	ITitleProvider::SetTitle(value);
	elm_object_item_part_text_set(naviframeItem, "elm.text.title", value.c_str());
	dlog_print(DLOG_DEBUG, LOG_TAG, "Set title on INavCont %s", value.c_str());
}

LIBAPI SRIN::Framework::ITitleProvider::ITitleProvider() :
	Title(this)
{
}

LIBAPI SRIN::Framework::ITitleProvider::~ITitleProvider()
{
}

LIBAPI SRIN::Framework::HeadlessApplicationBase::HeadlessApplicationBase(CString packageName) :
	ApplicationBase(packageName)
{
}

LIBAPI bool SRIN::Framework::HeadlessApplicationBase::ApplicationCreate()
{
	return true;
}

LIBAPI void SRIN::Framework::HeadlessApplicationBase::ApplicationControl(app_control_h app_control)
{
	OnReceiveAppControlMessage(app_control);
	Exit();
}

