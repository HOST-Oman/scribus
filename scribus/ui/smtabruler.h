/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef SMTABRULER_H
#define SMTABRULER_H

#include "tabruler.h"

class SMScrSpinBox;


class SMTabruler : public Tabruler
{
	Q_OBJECT
public:
	SMTabruler(QWidget* parent,
			   bool haveFirst = true,
			   int dEin = 1,
			   QList<ParagraphStyle::TabRecord> Tabs = QList<ParagraphStyle::TabRecord>(),
			   double wid = -1);
	~SMTabruler() {};

	void unitChange(int unitIndex);

	void setTabs(const QList<ParagraphStyle::TabRecord>& Tabs, int unitIndex);
	void setTabs(const QList<ParagraphStyle::TabRecord>& Tabs, int unitIndex, bool isParentValue);
	void setParentTabs(const QList<ParagraphStyle::TabRecord>& Tabs);

	void setFirstLineValue(double t);
	void setFirstLineValue(double t, bool isParentValue);
	void setParentFirstLine(double t);

	void setLeftIndentValue(double t);
	void setLeftIndentValue(double t, bool isParentValue);
	void setParentLeftIndent(double t);

	void setRightIndentValue(double t);
	void setRightIndentValue(double t, bool isParentValue);
	void setParentRightIndent(double t);

	bool useParentTabs();
	bool useParentFirstLine();
	bool useParentLeftIndent();
	bool useParentRightIndent();

	SMScrSpinBox *first_;
	SMScrSpinBox *left_;
	SMScrSpinBox *right_;

private:
	QList<ParagraphStyle::TabRecord> m_pTabs;
	int  m_unitIndex;
	QToolButton *m_parentButton;
	bool m_hasParent;
	bool m_tabsChanged;
	bool m_useParentTabs;
	bool m_isSetupRight;
	bool m_isSetupLeft;
	bool m_isSetupFirst;

private slots:
	void slotTabsChanged();
	void pbClicked();
	void firstDataChanged();
	void leftDataChanged();
	void rightDataChanged();
	void firstValueChanged();
	void leftValueChanged();
	void rightValueChanged();
};

#endif

