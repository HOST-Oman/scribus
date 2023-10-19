/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H

#include <QWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>

class QEvent;

#include "fpointarray.h"
#include "scribusapi.h"

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QPixmap;
class QSpacerItem;

class SCRIBUS_API KCurve : public QWidget
{
	Q_OBJECT

public:
	KCurve(QWidget *parent);
	virtual ~KCurve() = default;

protected:
	void paintEvent(QPaintEvent*) override;
	void keyPressEvent(QKeyEvent*) override;
	void mousePressEvent(QMouseEvent* e) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
	void mouseMoveEvent(QMouseEvent* e) override;

public:
	FPointArray getCurve() const;
	void setCurve(const FPointArray& inlist);
	void resetCurve();
	QPainterPath curvePath() const;
	void setLinear(bool setter);
	bool isLinear() const;

signals:
	void modified();

private:
	int m_leftmost { 0 };
	int m_rightmost { 0 };
	bool m_dragging { false };
	bool m_linear { false };
	FPointArray m_points;
	FPointArray m_points_back;
	QPointF m_mousePos;
	int m_selectedPoint { 0 };

	int selectedPoint() const;
};

class SCRIBUS_API CurveWidget : public QWidget
{
	Q_OBJECT

public:
	CurveWidget(QWidget* parent);
	~CurveWidget() {};
	
	void setLinear(bool setter);

	QPushButton* invertButton { nullptr };
	QPushButton* resetButton { nullptr };
	QPushButton *linearButton { nullptr };
	QPushButton* loadButton { nullptr };
	QPushButton* saveButton { nullptr };
	KCurve* cDisplay { nullptr };

private slots:
	void doInvert();
	void doReset();
	void doLinear();
	void doLoad();
	void doSave();

protected:
	QHBoxLayout* CurveWidgetLayout { nullptr };
	QVBoxLayout* layout1 { nullptr };
	QSpacerItem* spacer1 { nullptr };

	void changeEvent(QEvent *e) override;

protected slots:
	virtual void languageChange();
};

#endif // CURVEWIDGET_H
