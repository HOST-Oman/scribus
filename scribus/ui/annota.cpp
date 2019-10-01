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

#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QFrame>
#include <QLabel>
#include <QComboBox>
#include <QStackedWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QPixmap>
#include <QStringList>
#include <QCheckBox>

#include "scfonts.h"
#include "annota.h"
#include "ui/customfdialog.h"
#include "prefsmanager.h"
#include "prefsfile.h"
#include "commonstrings.h"
#include "pageitem.h"
#include "navigator.h"
#include "scribusview.h"
#include "util_formats.h"
#include "ui/scrspinbox.h"

Annota::Annota(QWidget* parent, PageItem *it, int Seite, int b, int h, ScribusView* vie) : QDialog(parent)
{
	setModal(true);
	setWindowTitle( tr( "Annotation Properties" ) );
	item = it;
	Width = b;
	Height = h;
	OriWidth = b;
	OriHeight = h;
	view = vie;
	MaxSeite = Seite;
	QStringList tl;
	if ((item->annotation().ActionType() == Annotation::Action_GoTo) || (item->annotation().ActionType() == Annotation::Action_GoToR_FileRel) || (item->annotation().ActionType() == Annotation::Action_GoToR_FileAbs))
	{
		QString tm = item->annotation().Action();
		tl = tm.split(" ", QString::SkipEmptyParts);
	}
	else
	{
		tl.append("0");
		tl.append("0");
	}

	AnnotLayout = new QVBoxLayout( this );
	AnnotLayout->setSpacing( 5 );
	AnnotLayout->setMargin( 10 );

	Layout1 = new QHBoxLayout;
	Layout1->setSpacing( 5 );
	Layout1->setMargin( 0 );

	ComboBox1 = new QComboBox(this);
	if (!view->Doc->masterPageMode())
		ComboBox1->addItem( tr("Text"));
	ComboBox1->addItem( tr("Link"));
	ComboBox1->addItem( tr("External Link"));
	ComboBox1->addItem( tr("External Web-Link"));
	ComboBox1->setEditable(false);
	TextLabel1 = new QLabel( tr("&Type:"), this);
	TextLabel1->setBuddy(ComboBox1);
	Layout1->addWidget( TextLabel1 );
	Layout1->addWidget( ComboBox1 );
	AnnotLayout->addLayout( Layout1 );
	if (view->Doc->masterPageMode())
	{
		if (item->annotation().Type() < 2)
			ComboBox1->setCurrentIndex(item->annotation().Type() - 1);
		else
			ComboBox1->setCurrentIndex(item->annotation().Type()-11);
		if ((item->annotation().ActionType() == Annotation::Action_GoToR_FileRel) || (item->annotation().ActionType() == Annotation::Action_URI))
			ComboBox1->setCurrentIndex(item->annotation().ActionType() - 6);
		if (item->annotation().ActionType() == Annotation::Action_GoToR_FileAbs)
			ComboBox1->setCurrentIndex(1);
	}
	else
	{
		if (item->annotation().Type() < 2)
			ComboBox1->setCurrentIndex(item->annotation().Type());
		else
			ComboBox1->setCurrentIndex(item->annotation().Type()-10);
		if ((item->annotation().ActionType() == Annotation::Action_GoToR_FileRel) || (item->annotation().ActionType() == Annotation::Action_URI))
			ComboBox1->setCurrentIndex(item->annotation().ActionType() - 5);
		if (item->annotation().ActionType() == Annotation::Action_GoToR_FileAbs)
			ComboBox1->setCurrentIndex(2);
	}
	Fram = new QStackedWidget(this);
	AnnotLayout->addWidget( Fram );

//	Frame9 = new QFrame( this );
//	Frame9->setFrameShape( QFrame::NoFrame );
//	Frame9->setFrameShadow( QFrame::Plain );

	GroupBox2 = new QGroupBox( this );
	GroupBox2->setTitle( tr( "Options" ) );
	GroupBox2Layout = new QGridLayout( GroupBox2 );
	GroupBox2Layout->setAlignment( Qt::AlignTop );
	GroupBox2Layout->setSpacing( 5 );
	GroupBox2Layout->setMargin( 10 );
	TextLabelG1 = new QLabel( tr("Icon:"), GroupBox2);
	ComboBox2 = new QComboBox(GroupBox2);
	ComboBox2->addItem( tr("Note"));
	ComboBox2->addItem( tr("Comment"));
	ComboBox2->addItem( tr("Key"));
	ComboBox2->addItem( tr("Help"));
	ComboBox2->addItem( tr("NewParagraph"));
	ComboBox2->addItem( tr("Paragraph"));
	ComboBox2->addItem( tr("Insert"));
	ComboBox2->addItem( tr("Cross"));
	ComboBox2->addItem( tr("Circle"));
	ComboBox2->setEditable(false);
	ComboBox2->setCurrentIndex(item->annotation().Icon());
	GroupBox2Layout->addWidget( TextLabelG1, 0, 0 );
	GroupBox2Layout->addWidget( ComboBox2, 0, 1 );
	textIsOpen = new QCheckBox( tr("Annotation is Open"), GroupBox2);
	textIsOpen->setChecked(item->annotation().IsAnOpen());
	GroupBox2Layout->addWidget( textIsOpen, 1, 0, 1, 1 );
	Fram->addWidget(GroupBox2);

	GroupBox1 = new QGroupBox( this );
	GroupBox1->setTitle( tr( "Destination" ) );
	GroupBox1Layout = new QGridLayout( GroupBox1 );
	GroupBox1Layout->setAlignment( Qt::AlignTop );
	GroupBox1Layout->setSpacing( 5 );
	GroupBox1Layout->setMargin( 10 );

	Destfile = new QLineEdit(GroupBox1);
	Destfile->setText(item->annotation().Extern());
	Destfile->setReadOnly(true);
	GroupBox1Layout->addWidget( Destfile, 0, 0, 1, 2 );
	ChFile = new QPushButton(GroupBox1);
	ChFile->setText( tr("C&hange..."));
	GroupBox1Layout->addWidget( ChFile, 0, 2 );
	useAbsolute = new QCheckBox( tr("Export absolute Filename"), GroupBox1);
	GroupBox1Layout->addWidget( useAbsolute, 1, 0, 1, 3 );
	if (item->annotation().ActionType() == Annotation::Action_GoToR_FileRel)
		useAbsolute->setChecked(false);
	else if (item->annotation().ActionType() == Annotation::Action_GoToR_FileAbs)
		useAbsolute->setChecked(true);
	if ((item->annotation().ActionType() != Annotation::Action_GoToR_FileRel) && (item->annotation().ActionType() != Annotation::Action_URI) && (item->annotation().ActionType() != Annotation::Action_GoToR_FileAbs))
	{
		Destfile->hide();
		ChFile->hide();
		useAbsolute->hide();
	}

	SpinBox1 = new ScrSpinBox( GroupBox1);
	SpinBox1->setDecimals(0);
	SpinBox1->setMinimum(1);
	SpinBox1->setMaximum(((item->annotation().ActionType() == Annotation::Action_GoToR_FileRel) || (item->annotation().ActionType() == Annotation::Action_GoToR_FileAbs)) ? 1000 : Seite);
	SpinBox1->setSuffix("");
	TextLabel3 = new QLabel( tr("&Page:"), GroupBox1);
	TextLabel3->setBuddy(SpinBox1);
	GroupBox1Layout->addWidget( TextLabel3, 2, 0 );
	GroupBox1Layout->addWidget( SpinBox1, 2, 1 );
	if ((!Destfile->text().isEmpty()) && ((item->annotation().ActionType() == Annotation::Action_GoToR_FileRel) || (item->annotation().ActionType() == Annotation::Action_GoToR_FileAbs)))
		Pg = new Navigator( GroupBox1, 100, item->annotation().Ziel()+1, view, item->annotation().Extern());
	else
	{
		int targetPage = item->annotation().Ziel();
		if ((targetPage >= 0) && (targetPage < view->Doc->Pages->count()))
			Pg = new Navigator( GroupBox1, 100, item->annotation().Ziel(), view);
		else
		{
			item->annotation().setZiel(view->Doc->currentPageNumber());
			Pg = new Navigator( GroupBox1, 100, item->annotation().Ziel(), view);
		}
	}
	SpinBox1->setValue(item->annotation().Ziel()+1);
	Pg->setMinimumSize(QSize(Pg->pmx.width(), Pg->pmx.height()));
	GroupBox1Layout->addWidget(Pg, 2, 2, 3, 1);

	SpinBox2 = new ScrSpinBox( GroupBox1);
	SpinBox2->setDecimals(0);
	SpinBox2->setSuffix( tr( " pt" ) );
	SpinBox2->setMaximum(Width);
	SpinBox2->setValue(tl[0].toInt());
	TextLabel4 = new QLabel( tr("&X-Pos:"), GroupBox1 );
	TextLabel4->setBuddy(SpinBox2);
	GroupBox1Layout->addWidget( TextLabel4, 3, 0 );
	GroupBox1Layout->addWidget( SpinBox2, 3, 1 );
	SpinBox3 = new ScrSpinBox( GroupBox1 );
	SpinBox3->setDecimals(0);
	SpinBox3->setMaximum(Height);
	SpinBox3->setSuffix( tr( " pt" ) );
	SpinBox3->setValue(Height-tl[1].toInt());
	TextLabel5 = new QLabel( tr("&Y-Pos:"), GroupBox1 );
	TextLabel5->setBuddy(SpinBox3);
	GroupBox1Layout->addWidget( TextLabel5, 4, 0 );
	GroupBox1Layout->addWidget( SpinBox3, 4, 1 );
	Fram->addWidget(GroupBox1);

	Layout1_2 = new QHBoxLayout;
	Layout1_2->setSpacing( 5 );
	Layout1_2->setMargin( 0 );

	QSpacerItem* spacer = new QSpacerItem( 2, 2, QSizePolicy::Expanding, QSizePolicy::Minimum );
	Layout1_2->addItem( spacer );
	PushButton1 = new QPushButton( CommonStrings::tr_OK, this );
	PushButton1->setDefault( true );
	Layout1_2->addWidget( PushButton1 );
	PushButton2 = new QPushButton( CommonStrings::tr_Cancel, this );
	Layout1_2->addWidget( PushButton2 );
	AnnotLayout->addLayout( Layout1_2 );

	connect(PushButton1, SIGNAL(clicked()), this, SLOT(SetValues()));
	connect(PushButton2, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ComboBox1, SIGNAL(activated(int)), this, SLOT(SetTarget(int)));
	connect(SpinBox1, SIGNAL(valueChanged(double)), this, SLOT(SetPage(double)));
	connect(Pg, SIGNAL(Coords(double, double)), this, SLOT(SetCoords(double, double)));
	connect(SpinBox2, SIGNAL(valueChanged(double)), this, SLOT(SetCross()));
	connect(SpinBox3, SIGNAL(valueChanged(double)), this, SLOT(SetCross()));
	connect(ChFile, SIGNAL(clicked()), this, SLOT(GetFile()));
	if (view->Doc->masterPageMode())
		SetTarget(item->annotation().Type() - 1);
	else
		SetTarget(item->annotation().Type());
	SetCross();
}

