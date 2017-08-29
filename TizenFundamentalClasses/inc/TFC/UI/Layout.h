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

			void SetPosition(Eext_Floatingbutton_Pos pos);
			Eext_Floatingbutton_Pos GetPosition();

			void SetMovable(bool val);
			bool GetMovable();

			void BringIn();
			void BringOut();

#ifdef TFC_HAS_PROPERTY
			__declspec(property(get = GetImage, put = SetImage))
			std::string Image;

			__declspec(property(get = GetVisible, put = SetVisible))
			bool Visible;

			__declspec(property(get = GetPosition, put = SetPosition))
			Eext_Floatingbutton_Pos Position;

			__declspec(property(get = GetMovable, put = SetMovable))
			bool Movable;
#endif

		private:
			FloatingButton(Evas_Object* layout);
			Eext_Floatingbutton_Pos position;
			Evas_Object* buttonObj;
			Evas_Object* imageObj;
			Evas_Object* floatingObj;

			bool visible { false };

			friend class ApplicationLayout;
		};

		ApplicationLayout(Evas_Object* parent);

		FloatingButton& GetFloatingButton() { return floatingButton; }

#ifdef TFC_HAS_PROPERTY
		__declspec(property(get = GetFloatingButton))
		FloatingButton& FloatingButton;
#endif

	private:
		class FloatingButton floatingButton;
	};

}}



#endif /* TFC_UI_LAYOUT_H_ */
