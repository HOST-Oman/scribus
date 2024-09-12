/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

#include "commonstrings.h"
#include "iconmanager.h"
#include "newmarginwidget.h"
#include "pagepropertiesdialog.h"
#include "pagesize.h"
#include "pagestructs.h"
#include "scpage.h"
#include "scribusdoc.h"
#include "scrspinbox.h"
#include "units.h"
#include "widgets/pagesizeselector.h"

PagePropertiesDialog::PagePropertiesDialog( QWidget* parent, ScribusDoc* doc )
                    : QDialog( parent),
                      m_unitRatio(doc->unitRatio())
{
	setModal(true);
	setWindowTitle( tr( "Manage Page Properties" ) );
	setWindowIcon(IconManager::instance().loadIcon("AppIcon.png"));
	dialogLayout = new QVBoxLayout(this);
	dialogLayout->setContentsMargins(9, 9, 9, 9);
	dialogLayout->setSpacing(4);
	
	PageSize ps(doc->currentPage()->size());

	// try to find coresponding page size by dimensions
	if (ps.name() == CommonStrings::customPageSize)
	{
		PageSizeInfoMap pages = ps.sizesByDimensions(QSize(doc->currentPage()->width(), doc->currentPage()->height()));
		if (pages.count() > 0)
			prefsPageSizeName = pages.firstKey();
	}
	else
		prefsPageSizeName = ps.name();

	dsGroupBox7 = new QGroupBox(this);
	dsGroupBox7->setTitle( tr( "Page Size" ) );
	dsGroupBox7Layout = new QGridLayout(dsGroupBox7);
	dsGroupBox7Layout->setAlignment( Qt::AlignTop );
	dsGroupBox7Layout->setSpacing(4);
	dsGroupBox7Layout->setContentsMargins(9, 9, 9, 9);
	TextLabel1 = new QLabel( tr( "&Size:" ), dsGroupBox7 );
	dsGroupBox7Layout->addWidget( TextLabel1, 0, 0, Qt::AlignTop | Qt::AlignRight);
	pageSizeSelector = new PageSizeSelector(dsGroupBox7);
	pageSizeSelector->setPageSize(doc->currentPage()->size());
	TextLabel1->setBuddy(pageSizeSelector);
	dsGroupBox7Layout->addWidget(pageSizeSelector, 0, 1);
	TextLabel2 = new QLabel( tr( "Orie&ntation:" ), dsGroupBox7 );
	dsGroupBox7Layout->addWidget( TextLabel2, 1, 0, Qt::AlignRight);
	orientationQComboBox = new QComboBox( dsGroupBox7 );
	orientationQComboBox->addItem( tr( "Portrait" ) );
	orientationQComboBox->addItem( tr( "Landscape" ) );
	orientationQComboBox->setEditable(false);
	orientationQComboBox->setCurrentIndex(doc->currentPage()->orientation() );
	oldOri = doc->currentPage()->orientation();
	TextLabel2->setBuddy(orientationQComboBox);
	dsGroupBox7Layout->addWidget( orientationQComboBox, 1, 1 );
	widthSpinBox = new ScrSpinBox(pts2value(1.0, doc->unitIndex()), 16777215, dsGroupBox7, doc->unitIndex());
	widthQLabel = new QLabel( tr( "&Width:" ), dsGroupBox7 );
	widthSpinBox->setValue(doc->currentPage()->width() * doc->unitRatio());
	widthQLabel->setBuddy(widthSpinBox);
	dsGroupBox7Layout->addWidget( widthQLabel, 2, 0, Qt::AlignRight);
	dsGroupBox7Layout->addWidget( widthSpinBox, 2, 1 );
	heightSpinBox = new ScrSpinBox(pts2value(1.0, doc->unitIndex()), 16777215, dsGroupBox7, doc->unitIndex());
	heightSpinBox->setValue(doc->currentPage()->height() * doc->unitRatio());
	heightQLabel = new QLabel( tr( "&Height:" ), dsGroupBox7 );
	heightQLabel->setBuddy(heightSpinBox);
	dsGroupBox7Layout->addWidget( heightQLabel, 3, 0, Qt::AlignRight );
	dsGroupBox7Layout->addWidget( heightSpinBox, 3, 1 );
	moveObjects = new QCheckBox( dsGroupBox7 );
	moveObjects->setText( tr( "Move Objects with their Page" ) );
	moveObjects->setChecked( true );
	dsGroupBox7Layout->addWidget( moveObjects, 4, 0, 1, 2 );
	Links = nullptr;
	if ((doc->pagePositioning() != singlePage) && (doc->masterPageMode()))
	{
		TextLabel3 = new QLabel( tr( "Type:" ), dsGroupBox7 );
		dsGroupBox7Layout->addWidget( TextLabel3, 5, 0, Qt::AlignRight);
		Links = new QComboBox( dsGroupBox7 );
		QList<PageSet> pageSet(doc->pageSets());
		const QStringList& pageNames = pageSet.at(doc->pagePositioning()).pageNames;
		for (auto pNames = pageNames.begin(); pNames != pageNames.end(); ++pNames )
		{
			Links->addItem(CommonStrings::translatePageSetLocString((*pNames)));
		}
		Links->setEditable(false);
		dsGroupBox7Layout->addWidget( Links, 5, 1);
		if (doc->currentPage()->LeftPg == 0)
			Links->setCurrentIndex(Links->count()-1);
		else if (doc->currentPage()->LeftPg == 1)
			Links->setCurrentIndex(0);
		else
			Links->setCurrentIndex(doc->currentPage()->LeftPg-1);
	}
	dialogLayout->addWidget( dsGroupBox7 );
	
//	marginWidget = new MarginWidget(this,  tr( "Margin Guides" ), &doc->currentPage()->initialMargins, doc->unitIndex(), false, false);
	marginWidget = new NewMarginWidget();
	marginWidget->setup(doc->currentPage()->initialMargins, doc->currentPage()->marginPreset, doc->unitIndex(), NewMarginWidget::MarginWidgetFlags );
	marginWidget->setPageHeight(doc->currentPage()->height());
	marginWidget->setPageWidth(doc->currentPage()->width());
	marginWidget->setFacingPages(!(doc->pagePositioning() == singlePage), doc->locationOfPage(doc->currentPage()->pageNr()));
//	marginWidget->setMarginPreset(doc->currentPage()->marginPreset);
	dialogLayout->addWidget( marginWidget );

	groupMaster = new QGroupBox( this );
	groupMaster->setTitle( tr( "Other Settings" ) );
	masterLayout = new QHBoxLayout( groupMaster );
	masterLayout->setSpacing(4);
	masterLayout->setContentsMargins(8, 8, 8, 8);
	masterPageLabel = new QLabel( groupMaster );
	masterLayout->addWidget( masterPageLabel );
	if (!doc->masterPageMode())
	{
		masterPageLabel->setText( tr( "Master Page:" ) );
		masterPageComboBox = new QComboBox( groupMaster );
		QString Nam = doc->currentPage()->masterPageName();
		QString na = Nam == CommonStrings::masterPageNormal ? CommonStrings::trMasterPageNormal : Nam, in;
		int cc = 0;
		for (QMap<QString,int>::Iterator it = doc->MasterNames.begin(); it != doc->MasterNames.end(); ++it)
		{
			in = it.key() == CommonStrings::masterPageNormal ? CommonStrings::trMasterPageNormal : it.key();
			masterPageComboBox->addItem(in);
			if (in == na)
				masterPageComboBox->setCurrentIndex(cc);
			++cc;
		}
		masterLayout->addWidget( masterPageComboBox );
	}
	dialogLayout->addWidget( groupMaster );
	if (doc->masterPageMode())
		groupMaster->hide();

	okCancelLayout = new QHBoxLayout;
	okCancelLayout->setSpacing(6);
	okCancelLayout->setContentsMargins(0, 0, 0, 0);
	QSpacerItem* spacer = new QSpacerItem( 2, 2, QSizePolicy::Expanding, QSizePolicy::Minimum );
	okCancelLayout->addItem( spacer );
	okButton = new QPushButton( CommonStrings::tr_OK, this );
	okButton->setDefault( true );
	okCancelLayout->addWidget(okButton);
	cancelButton = new QPushButton( CommonStrings::tr_Cancel, this );
	cancelButton->setDefault( false );
	okCancelLayout->addWidget(cancelButton);
	dialogLayout->addLayout( okCancelLayout );
	setMaximumSize(sizeHint());

	m_pageWidth = widthSpinBox->value() / m_unitRatio;
	m_pageHeight = heightSpinBox->value() / m_unitRatio;

	bool isCustom = (pageSizeSelector->pageSizeTR() == CommonStrings::trCustomPageSize);
	heightSpinBox->setEnabled(isCustom);
	widthSpinBox->setEnabled(isCustom);
	// signals and slots connections
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(orientationQComboBox, SIGNAL(activated(int)), this, SLOT(setOrientation(int)));
	connect(pageSizeSelector, SIGNAL(pageSizeChanged(QString)), this, SLOT(setPageSize()));
	connect(widthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setPageWidth(double)));
	connect(heightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setPageHeight(double)));
	
	//tooltips
	pageSizeSelector->setToolTip( tr( "Size of the inserted pages, either a standard or custom size" ) );
	orientationQComboBox->setToolTip( tr( "Orientation of the page(s) to be inserted" ) );
	widthSpinBox->setToolTip( tr( "Width of the page(s) to be inserted" ) );
	heightSpinBox->setToolTip( tr( "Height of the page(s) to be inserted" ) );
	moveObjects->setToolTip( tr( "When inserting a new page between others, move objects with their current pages. This is the default action." ) );
	
	setPageSize();
	setMinimumSize(minimumSizeHint());
	setMaximumSize(minimumSizeHint());
	resize(minimumSizeHint());
}