void Annota::SetCoords(double x, double y)
{
	SpinBox2->setValue(static_cast<int>(x*Width));
	SpinBox3->setValue(static_cast<int>(y*Height));
}

void Annota::SetPage(double v)
{
	disconnect(SpinBox1, SIGNAL(valueChanged(double)), this, SLOT(SetPage(double)));
	int link = 2;
	if (view->Doc->masterPageMode())
		link = 1;
	if (ComboBox1->currentIndex() == link)
	{
		if (!Pg->SetSeite(static_cast<int>(v), 100, Destfile->text()))
		{
			SpinBox1->setValue(1);
			Pg->SetSeite(1, 100, Destfile->text());
		}
		Width = Pg->Width;
		Height = Pg->Height;
	}
	else
	{
		Pg->SetSeite(qMin(static_cast<int>(v)-1, MaxSeite-1), 100);
		SpinBox1->setValue(qMin(static_cast<int>(v), MaxSeite));
		Width = OriWidth;
		Height = OriHeight;
	}
	SpinBox2->setMaximum(Width);
	SpinBox3->setMaximum(Height);
	connect(SpinBox1, SIGNAL(valueChanged(double)), this, SLOT(SetPage(double)));
}

void Annota::SetCross()
{
	int x,y;
	disconnect(Pg, SIGNAL(Coords(double, double)), this, SLOT(SetCoords(double, double)));
	x = static_cast<int>(SpinBox2->value()/static_cast<double>(Width)*Pg->pmx.width());
	y = static_cast<int>(SpinBox3->value()/static_cast<double>(Height)*Pg->pmx.height());
	Pg->drawMark(x, y);
	connect(Pg, SIGNAL(Coords(double, double)), this, SLOT(SetCoords(double, double)));
}

