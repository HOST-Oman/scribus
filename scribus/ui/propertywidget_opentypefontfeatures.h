#ifndef PROPERTYWIDGET_OPENTYPEFONTFEATURES_H
#define PROPERTYWIDGET_OPENTYPEFONTFEATURES_H

#include "ui_propertywidget_opentypefontfeatures.h"
#include "propertywidgetbase.h"
class CharStyle;
class ParagraphStyle;
class ScribusDoc;
class ScribusMainWindow;

class PropertyWidget_OpenTypeFontFeatures : public QFrame, Ui::PropertyWidget_OpenTypeFontFeatures,
                                            public PropertyWidgetBase
{
    Q_OBJECT

public:
    PropertyWidget_OpenTypeFontFeatures(QWidget *parent);
    ~PropertyWidget_OpenTypeFontFeatures() {};

protected:
        void connectSignals();
        void disconnectSignals();
        PageItem*          m_item;
        ScribusMainWindow* m_ScMW;
        double m_unitRatio;
        int    m_unitIndex;

        void configureWidgets(void);
        void setCurrentItem(PageItem *item);
        virtual void changeEvent(QEvent *e);
public slots:
		void setMainWindow(ScribusMainWindow *mw);
		void setDoc(ScribusDoc *d);
		void handleSelectionChanged();
        void languageChange();
		void unitChange() {};
        void showFontFeatures(QString s);
		void updateCharStyle(const CharStyle& charStyle);
		void updateStyle(const ParagraphStyle& newCurrent);

private slots:
        void handlefontfeatures();


};

#endif
