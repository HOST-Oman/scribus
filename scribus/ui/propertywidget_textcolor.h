/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef PROPERTYWIDGET_TEXTCOLOR_H
#define PROPERTYWIDGET_TEXTCOLOR_H

#include "ui_propertywidget_textcolorbase.h"

#include "propertywidgetbase.h"

class ScribusDoc;
class ScribusMainWindow;

class PropertyWidget_TextColor : public QFrame, Ui::PropertyWidget_TextColorBase,
	                             public PropertyWidgetBase
{
	Q_OBJECT

public:
	PropertyWidget_TextColor(QWidget* parent);
	~PropertyWidget_TextColor() {};

	void updateColorList();
	void setCurrentItem(PageItem *i);

protected:
	PageItem *         m_item;
	ScribusMainWindow* m_ScMW;

	void connectSignals();
	void disconnectSignals();

	void configureWidgets();

	virtual void changeEvent(QEvent *e);

public slots:
	void setMainWindow(ScribusMainWindow *mw);
	void setDoc(ScribusDoc *d);

	void handleSelectionChanged();
	void handleUpdateRequest(int);

	void languageChange();
	void unitChange() {};

	void showOutlineW(double x);
	void showShadowOffset(double x, double y);
	void showStrikeThru(double p, double w);
	void showTextColors(const QString& p, const QString& b, const QString& bc, double shp, double shb, double sbc);
	void showTextEffects(int s);
	void showUnderline(double p, double w);

	void updateCharStyle(const CharStyle& charStyle);
	void updateStyle(const ParagraphStyle& newCurrent);

private slots:
	void handleOutlineWidth();
	void handleShadowOffs();
	void handleStrikeThru();
	void handleTextFill();
	void handleTextShade();
	void handleTextStroke();
	void handleTextBackground();
	void handleTypeStyle(int s);
	void handleUnderline();
};

#endif
