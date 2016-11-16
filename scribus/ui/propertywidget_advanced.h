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
#include "text/glyphcluster.h"

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

	double m_unitRatio;
	int    m_unitIndex;

	PageItem*          m_item;
	ScribusMainWindow* m_ScMW;

	void configureWidgets(void);
	void setCurrentItem(PageItem *i);

	virtual void changeEvent(QEvent *e);

public slots:
	void setMainWindow(ScribusMainWindow *mw);
	void setDoc(ScribusDoc *d);

	void handleSelectionChanged();

	void languageChange();
	void unitChange() {};

	void showBaseLineOffset(double e);
	void showTextScaleH(double e);
	void showTextScaleV(double e);
	void showTracking(double e);
	void showFontFallBack(const QString& font);
	void showFontFallBackSize(double s);

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
	void handleFontFallBack(const QString &font);
	void handleFontFallBackSize(double s);
private:
	QList<GlyphCluster> m_missingfaceslist;
};

#endif
