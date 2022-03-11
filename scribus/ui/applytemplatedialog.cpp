/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "applytemplatedialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QEvent>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSpacerItem>
#include <QSpinBox>
#include <QToolTip>
#include <QVBoxLayout>

#include "commonstrings.h"
#include "scpage.h"
#include "scribusdoc.h"
#include "iconmanager.h"

enum {
    CurrentPage,
    EvenPages,
    OddPages,
    AllPages
};

/*
 *  Constructs a ApplyMasterPageDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
ApplyMasterPageDialog::ApplyMasterPageDialog( QWidget* parent ) : QDialog( parent )
{
	setModal(true);
	setWindowTitle( tr( "Apply Master Page" ));
	setWindowIcon(IconManager::instance().loadIcon("AppIcon.png"));
	ApplyMasterPageDialogLayout = new QVBoxLayout(this);
	ApplyMasterPageDialogLayout->setContentsMargins(9, 9, 9, 9);
	ApplyMasterPageDialogLayout->setSpacing(6);

	templateNameLayout = new QHBoxLayout;
	templateNameLayout->setContentsMargins(0, 0, 0, 0);
	templateNameLayout->setSpacing(6);

	masterPageLabel = new QLabel( this );
	templateNameLayout->addWidget( masterPageLabel );
	spacer2 = new QSpacerItem( 1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum );
	templateNameLayout->addItem( spacer2 );

	masterPageComboBox = new QComboBox(this);
	masterPageComboBox->setEditable(false);
	templateNameLayout->addWidget( masterPageComboBox );
	ApplyMasterPageDialogLayout->addLayout( templateNameLayout );

	applyToPageButtonGroup = new QGroupBox(this);
	applyToPageButtonGroup->setMinimumSize( QSize( 250, 0 ) );
	applyToPageButtonGroupLayout = new QVBoxLayout(applyToPageButtonGroup);
	applyToPageButtonGroupLayout->setSpacing(6);
	applyToPageButtonGroupLayout->setContentsMargins(9, 9, 9, 9);

	currentPageRadioButton = new QRadioButton( applyToPageButtonGroup );
	currentPageRadioButton->setChecked( true );
	applyToPageButtonGroupLayout->addWidget( currentPageRadioButton );

	evenPagesRadioButton = new QRadioButton( applyToPageButtonGroup );
	applyToPageButtonGroupLayout->addWidget( evenPagesRadioButton );

	oddPagesRadioButton = new QRadioButton( applyToPageButtonGroup );
	applyToPageButtonGroupLayout->addWidget( oddPagesRadioButton );

	allPagesRadioButton = new QRadioButton( applyToPageButtonGroup );
	applyToPageButtonGroupLayout->addWidget( allPagesRadioButton );

	rangeLayout = new QHBoxLayout;
	rangeLayout->setSpacing(6);
	rangeLayout->setContentsMargins(0, 0, 0, 0);

	useRangeCheckBox = new QCheckBox( applyToPageButtonGroup );
	useRangeCheckBox->setEnabled( false );	
	rangeLayout->addWidget( useRangeCheckBox );

	fromPageSpinBox = new ScrSpinBox( applyToPageButtonGroup );
	fromPageSpinBox->setEnabled( false );
	fromPageSpinBox->setMinimum( 1 );
	fromPageSpinBox->setDecimals(0);
	fromPageSpinBox->setSuffix("");
	rangeLayout->addWidget( fromPageSpinBox );

	toPageLabel = new QLabel( applyToPageButtonGroup );
	rangeLayout->addWidget( toPageLabel );

	toPageSpinBox = new ScrSpinBox( applyToPageButtonGroup );
	toPageSpinBox->setEnabled( false );
	toPageSpinBox->setMinimum( 1 );
	toPageSpinBox->setDecimals(0);
	toPageSpinBox->setSuffix("");
	rangeLayout->addWidget( toPageSpinBox );
	spacer3 = new QSpacerItem( 1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum );
	rangeLayout->addItem( spacer3 );
	applyToPageButtonGroupLayout->addLayout( rangeLayout );
	ApplyMasterPageDialogLayout->addWidget( applyToPageButtonGroup );

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	ApplyMasterPageDialogLayout->addWidget(buttonBox);
	languageChange();
	resize( QSize(268, 230).expandedTo(minimumSizeHint()) );

	// signals and slots connections
	connect( useRangeCheckBox, SIGNAL( toggled(bool) ), this, SLOT( enableRange(bool) ) );
	connect( currentPageRadioButton, SIGNAL( clicked() ), this, SLOT( singleSelectable() ) );
	connect( evenPagesRadioButton, SIGNAL( clicked() ), this, SLOT( rangeSelectable() ) );
	connect( oddPagesRadioButton, SIGNAL( clicked() ), this, SLOT( rangeSelectable() ) );
	connect( allPagesRadioButton, SIGNAL( clicked() ), this, SLOT( rangeSelectable() ) );
	connect( fromPageSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( checkRangeFrom() ) );
	connect( toPageSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( checkRangeTo() ) );
	connect(buttonBox, &QDialogButtonBox::accepted, this, &ApplyMasterPageDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &ApplyMasterPageDialog::reject);

	// buddies
	masterPageLabel->setBuddy( masterPageComboBox );
}

void ApplyMasterPageDialog::setup(ScribusDoc *doc, const QString& Nam)
{
	QString na = (Nam == CommonStrings::masterPageNormal) ? CommonStrings::trMasterPageNormal : Nam;
	QString in;

	int cc = 0;
	for (QMap<QString,int>::Iterator it = doc->MasterNames.begin(); it != doc->MasterNames.end(); ++it)
	{
		in = (it.key() == CommonStrings::masterPageNormal) ? CommonStrings::trMasterPageNormal : it.key();
		masterPageComboBox->addItem(in, it.key());
		if (in == na)
			masterPageComboBox->setCurrentIndex(cc);
		++cc;
	}
	const int docPagesCount = doc->Pages->count();
	if (docPagesCount < 2)
		evenPagesRadioButton->setEnabled(false);
	fromPageSpinBox->setMaximum(docPagesCount);
	fromPageSpinBox->setValue(doc->currentPage()->pageNr() + 1);
	toPageSpinBox->setMaximum(docPagesCount);
	toPageSpinBox->setValue(docPagesCount);
}


QString ApplyMasterPageDialog::getMasterPageName()
{
	int currentIndex = masterPageComboBox->currentIndex();
	return masterPageComboBox->itemData(currentIndex).toString();
}


int ApplyMasterPageDialog::getPageSelection()
{
	if (currentPageRadioButton->isChecked())
		return CurrentPage;
	if (evenPagesRadioButton->isChecked())
		return EvenPages;
	if (oddPagesRadioButton->isChecked())
		return OddPages;
	return AllPages;
}

void ApplyMasterPageDialog::checkRangeFrom()
{
	disconnect(fromPageSpinBox, SIGNAL(valueChanged(double)), this, SLOT(checkRangeFrom()));
	disconnect(toPageSpinBox, SIGNAL(valueChanged(double)), this, SLOT(checkRangeTo()));
	if (fromPageSpinBox->value() > toPageSpinBox->value())
		toPageSpinBox->setValue(fromPageSpinBox->value());
	connect(fromPageSpinBox, SIGNAL(valueChanged(double)), this, SLOT(checkRangeFrom()));
	connect(toPageSpinBox, SIGNAL(valueChanged(double)), this, SLOT(checkRangeTo()));
}

void ApplyMasterPageDialog::checkRangeTo()
{
	disconnect(fromPageSpinBox, SIGNAL(valueChanged(double)), this, SLOT(checkRangeFrom()));
	disconnect(toPageSpinBox, SIGNAL(valueChanged(double)), this, SLOT(checkRangeTo()));
	if (toPageSpinBox->value() < fromPageSpinBox->value())
		fromPageSpinBox->setValue(toPageSpinBox->value());
	connect(fromPageSpinBox, SIGNAL(valueChanged(double)), this, SLOT(checkRangeFrom()));
	connect(toPageSpinBox, SIGNAL(valueChanged(double)), this, SLOT(checkRangeTo()));
}

void ApplyMasterPageDialog::enableRange( bool enabled )
{
	fromPageSpinBox->setEnabled(enabled);
	toPageSpinBox->setEnabled(enabled);
}

void ApplyMasterPageDialog::rangeSelectable()
{
	useRangeCheckBox->setEnabled(true);
	enableRange(useRangeCheckBox->isChecked());
}

void ApplyMasterPageDialog::singleSelectable()
{
	useRangeCheckBox->setEnabled(false);
	fromPageSpinBox->setEnabled(false);
	toPageSpinBox->setEnabled(false);
}


bool ApplyMasterPageDialog::usingRange()
{
	return useRangeCheckBox->isChecked();
}


int ApplyMasterPageDialog::getFromPage()
{
	if (useRangeCheckBox->isChecked())
		return static_cast<int>(fromPageSpinBox->value());
	return -1;
}


int ApplyMasterPageDialog::getToPage()
{
	if (useRangeCheckBox->isChecked())
		return static_cast<int>(toPageSpinBox->value());
	return -1;
}

void ApplyMasterPageDialog::changeEvent(QEvent *e)
{
	if (e->type() == QEvent::LanguageChange)
	{
		languageChange();
	}
	else
		QWidget::changeEvent(e);
}

void ApplyMasterPageDialog::languageChange()
{
	setWindowTitle( tr( "Apply Master Page" ) );
	masterPageLabel->setText( tr( "&Master Page:" ) );
	applyToPageButtonGroup->setTitle( tr( "Apply to" ) );
	currentPageRadioButton->setText( tr( "Current &Page" ) );
	currentPageRadioButton->setShortcut( QKeySequence( tr( "Alt+P" ) ) );
	evenPagesRadioButton->setText( tr( "&Even Pages" ) );
	evenPagesRadioButton->setShortcut( QKeySequence( tr( "Alt+E" ) ) );
	oddPagesRadioButton->setText( tr( "O&dd Pages" ) );
	oddPagesRadioButton->setShortcut( QKeySequence( tr( "Alt+D" ) ) );
	allPagesRadioButton->setText( tr( "&All Pages" ) );
	allPagesRadioButton->setShortcut( QKeySequence( tr( "Alt+A" ) ) );
	useRangeCheckBox->setText( tr( "&Within Range" ) );
	useRangeCheckBox->setShortcut( QKeySequence( tr( "Alt+W" ) ) );
	useRangeCheckBox->setToolTip( "<qt>" + tr( "Apply the selected master page to even, odd or all pages within the following range") + "</qt>" );
	toPageLabel->setText( tr( "to" ) );
}