void PagePropertiesDialog::setPageWidth(double)
{
	m_pageWidth = widthSpinBox->value() / m_unitRatio;
	marginWidget->setPageWidth(m_pageWidth);
	int newOrientation = (widthSpinBox->value() > heightSpinBox->value()) ? landscapePage : portraitPage;
	if (newOrientation != orientationQComboBox->currentIndex())
	{
		orientationQComboBox->blockSignals(true);
		orientationQComboBox->setCurrentIndex(newOrientation);
		orientationQComboBox->blockSignals(false);
		oldOri = newOrientation;
	}
}

void PagePropertiesDialog::setPageHeight(double)
{
	m_pageHeight = heightSpinBox->value() / m_unitRatio;
	marginWidget->setPageHeight(m_pageHeight);
	int newOrientation = (widthSpinBox->value() > heightSpinBox->value()) ? landscapePage : portraitPage;
	if (newOrientation != orientationQComboBox->currentIndex())
	{
		orientationQComboBox->blockSignals(true);
		orientationQComboBox->setCurrentIndex(newOrientation);
		orientationQComboBox->blockSignals(false);
		oldOri = newOrientation;
	}
}

void PagePropertiesDialog::setPageSize()
{
	if (pageSizeSelector->pageSizeTR() != CommonStrings::trCustomPageSize)
		oldOri++;
	setOrientation(orientationQComboBox->currentIndex());
}

