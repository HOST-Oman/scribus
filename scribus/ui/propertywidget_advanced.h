/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef PROPERTYWIDGET_ADVANCED_H
#define PROPERTYWIDGET_ADVANCED_H

#include "ui_propertywidget_advancedbase.h"

#include "propertywidgetbase.h"
#include "units.h"

class CharStyle;
class ParagraphStyle;
class ScribusDoc;
class ScribusMainWindow;

class PropertyWidget_Advanced : public QFrame, Ui::PropertyWidget_AdvancedBase,
                                public PropertyWidgetBase
{
	Q_OBJECT

public:
	PropertyWidget_Advanced(QWidget* parent);
	~PropertyWidget_Advanced() {};

protected:
	void connectSignals();
	void disconnectSignals();

	double m_unitRatio {1.0};
	int m_unitIndex {SC_PT};
	bool m_blockUpdate {false};
	PageItem *m_item {nullptr};
	ScribusMainWindow* m_ScMW {nullptr};

	void configureWidgets();
	void setCurrentItem(PageItem *item);

	void changeEvent(QEvent *e) override;

public slots:
	void setMainWindow(ScribusMainWindow *mw);
	void setDoc(ScribusDoc *d);

	void handleSelectionChanged();

	void iconSetChange();
	void languageChange();
	void unitChange() {};
	void localeChange();
	void toggleLabelVisibility(bool v);

	void showBaseLineOffset(double e);
	void showTextScaleH(double e);
	void showTextScaleV(double e);
	void showTracking(double e);

	void showOutlineW(double x);
	void showShadowOffset(double x, double y);
	void showStrikeThru(double p, double w);
	void showTextEffects(int s);
	void showUnderline(double p, double w);

	void updateCharStyle(const CharStyle& charStyle);
	void updateStyle(const ParagraphStyle& newCurrent);

private slots:
	void handleBaselineOffset();
	void handleMinWordTracking();
	void handleNormWordTracking();
	void handleMinGlyphExtension();
	void handleMaxGlyphExtension();
	void handleTextScaleH();
	void handleTextScaleV();
	void handleTracking();

	void handleOutlineWidth();
	void handleShadowOffs();
	void handleStrikeThru();
	void handleTypeStyle(int s);
	void handleUnderline();

};

#endif
