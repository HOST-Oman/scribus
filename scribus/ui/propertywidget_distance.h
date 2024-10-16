/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef PROPERTYWIDGET_DISTANCE_H
#define PROPERTYWIDGET_DISTANCE_H

#include "ui_propertywidget_distancebase.h"

#include "propertywidgetbase.h"
#include "units.h"

class ScribusDoc;
class ScribusMainWindow;

class PropertyWidget_Distance : public QFrame, public Ui::PropertyWidget_DistanceBase,
                                public PropertyWidgetBase
{
	Q_OBJECT

public:
	PropertyWidget_Distance(QWidget* parent);
	~PropertyWidget_Distance() {};

protected:
	void connectSignals();
	void disconnectSignals();

	double m_unitRatio {1.0};
	int m_unitIndex {SC_PT};
	PageItem *m_item {nullptr};
	ScribusMainWindow* m_ScMW {nullptr};

	void configureWidgets();
	void setCurrentItem(PageItem *item);

	void changeEvent(QEvent *e) override;

public slots:
	void setMainWindow(ScribusMainWindow *mw);
	void setDoc(ScribusDoc *d);

	void handleAppModeChanged(int oldMode, int mode);
	void handleSelectionChanged();
	void handleUpdateRequest(int);

	void iconSetChange();
	void languageChange();
	void unitChange();
	void localeChange();
	void toggleLabelVisibility(bool v);

	void showColumns(int r, double g);

private slots:
	void handleColumns();
	void handleColumnGap();
	void handleGapSwitch();
	void handleTabs();
	void handleTextDistances();
};

#endif
