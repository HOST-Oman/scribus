#ifndef DIRECTIONSELECT_H
#define DIRECTIONSELECT_H

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QButtonGroup>

class QEvent;

#include "scribusapi.h"

class SCRIBUS_API DirectionSelect : public QWidget
{
	Q_OBJECT
public:
        DirectionSelect(QWidget* parent);
        ~DirectionSelect() {};
        void setStyle(int s);
        int getStyle();
        int selectedId();

        virtual void changeEvent(QEvent *e);

        QButtonGroup* buttonGroup;
        int selected;
        QToolButton* RTL;
        QToolButton* LTR;


public slots:
        void languageChange();

private slots:
        void setTypeStyle(int a);

signals:
        void State(int);

protected:
        QHBoxLayout* GroupSelectLayout;
};

#endif // DIRECTIONSELECT_H