void Annota::SetValues()
{
	QString tmp, tmp2;
	item->annotation().setZiel(SpinBox1->value()-1);
	if (view->Doc->masterPageMode())
		item->annotation().setType(ComboBox1->currentIndex()+11);
	else
		item->annotation().setType(ComboBox1->currentIndex()+10);
	switch (item->annotation().Type())
	{
	case 10:
		item->annotation().setActionType(Annotation::Action_None);
		item->annotation().setAnOpen(textIsOpen->isChecked());
		item->annotation().setIcon(ComboBox2->currentIndex());
		break;
	case 11:
		item->annotation().setAction(tmp.setNum(SpinBox2->value())+" "+ tmp2.setNum(Height-SpinBox3->value())+" 0");
		item->annotation().setExtern("");
		item->annotation().setActionType(Annotation::Action_GoTo);
		break;
	case 12:
		item->annotation().setAction(tmp.setNum(SpinBox2->value())+" "+ tmp2.setNum(Height-SpinBox3->value())+" 0");
		if (!Destfile->text().isEmpty())
		{
			item->annotation().setExtern(Destfile->text());
			if (useAbsolute->isChecked())
				item->annotation().setActionType(Annotation::Action_GoToR_FileAbs);
			else
				item->annotation().setActionType(Annotation::Action_GoToR_FileRel);
		}
		item->annotation().setType(Annotation::Link);
		break;
	case 13:
		item->annotation().setAction("");
		if (!Destfile->text().isEmpty())
		{
			item->annotation().setExtern(Destfile->text());
			item->annotation().setActionType(Annotation::Action_URI);
		}
		item->annotation().setType(Annotation::Link);
		break;
	}
	accept();
}

