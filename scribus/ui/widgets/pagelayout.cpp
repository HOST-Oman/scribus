/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include "pagelayout.h"

#include <QComboBox>
#include <QToolButton>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QLabel>
#include <QPixmap>
#include <QList>

#include "commonstrings.h"
#include "iconmanager.h"
#include "scribusapp.h"
#include "ui/widgets/form_widget.h"

PageLayouts::PageLayouts(QWidget* parent)  : QWidget( parent )
{
	struct PageSet pageS;
	pageS.Name = CommonStrings::trPageSet2;
	pageS.FirstPage = 0;
	pageS.Rows = 1;
	pageS.Columns = 1;
	pageS.pageNames.clear();
	pageS.pageNames.append(CommonStrings::trPageLocMiddleRight);
	m_pageSets.append(pageS);

	layoutGroupLayout = new QHBoxLayout(this);
	layoutGroupLayout->setSpacing(4);
	layoutGroupLayout->setContentsMargins(0, 0, 0, 0);

	QFont layFont(this->font());
	layFont.setPointSize(8);

	menuScheme = new QMenu();

	buttonScheme = new QToolButton(this);
	buttonScheme->setPopupMode(QToolButton::InstantPopup);
	buttonScheme->setMenu(menuScheme);

	labelScheme = new FormWidget();
	labelScheme->setFont(layFont);
	labelScheme->addWidget(buttonScheme);
	labelScheme->setLabelVisibility(!m_hideLabels);
	layoutGroupLayout->addWidget( labelScheme );

	menuFirstPage = new QMenu();

	buttonFirstPage = new QToolButton(this);
	buttonFirstPage->setPopupMode(QToolButton::InstantPopup);
	buttonFirstPage->setMenu(menuFirstPage);

	labelPages = new FormWidget();
	labelPages->setFont(layFont);
	labelPages->addWidget( buttonFirstPage );
	labelPages->setLabelVisibility(!m_hideLabels);
	layoutGroupLayout->addWidget( labelPages );

	languageChange();

	connect(ScQApp, SIGNAL(labelVisibilityChanged(bool)), this, SLOT(toggleLabelVisibility(bool)));
	connect(menuScheme, &QMenu::triggered, this, &PageLayouts::changeScheme);
	connect(menuFirstPage, &QMenu::triggered, this, &PageLayouts::changeFirstPage);
}

void PageLayouts::updateSchemeSelector(QList<PageSet> pageSets, int pagePositioning)
{
	m_pageSets = pageSets;
	docPagePositioning = pagePositioning;
	labelPages->setVisible( docPagePositioning > 0 );

	reloadScheme();
}

void PageLayouts::setFirstPage(int nr)
{
	if (menuFirstPage->actions().isEmpty())
		return;

	m_firstPage = qBound(0, nr, static_cast<int>(menuFirstPage->actions().count()));
	buttonFirstPage->setIcon(menuFirstPage->actions().at(m_firstPage)->icon());
}

void PageLayouts::setScheme(int nr)
{
	if (menuScheme->actions().isEmpty())
		return;

	m_scheme = qBound(0, nr, static_cast<int>(menuScheme->actions().count()));
	buttonScheme->setIcon(menuScheme->actions().at(m_scheme)->icon());

	reloadFirstPage(m_scheme);

}

void PageLayouts::setHideLabelsPermanently(bool hide)
{
	m_hideLabels = hide;
	toggleLabelVisibility(!hide);
}

void PageLayouts::reloadScheme()
{
	disconnect(menuScheme, &QMenu::triggered, this, &PageLayouts::changeScheme);

	menuScheme->clear();

	for (int pg = 0; pg < m_pageSets.count(); ++pg)
	{
		//Leave the code in for 3-/4- fold but exclude from UI in case we bring it back.
		if (pg > 1 && docPagePositioning < 2)
			continue;

		QString psname = CommonStrings::translatePageSetString(m_pageSets[pg].Name);
		if (pg == 0)
			menuScheme->addAction(IconManager::instance().loadIcon("16/page-simple.png"), psname)->setData(QVariant(pg));
		else if (pg == 1)
			menuScheme->addAction(IconManager::instance().loadIcon("16/page-doublesided.png"), psname)->setData(QVariant(pg));
		else if (pg == 2)
			menuScheme->addAction(IconManager::instance().loadIcon("16/page-3fold.png"), psname)->setData(QVariant(pg));
		else if (pg == 3)
			menuScheme->addAction(IconManager::instance().loadIcon("16/page-4fold.png"), psname)->setData(QVariant(pg));
		else
			menuScheme->addAction(IconManager::instance().loadIcon("16/page-simple.png"), psname)->setData(QVariant(pg));
	}

	connect(menuScheme, &QMenu::triggered, this, &PageLayouts::changeScheme);
}

void PageLayouts::reloadFirstPage(int scheme)
{
	disconnect(menuFirstPage, &QMenu::triggered, this, &PageLayouts::changeFirstPage);

	menuFirstPage->clear();

	// We have to add the other cases if want to support them again
	// CommonStrings::pageLocLeft
	// CommonStrings::pageLocMiddle
	// CommonStrings::pageLocMiddleLeft
	// CommonStrings::pageLocMiddleRight
	// CommonStrings::pageLocRight

	for (int pg = 0; pg < m_pageSets[scheme].pageNames.count(); ++pg)
	{
		QString psname = m_pageSets[scheme].pageNames[pg];

		if (psname == CommonStrings::pageLocLeft)
			menuFirstPage->addAction(IconManager::instance().loadIcon("page-first-left"), psname)->setData(QVariant(pg));
		else if (psname == CommonStrings::pageLocRight)
			menuFirstPage->addAction(IconManager::instance().loadIcon("page-first-right"), psname)->setData(QVariant(pg));
		else
			menuFirstPage->addAction(IconManager::instance().loadIcon("page-first-left"), psname)->setData(QVariant(pg));
	}

	connect(menuFirstPage, &QMenu::triggered, this, &PageLayouts::changeFirstPage);
}

void PageLayouts::toggleLabelVisibility(bool visibility)
{
	// hide labels always if flag is false
	if (m_hideLabels)
		visibility = false;

	labelScheme->setLabelVisibility(visibility);
	labelPages->setLabelVisibility(visibility);
}

void PageLayouts::languageChange()
{

	reloadScheme();
	reloadFirstPage(m_scheme);

	labelScheme->setText( tr( "Scheme" ) );
	labelPages->setText( tr( "First Page" ) );

	buttonScheme->setToolTip( tr( "Number of pages to show side-by-side on the canvas. Often used for allowing items to be placed across page spreads." ) );
	buttonFirstPage->setToolTip( tr( "Location on the canvas where the first page of the document is placed" ) );
}

void PageLayouts::changeScheme(QAction *action)
{
	buttonScheme->setIcon(action->icon());
	int ic = action->data().toInt();
	reloadFirstPage(ic);
	labelPages->setVisible( ic > 0 );
	emit schemeChanged(ic);
}

void PageLayouts::changeFirstPage(QAction *action)
{
	buttonFirstPage->setIcon(action->icon());
	m_firstPage = action->data().toInt();
	emit firstPageChanged(m_firstPage);
}
