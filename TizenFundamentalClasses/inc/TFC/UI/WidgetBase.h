/*
 * WidgetBase.h
 *
 *  Created on: Jul 15, 2017
 *      Author: gilang
 */

#ifndef TFC_UI_WIDGETBASE_H_
#define TFC_UI_WIDGETBASE_H_

#include "TFC/Core.h"
#include "TFC/EFL.h"
#include <string>

namespace TFC {
namespace UI {

	template<typename T>
	struct Coordinate {
		T x;
		T y;
	};

	static constexpr Coordinate<double> HintFillAll { EVAS_HINT_FILL, EVAS_HINT_FILL };
	static constexpr Coordinate<double> HintExpandAll { EVAS_HINT_EXPAND, EVAS_HINT_EXPAND };

	class WidgetBase :
			public EventEmitterClass<WidgetBase>,
			public EFL::EFLProxyClass,
			public ManagedClass
	{
	public:
		operator Evas_Object*();
		operator Evas_Object const*() const;

		bool IsEnabled() const;
		void SetEnabled(bool b);

		bool IsVisible() const;
		void SetVisible(bool b);

		Coordinate<double> GetWeight() const;
		void SetWeight(Coordinate<double> const& c);

		Coordinate<double> GetAlign() const;
		void SetAlign(Coordinate<double> const& c);

#ifdef TFC_HAS_PROPERTY
		__declspec(property(get = IsEnabled, put = SetEnabled))
		bool Enabled;

		__declspec(property(get = IsVisible, put = SetVisible))
		bool Visible;

		__declspec(property(get = GetWeight, put = SetWeight))
		Coordinate<double> Weight;

		__declspec(property(get = GetAlign, put = SetAlign))
		Coordinate<double> Align;
#endif

	protected:
		WidgetBase(Evas_Object* widget);
		Evas_Object* const& widgetRoot;
		virtual ~WidgetBase() = 0;

	private:
		static void Deallocator(void* userData, Evas_Object* src, Evas* evas, void* eventData);
		Evas_Object* widgetRootInternal;
	};

}}

#endif /* TFC_UI_WIDGETBASE_H_ */
