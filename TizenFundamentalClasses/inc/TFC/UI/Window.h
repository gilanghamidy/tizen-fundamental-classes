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

	class Window :
			public WidgetBase
	{
	public:
		Window(char const* windowName);

		void Show();
		void Hide();
		void Close();

		void SetExitOnClose(bool val);
		bool GetExitOnclose();


	};

}} // End Namespace TFC::UI



#endif /* TFC_UI_WINDOW_H_ */
