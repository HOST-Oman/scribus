/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include "polyprops.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPixmap>
#include <QPushButton>
#include "ui/polygonwidget.h"
#include "commonstrings.h"
#include "iconmanager.h"

PolyVectorDialog::PolyVectorDialog(QWidget* parent, int polyC, double polyF, bool polyS, double polyR, double polyCurvature, double polyInnerRot, double polyOuterCurvature) : ScrPaletteBase( parent, "PolyVectorDialog", false, nullptr )
{
	setWindowTitle( tr( "Polygon Properties" ) );
	setWindowIcon(IconManager::instance()->loadIcon("AppIcon.png"));
	PolygonPropsLayout = new QVBoxLayout( this );
	PolygonPropsLayout->setMargin(10);
	PolygonPropsLayout->setSpacing(5);
	polyWidget = new PolygonWidget(this, polyC, polyF, polyS, polyR, polyCurvature, polyInnerRot, polyOuterCurvature, true);
	PolygonPropsLayout->addWidget( polyWidget );
	// signals and slots connections
	connect(polyWidget, SIGNAL(NewVectors(int,double,bool,double,double,double,double)), this, SIGNAL(NewVectors(int,double,bool,double,double,double,double)));
	connect (this, SIGNAL(accepted()), this, SIGNAL(endEdit()));
}

void PolyVectorDialog::setValues(int polyCorners, double polyF, bool polyUseConvexFactor, double polyRotation, double polyCurvature, double polyInnerRot, double polyOuterCurvature)
{
	polyWidget->setValues(polyCorners, polyF, polyUseConvexFactor, polyRotation, polyCurvature, polyInnerRot, polyOuterCurvature);
}

PolygonProps::PolygonProps(QWidget* parent, int polyC, double polyF, bool polyS, double polyR, double polyCurvature, double polyInnerRot, double polyOuterCurvature) : QDialog( parent )
{
	setModal(true);
	setWindowTitle( tr( "Polygon Properties" ) );
	setWindowIcon(IconManager::instance()->loadIcon("AppIcon.png"));
	PolygonPropsLayout = new QVBoxLayout( this );
	PolygonPropsLayout->setMargin(10);
	PolygonPropsLayout->setSpacing(5);
	polyWidget = new PolygonWidget(this, polyC, polyF, polyS, polyR, polyCurvature, polyInnerRot, polyOuterCurvature);
	PolygonPropsLayout->addWidget( polyWidget );
	Layout1 = new QHBoxLayout;
	Layout1->setMargin(0);
	Layout1->setSpacing(5);
	QSpacerItem* spacer_2 = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
	Layout1->addItem( spacer_2 );
	okButton = new QPushButton(this);
	okButton->setText( CommonStrings::tr_OK );
	okButton->setDefault( true );
	Layout1->addWidget( okButton );
	cancelButton = new QPushButton(this);
	cancelButton->setText( CommonStrings::tr_Cancel );
	Layout1->addWidget( cancelButton );
	PolygonPropsLayout->addLayout( Layout1 );
	// signals and slots connections
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void PolygonProps::getValues(int* polyC, double* polyF, bool* polyS, double* polyR, double* polyCurvature, double* polyInnerRot, double* polyOuterCurvature)
{
	polyWidget->getValues(polyC, polyF, polyS, polyR, polyCurvature, polyInnerRot, polyOuterCurvature);
}

void PolygonProps::setValues(int polyCorners, double polyF, bool polyUseConvexFactor, double polyRotation, double polyCurvature, double polyInnerRot, double polyOuterCurvature)
{
	polyWidget->setValues(polyCorners, polyF, polyUseConvexFactor, polyRotation, polyCurvature, polyInnerRot, polyOuterCurvature);
}
