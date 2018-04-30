/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include "inspage.h"

#include <QLabel>
#include <QDialog>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QPushButton>

#include "commonstrings.h"
#include "iconmanager.h"
#include "pagesize.h"
#include "scpage.h"
#include "scribusdoc.h"
#include "scrspinbox.h"
#include "units.h"
#include "util.h"

InsPage::InsPage( QWidget* parent, ScribusDoc* currentDoc, int currentPage, int maxPages)
		: QDialog( parent, 0 )
{
	masterPageCombos.clear();
	setModal(true);
	setWindowTitle( tr( "Insert Page" ) );
	setWindowIcon(IconManager::instance()->loadIcon("AppIcon.png"));
	dialogLayout = new QVBoxLayout(this);
	dialogLayout->setSpacing( 5 );
	dialogLayout->setMargin( 5 );
	whereLayout = new QGridLayout();
	whereLayout->setSpacing( 5 );
	whereLayout->setMargin( 5 );
	insCountData = new QSpinBox( this );
	insCountData->setMinimum(1);
	insCountData->setMaximum(999);
	insCountData->setValue( 1 );
	insCountLabel = new QLabel(tr( "&Insert" ), this );
	insCountLabel->setBuddy(insCountData);
	whereLayout->addWidget( insCountLabel, 0, 0 );
	whereLayout->addWidget( insCountData, 0, 1 );
	pagesLabel = new QLabel( tr( "Page(s)" ), this);
	whereLayout->addWidget( pagesLabel, 0, 2 );

	insWhereData = new QComboBox( this );
	insWhereData->addItem( tr("before Page"));
	insWhereData->addItem( tr("after Page"));
	insWhereData->addItem( tr("at End"));
	insWhereData->setCurrentIndex(2);
	whereLayout->addWidget( insWhereData, 1, 0, 1, 2 );

	insWherePageData = new QSpinBox(this);
	insWherePageData->setMinimum(1);
	insWherePageData->setMaximum(maxPages);
	insWherePageData->setValue( currentPage+1 );
	insWherePageData->setDisabled( true );

	whereLayout->addWidget( insWherePageData, 1, 2 );
	whereLayout->addItem(new QSpacerItem(insCountLabel->fontMetrics().width( tr( "&Insert" )), 0), 0, 0);
	dialogLayout->addLayout( whereLayout );
	
	masterPageLabel = 0;
	masterPageGroup = new QGroupBox( this);
	masterPageGroup->setTitle( tr( "Master Pages" ) );
	masterPageLayout = new QGridLayout( masterPageGroup );
	masterPageLayout->setAlignment( Qt::AlignTop );
	masterPageLayout->setSpacing( 5 );
	masterPageLayout->setMargin( 5 );
	if (currentDoc->pagePositioning() == 0)
	{
		QComboBox* pageData = new QComboBox(masterPageGroup);
		for (QMap<QString,int>::Iterator it = currentDoc->MasterNames.begin(); it != currentDoc->MasterNames.end(); ++it)
		{
			pageData->addItem(it.key() == CommonStrings::masterPageNormal ? CommonStrings::trMasterPageNormal : it.key(), it.key());
		}
		if (currentDoc->MasterNames.contains( CommonStrings::trMasterPageNormal))
			setCurrentComboItem(pageData, CommonStrings::trMasterPageNormal);
		masterPageLabel = new QLabel(tr("&Master Page:"), masterPageGroup);
		masterPageLabel->setBuddy(pageData);
		masterPageLayout->addWidget( masterPageLabel, 0, 0 );
		masterPageLayout->addWidget(pageData, 0, 1);
		masterPageCombos.append(pageData);
	}
	else
	{
		int row = 0;
		for (int mp = 0; mp < currentDoc->pageSets()[currentDoc->pagePositioning()].pageNames.count(); ++mp)
		{
			QComboBox* pageData = new QComboBox(masterPageGroup);
//			for (QMap<QString,int>::Iterator it = currentDoc->MasterNames.begin(); it != currentDoc->MasterNames.end(); ++it)
//				pageData->insertItem(it.key() == CommonStrings::masterPageNormal ? CommonStrings::trMasterPageNormal : it.key());
			if (mp == 0)
			{
				bool conNam = currentDoc->MasterNames.contains( CommonStrings::trMasterPageNormalLeft);
				for (QMap<QString,int>::Iterator it = currentDoc->MasterNames.begin(); it != currentDoc->MasterNames.end(); ++it)
				{
					if ((it.key() == CommonStrings::masterPageNormal) && (!conNam))
						pageData->addItem(CommonStrings::trMasterPageNormal, it.key());
					else if ((it.key() == CommonStrings::trMasterPageNormal) && (!conNam))
						pageData->addItem(CommonStrings::trMasterPageNormal, it.key());
					else
					{
						if (currentDoc->MasterPages.at(it.value())->LeftPg == 1)
							pageData->addItem(it.key(), it.key());
					}
				}
				if (currentDoc->MasterNames.contains( CommonStrings::trMasterPageNormalLeft))
					setCurrentComboItem(pageData, CommonStrings::trMasterPageNormalLeft);
			}
			else if (mp == 1)
			{
				if (currentDoc->pageSets()[currentDoc->pagePositioning()].pageNames.count() > 2)
				{
					bool conNam = currentDoc->MasterNames.contains( CommonStrings::trMasterPageNormalMiddle);
					for (QMap<QString,int>::Iterator it = currentDoc->MasterNames.begin(); it != currentDoc->MasterNames.end(); ++it)
					{
						if ((it.key() == CommonStrings::masterPageNormal) && (!conNam))
							pageData->addItem(CommonStrings::trMasterPageNormal, it.key());
						else if ((it.key() == CommonStrings::trMasterPageNormal) && (!conNam))
							pageData->addItem(CommonStrings::trMasterPageNormal, it.key());
						else
						{
							if ((currentDoc->MasterPages.at(it.value())->LeftPg != 0) && (currentDoc->MasterPages.at(it.value())->LeftPg != 1))
								pageData->addItem(it.key(), it.key());
						}
					}
					if (currentDoc->MasterNames.contains( CommonStrings::trMasterPageNormalMiddle))
						setCurrentComboItem(pageData, CommonStrings::trMasterPageNormalMiddle);
				}
				else
				{
					bool conNam = currentDoc->MasterNames.contains( CommonStrings::trMasterPageNormalRight);
					for (QMap<QString,int>::Iterator it = currentDoc->MasterNames.begin(); it != currentDoc->MasterNames.end(); ++it)
					{
						if ((it.key() == CommonStrings::masterPageNormal) && (!conNam))
							pageData->addItem(CommonStrings::trMasterPageNormal, it.key());
						else if ((it.key() == CommonStrings::trMasterPageNormal) && (!conNam))
							pageData->addItem(CommonStrings::trMasterPageNormal, it.key());
						else
						{
							if (currentDoc->MasterPages.at(it.value())->LeftPg == 0)
								pageData->addItem(it.key(), it.key());
						}
					}
					if (currentDoc->MasterNames.contains( CommonStrings::trMasterPageNormalRight))
						setCurrentComboItem(pageData, CommonStrings::trMasterPageNormalRight);
				}
			}
			else if (mp == 2)
			{
				if (currentDoc->pageSets()[currentDoc->pagePositioning()].pageNames.count() > 3)
				{
					bool conNam = currentDoc->MasterNames.contains( CommonStrings::trMasterPageNormalMiddle);
					for (QMap<QString,int>::Iterator it = currentDoc->MasterNames.begin(); it != currentDoc->MasterNames.end(); ++it)
					{
						if ((it.key() == CommonStrings::masterPageNormal) && (!conNam))
							pageData->addItem(CommonStrings::trMasterPageNormal, it.key());
						else if ((it.key() == CommonStrings::trMasterPageNormal) && (!conNam))
							pageData->addItem(CommonStrings::trMasterPageNormal, it.key());
						else
						{
							if ((currentDoc->MasterPages.at(it.value())->LeftPg != 0) && (currentDoc->MasterPages.at(it.value())->LeftPg != 1))
								pageData->addItem(it.key(), it.key());
						}
					}
					if (currentDoc->MasterNames.contains( CommonStrings::trMasterPageNormalMiddle))
						setCurrentComboItem(pageData, CommonStrings::trMasterPageNormalMiddle);
				}
				else
				{
					bool conNam = currentDoc->MasterNames.contains( CommonStrings::trMasterPageNormalRight);
					for (QMap<QString,int>::Iterator it = currentDoc->MasterNames.begin(); it != currentDoc->MasterNames.end(); ++it)
					{
						if ((it.key() == CommonStrings::masterPageNormal) && (!conNam))
							pageData->addItem(CommonStrings::trMasterPageNormal, it.key());
						else if ((it.key() == CommonStrings::trMasterPageNormal) && (!conNam))
							pageData->addItem(CommonStrings::trMasterPageNormal, it.key());
						else
						{
							if (currentDoc->MasterPages.at(it.value())->LeftPg == 0)
								pageData->addItem(it.key(), it.key());
						}
					}
					if (currentDoc->MasterNames.contains( CommonStrings::trMasterPageNormalRight))
						setCurrentComboItem(pageData, CommonStrings::trMasterPageNormalRight);
				}
			}
			else if (mp == 3)
			{
				bool conNam = currentDoc->MasterNames.contains( CommonStrings::trMasterPageNormalRight);
				for (QMap<QString,int>::Iterator it = currentDoc->MasterNames.begin(); it != currentDoc->MasterNames.end(); ++it)
				{
					if ((it.key() == CommonStrings::masterPageNormal) && (!conNam))
						pageData->addItem(CommonStrings::trMasterPageNormal, it.key());
					else if ((it.key() == CommonStrings::trMasterPageNormal) && (!conNam))
						pageData->addItem(CommonStrings::trMasterPageNormal, it.key());
					else
					{
						if (currentDoc->MasterPages.at(it.value())->LeftPg == 0)
							pageData->addItem(it.key(), it.key());
					}
				}
				if (currentDoc->MasterNames.contains( CommonStrings::trMasterPageNormalRight))
					setCurrentComboItem(pageData, CommonStrings::trMasterPageNormalRight);
			}
			QString transLabel = currentDoc->pageSets()[currentDoc->pagePositioning()].pageNames[mp];
			QLabel* pageLabel = new QLabel(CommonStrings::translatePageSetLocString(transLabel), masterPageGroup);
			pageLabel->setBuddy(pageData);
			masterPageLayout->addWidget(pageLabel, row, 0 );
			masterPageLayout->addWidget(pageData, row, 1);
			row++;
			masterPageCombos.append(pageData);
		}
	}
	dialogLayout->addWidget(masterPageGroup);
	overrideMPSizingCheckBox=new QCheckBox( tr("Override Master Page Sizing"));
	dialogLayout->addWidget(overrideMPSizingCheckBox);
	dsGroupBox7 = new QGroupBox( this );
	dsGroupBox7->setTitle( tr( "Page Size" ) );
	dsGroupBox7Layout = new QGridLayout( dsGroupBox7 );
	dsGroupBox7Layout->setSpacing( 5 );
	dsGroupBox7Layout->setMargin( 5 );
	TextLabel1 = new QLabel( tr( "&Size:" ), dsGroupBox7);
	dsGroupBox7Layout->addWidget( TextLabel1, 0, 0);

	PageSize *ps=new PageSize(currentDoc->pageSize());
	prefsPageSizeName=ps->name();
	sizeQComboBox = new QComboBox(dsGroupBox7);
	QStringList insertList(ps->activeSizeTRList());
	if (insertList.indexOf(prefsPageSizeName)==-1)
		insertList<<prefsPageSizeName;
	insertList.sort();
	insertList<<CommonStrings::trCustomPageSize;
	sizeQComboBox->addItems(insertList);
	int sizeIndex = insertList.indexOf(ps->nameTR());
	if (sizeIndex != -1)
		sizeQComboBox->setCurrentIndex(sizeIndex);
	else
		sizeQComboBox->setCurrentIndex(sizeQComboBox->count()-1);

	TextLabel1->setBuddy(sizeQComboBox);
	dsGroupBox7Layout->addWidget(sizeQComboBox, 0, 1, 1, 3);
	TextLabel2 = new QLabel( tr( "Orie&ntation:" ), dsGroupBox7);
	dsGroupBox7Layout->addWidget( TextLabel2, 1, 0);
	orientationQComboBox = new QComboBox(dsGroupBox7);
	orientationQComboBox->addItem( tr( "Portrait" ) );
	orientationQComboBox->addItem( tr( "Landscape" ) );
	orientationQComboBox->setCurrentIndex(currentDoc->pageOrientation() );
	TextLabel2->setBuddy(orientationQComboBox);
	dsGroupBox7Layout->addWidget( orientationQComboBox, 1, 1, 1, 3 );
	widthSpinBox = new ScrSpinBox( 1, 10000, dsGroupBox7, currentDoc->unitIndex() );
	widthQLabel = new QLabel( tr( "&Width:" ), dsGroupBox7);
	widthSpinBox->setValue(currentDoc->pageWidth() * currentDoc->unitRatio());
	widthQLabel->setBuddy(widthSpinBox);
	dsGroupBox7Layout->addWidget( widthQLabel, 2, 0 );
	dsGroupBox7Layout->addWidget( widthSpinBox, 2, 1 );
	heightSpinBox = new ScrSpinBox( 1, 10000, dsGroupBox7, currentDoc->unitIndex() );
	heightSpinBox->setValue(currentDoc->pageHeight() * currentDoc->unitRatio());
	heightQLabel = new QLabel( tr( "&Height:" ), dsGroupBox7);
	heightQLabel->setBuddy(heightSpinBox);
	dsGroupBox7Layout->addWidget( heightQLabel, 2, 2 );
	dsGroupBox7Layout->addWidget( heightSpinBox, 2, 3 );
	moveObjects = new QCheckBox( dsGroupBox7);
	moveObjects->setText( tr( "Move Objects with their Page" ) );
	moveObjects->setChecked( true );
	dsGroupBox7Layout->addWidget( moveObjects, 3, 0, 1, 4 );
	dialogLayout->addWidget( dsGroupBox7 );
	dsGroupBox7->setEnabled(false);
	bool b=(sizeQComboBox->currentText() == CommonStrings::trCustomPageSize);
	heightSpinBox->setEnabled( b );
	widthSpinBox->setEnabled( b );
	delete ps;

	okCancelLayout = new QHBoxLayout;
	okCancelLayout->setSpacing( 5 );
	okCancelLayout->setMargin( 5 );
	QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	okCancelLayout->addItem( spacer );

	okButton = new QPushButton( CommonStrings::tr_OK, this);
	okButton->setDefault( true );
	okCancelLayout->addWidget( okButton );

	cancelButton = new QPushButton( CommonStrings::tr_Cancel, this);
	okCancelLayout->addWidget( cancelButton );
	dialogLayout->addLayout( okCancelLayout );
	setMaximumSize(sizeHint());
	unitRatio = currentDoc->unitRatio();

	// signals and slots connections
	connect( insWhereData, SIGNAL( activated(int) ), this, SLOT( insWherePageDataDisable(int) ) );
	connect( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect(orientationQComboBox, SIGNAL(activated(int)), this, SLOT(setOrientation(int)));
	connect(sizeQComboBox, SIGNAL(activated(const QString &)), this, SLOT(setSize(const QString &)));
	connect(overrideMPSizingCheckBox, SIGNAL(stateChanged(int)), this, SLOT(enableSizingControls(int)));
}

void InsPage::setSize(const QString & gr)
{
	widthSpinBox->setEnabled(false);
	heightSpinBox->setEnabled(false);
	PageSize *ps2 = new PageSize(gr);
	prefsPageSizeName = ps2->name();
	if (gr == CommonStrings::trCustomPageSize)
	{
		widthSpinBox->setEnabled(true);
		heightSpinBox->setEnabled(true);
		prefsPageSizeName = CommonStrings::customPageSize;
	}
	else
	{
		widthSpinBox->setValue(ps2->width() * unitRatio);
		heightSpinBox->setValue(ps2->height() * unitRatio);
	}
	delete ps2;
}

void InsPage::setOrientation(int ori)
{
	double br;
	setSize(sizeQComboBox->currentText());
	if (ori == 0)
	{
		if (sizeQComboBox->currentText() == CommonStrings::trCustomPageSize)
		{
			br = widthSpinBox->value();
			widthSpinBox->setValue(heightSpinBox->value());
			heightSpinBox->setValue(br);
		}
	}
	else
	{
		br = widthSpinBox->value();
		widthSpinBox->setValue(heightSpinBox->value());
		heightSpinBox->setValue(br);
	}
}

const QStringList InsPage::getMasterPages()
{
	QStringList ret;
	for (int n = 0; n < masterPageCombos.count(); ++n)
	{
		int currentIndex = masterPageCombos.at(n)->currentIndex();
		QVariant pageVar = masterPageCombos.at(n)->itemData(currentIndex);
		ret.append(pageVar.toString());
	}
	return ret;
}

const QString InsPage::getMasterPageN(uint n)
{
	QComboBox* comboBox = masterPageCombos.at(n);
	int currentIndex = comboBox->currentIndex();
	return comboBox->itemData(currentIndex).toString();
}

int InsPage::getWhere() const
{
	return insWhereData->currentIndex();
}

int InsPage::getWherePage() const
{
	return insWherePageData->value();
}

int InsPage::getCount() const
{
	return insCountData->value();
}

void InsPage::insWherePageDataDisable(int index)
{
	insWherePageData->setDisabled((index==2));
}

void InsPage::enableSizingControls(int state)
{
	dsGroupBox7->setEnabled(state==Qt::Checked);
}