void PagePropertiesDialog::setSize(const QString & gr)
{
	m_pageWidth = widthSpinBox->value() / m_unitRatio;
	m_pageHeight = heightSpinBox->value() / m_unitRatio;
	widthSpinBox->setEnabled(false);
	heightSpinBox->setEnabled(false);
	PageSize ps2(gr);
	prefsPageSizeName = ps2.name();
	if (gr == CommonStrings::trCustomPageSize)
	{
		widthSpinBox->setEnabled(true);
		heightSpinBox->setEnabled(true);
		prefsPageSizeName = CommonStrings::customPageSize;
	}
	else
	{
		m_pageWidth = ps2.width();
		m_pageHeight = ps2.height();
	}
	disconnect(widthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setPageWidth(double)));
	disconnect(heightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setPageHeight(double)));
	widthSpinBox->setValue(m_pageWidth * m_unitRatio);
	heightSpinBox->setValue(m_pageHeight * m_unitRatio);
	marginWidget->setPageHeight(m_pageHeight);
	marginWidget->setPageWidth(m_pageWidth);
	marginWidget->setPageSize(gr);
	connect(widthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setPageWidth(double)));
	connect(heightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setPageHeight(double)));
}

void PagePropertiesDialog::setOrientation(int ori)
{
	setSize(pageSizeSelector->pageSizeTR());
	disconnect(widthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setPageWidth(double)));
	disconnect(heightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setPageHeight(double)));
	if ((pageSizeSelector->pageSizeTR() == CommonStrings::trCustomPageSize) && (ori != oldOri))
	{
		double w = widthSpinBox->value(), h = heightSpinBox->value();
		widthSpinBox->setValue((ori == portraitPage) ? qMin(w, h) : qMax(w, h));
		heightSpinBox->setValue((ori == portraitPage) ? qMax(w, h) : qMin(w, h));
	}
	else
	{
		if ((ori != portraitPage) && (ori != oldOri))
		{
			double w = widthSpinBox->value(), h = heightSpinBox->value();
			widthSpinBox->setValue(qMax(w, h));
			heightSpinBox->setValue(qMin(w, h));
		}
	}
	oldOri = ori;
	m_pageWidth = widthSpinBox->value() / m_unitRatio;
	m_pageHeight = heightSpinBox->value() / m_unitRatio;
	connect(widthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setPageWidth(double)));
	connect(heightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setPageHeight(double)));
}

int PagePropertiesDialog::pageOrder() const
{
	int lp=0;
	if (Links != nullptr)
		lp = Links->currentIndex();
	if (lp == 0)
		lp = 1;
	else if (lp == static_cast<int>(Links->count() - 1))
		lp = 0;
	else
		lp++;
	return lp;
}

double PagePropertiesDialog::getPageWidth() const
{
	return m_pageWidth;
}

double PagePropertiesDialog::getPageHeight() const
{
	return m_pageHeight;
}

int PagePropertiesDialog::getPageOrientation() const
{
	return orientationQComboBox->currentIndex();
}

QString PagePropertiesDialog::getPrefsPageSizeName() const
{
	return prefsPageSizeName;
}

bool PagePropertiesDialog::getMoveObjects() const
{
	return moveObjects->isChecked();
}

double PagePropertiesDialog::top() const
{
	return marginWidget->margins().top();
}

double PagePropertiesDialog::bottom() const
{
	return marginWidget->margins().bottom();
}

double PagePropertiesDialog::left() const
{
	return marginWidget->margins().left();
}

double PagePropertiesDialog::right() const
{
	return marginWidget->margins().right();
}

QString PagePropertiesDialog::masterPage() const
{
	if (masterPageComboBox != nullptr)
		return masterPageComboBox->currentText();
	return QString();
}

int PagePropertiesDialog::getMarginPreset() const
{
	return marginWidget->marginPreset();
}