void Annota::SetTarget(int it)
{
	int a = it;
	disconnect(ComboBox1, SIGNAL(activated(int)), this, SLOT(SetTarget(int)));
	if (view->Doc->masterPageMode())
		a++;
	Pg->show();
	TextLabel3->show();
	TextLabel4->show();
	TextLabel5->show();
	SpinBox1->show();
	SpinBox2->show();
	SpinBox3->show();
	switch (a)
	{
	case 1:
		Fram->setCurrentIndex(1);
		Destfile->setText("");
		Destfile->hide();
		ChFile->hide();
		useAbsolute->hide();
		item->annotation().setActionType(Annotation::Action_GoTo);
		SetPage(qMin(static_cast<int>(SpinBox1->value()), MaxSeite));
		break;
	case 2:
		Fram->setCurrentIndex(1);
		Destfile->show();
		ChFile->show();
		useAbsolute->show();
		Destfile->setReadOnly(true);
		if ((Destfile->text().isEmpty())  || (item->annotation().ActionType() == Annotation::Action_URI))
		{
			Destfile->setText("");
			GetFile();
		}
		if (Destfile->text().isEmpty())
		{
			item->annotation().setActionType(Annotation::Action_GoTo);
			Destfile->setText("");
			Destfile->hide();
			ChFile->hide();
			useAbsolute->hide();
			if (view->Doc->masterPageMode())
				ComboBox1->setCurrentIndex(0);
			else
				ComboBox1->setCurrentIndex(1);
		}
		else
		{
			if (useAbsolute->isChecked())
				item->annotation().setActionType(Annotation::Action_GoToR_FileAbs);
			else
				item->annotation().setActionType(Annotation::Action_GoToR_FileRel);
		}
		SetPage(qMin(static_cast<int>(SpinBox1->value()), MaxSeite));
		break;
	case 3:
		Fram->setCurrentIndex(1);
		Destfile->show();
		Destfile->setReadOnly(false);
		ChFile->hide();
		useAbsolute->hide();
		Pg->hide();
		TextLabel3->hide();
		TextLabel4->hide();
		TextLabel5->hide();
		SpinBox1->hide();
		SpinBox2->hide();
		SpinBox3->hide();
		item->annotation().setActionType(Annotation::Action_URI);
		break;
	case 11:
		Fram->setCurrentIndex(1);
		if ((item->annotation().ActionType() == Annotation::Action_GoToR_FileRel) || (item->annotation().ActionType() == Annotation::Action_GoToR_FileAbs))
		{
			Destfile->show();
			ChFile->show();
			useAbsolute->show();
			Destfile->setReadOnly(true);
		}
		if (item->annotation().ActionType() == Annotation::Action_URI)
		{
			Destfile->show();
			Destfile->setReadOnly(false);
			ChFile->hide();
			useAbsolute->hide();
			Pg->hide();
			TextLabel3->hide();
			TextLabel4->hide();
			TextLabel5->hide();
			SpinBox1->hide();
			SpinBox2->hide();
			SpinBox3->hide();
		}
		if (Pg->isVisible())
			SetPage(qMin(static_cast<int>(SpinBox1->value()), MaxSeite));
		break;
	default:
		Fram->setCurrentIndex(0);
		break;
	}
	connect(ComboBox1, SIGNAL(activated(int)), this, SLOT(SetTarget(int)));
}

void Annota::GetFile()
{
	QString fn;
	PrefsContext* dirs = PrefsManager::instance().prefsFile->getContext("dirs");
	QString wdir = dirs->get("annot_getfile", ".");
	CustomFDialog dia(this, wdir, tr("Open"), tr("%1;;All Files (*)").arg(FormatsManager::instance()->extensionsForFormat(FormatsManager::PDF)));
	if (!Destfile->text().isEmpty())
		dia.setSelection(Destfile->text());
	if (dia.exec() == QDialog::Accepted)
	{
		fn = dia.selectedFile();
		if (!fn.isEmpty())
		{
			dirs->set("annot_getfile", fn.left(fn.lastIndexOf("/")));
			Destfile->setText(fn);
			SpinBox1->setValue(1);
			SpinBox1->setMaximum(1000);
			SetPage(1);
		}
	}
}
