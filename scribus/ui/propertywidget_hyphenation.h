/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef PROPERTYWIDGET_HYPHENATION_H
#define PROPERTYWIDGET_HYPHENATION_H

#include "ui_propertywidget_hyphenationbase.h"
#include "propertywidgetbase.h"

class CharStyle;
class ParagraphStyle;
class ScribusDoc;
class ScribusMainWindow;

class PropertyWidget_Hyphenation : public QFrame, Ui::PropertyWidget_HyphenationBase,
		public PropertyWidgetBase
{
	Q_OBJECT

public:
	PropertyWidget_Hyphenation(QWidget *parent);
	~PropertyWidget_Hyphenation() {}

protected:
	void connectSignals();
	void disconnectSignals();

	void configureWidgets();
	void setCurrentItem(PageItem *item);
	void changeEvent(QEvent *e) override;

	PageItem* m_item { nullptr };
	ScribusMainWindow* m_ScMW { nullptr };

public slots:
	void setMainWindow(ScribusMainWindow *mw);
	void setDoc(ScribusDoc *d);
	void handleSelectionChanged();
	void toggleLabelVisibility(bool v);
	void iconSetChange();
	void languageChange();
	void unitChange() {};
	void updateCharStyle(const CharStyle& charStyle);
	void updateStyle(const ParagraphStyle& paraStyle);
	void showLanguage(const QString& w);
	void changeLang(int id);

private slots:
	void handleWordMin(int minWord);
	void handleConsecutiveLines(int consecutiveLines);
	void handleHyphenChar(const QString& hyphenText);
	void handleHyphenate();
	void handleDehyphenate();
};

#endif // PROPERTYWIDGET_HYPHENATION_H
