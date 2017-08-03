/*
 * Window.h
 *
 *  Created on: Aug 2, 2017
 *      Author: gmh
 */

#ifndef TFC_UI_WINDOW_H_
#define TFC_UI_WINDOW_H_

#include <TFC/UI/WidgetBase.h>

namespace TFC {
namespace UI {

	class LIBAPI Window :
			public WidgetBase,
			EventEmitterClass<Window>
	{
		using EventEmitterClass<Window>::Event;
	public:
		Window(char const* windowName);

		void Show();
		void Hide();
		void Close();

		void SetExitOnClose(bool val);
		bool GetExitOnclose();

		Event<void*> eventClosing;

	protected:
		Evas_Object* const& conformant;

		virtual void OnBackButtonClicked();
		virtual void OnMoreButtonClicked();

	private:
		Evas_Object* conformantPtr;
	};

	class LIBAPI TransparentWindow :
			public Window
	{
	public:
		TransparentWindow(char const* windowName);

	protected:
		Evas_Object* const& contentWrapper;

	private:
		Evas_Object* contentWrapperPtr;
	};

}} // End Namespace TFC::UI



#endif /* TFC_UI_WINDOW_H_ */
