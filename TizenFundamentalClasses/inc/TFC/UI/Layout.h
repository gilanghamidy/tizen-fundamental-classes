/*
 * Layout.h
 *
 *  Created on: Aug 16, 2017
 *      Author: gmh
 */

#ifndef TFC_UI_LAYOUT_H_
#define TFC_UI_LAYOUT_H_

#include <TFC/UI/WidgetBase.h>

namespace TFC {
namespace UI {

	class LIBAPI Layout :
			public WidgetBase
	{
	public:
		class LayoutPart
		{
		public:
			Evas_Object* operator=(Evas_Object* object);
			operator Evas_Object*();

		private:
			Layout& layoutObj;
			char const* part;

			LayoutPart(Layout& obj, char const* part);
			friend class Layout;
		};

		Layout(Evas_Object* parent);
		LayoutPart operator[](char const* part);

		void SetTheme(char const* group, char const* style);

	private:
		friend class LayoutPart;
	};

	class LIBAPI ApplicationLayout :
			public Layout
	{
	public:
		class FloatingButton :
				EFL::EFLProxyClass
		{
		public:
			EvasSmartEvent eventButtonClicked;

			void SetVisible(bool val);
			bool GetVisible() { return visible; }

			void SetImage(char const* image);
			void SetImage(std::string const& ref);
			std::string GetImage();

		private:
			FloatingButton(Evas_Object* layout);

			Evas_Object* buttonObj;
			Evas_Object* floatingObj;

			bool visible { false };
		};

		ApplicationLayout(Evas_Object* parent);

		FloatingButton& GetLeftFloatingButton() { return leftButton; }
		FloatingButton& GetRightFloatingButton() { return rightButton; }

#ifdef TFC_HAS_PROPERTY
		__declspec(property(get = GetLeftFloatingButton))
		FloatingButton& LeftFloatingButton;

		__declspec(property(get = GetRightFloatingButton))
		FloatingButton& RightFloatingButton;
#endif

	private:
		FloatingButton leftButton;
		FloatingButton rightButton;
	};

}}



#endif /* TFC_UI_LAYOUT_H_ */
