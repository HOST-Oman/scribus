/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include "polygonwidget.h"
#include "scconfig.h"
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QRectF>

#if defined(_MSC_VER) && !defined(_USE_MATH_DEFINES)
#define _USE_MATH_DEFINES
#endif
#include <cmath>

#include "util_math.h"
#include "prefsstructs.h"

using namespace std;

PolygonWidget::PolygonWidget(QWidget* parent) : QWidget( parent )
{
	setupUi(this);
	factorSpinBox->setDecimals(0);
	rotationSpinBox->setNewUnit(6);
	rotationSpinBox->setDecimals(0);
	cornersSpinBox->setDecimals(0);
	cornersSpinBox->setDecimals(0);
	curvatureSpinBox->setDecimals(0);
	innerRotationSpinBox->setNewUnit(6);
	innerRotationSpinBox->setDecimals(0);
	outerCurvatureSpinBox->setDecimals(0);
	editMode = false;
}

PolygonWidget::PolygonWidget(QWidget* parent, int polyCorners, double polyF, bool polyUseConvexFactor, double polyRotation, double polyCurvature, double polyInnerRot, double polyOuterCurvature, bool forEditMode) : QWidget( parent )
{
	setupUi(this);
	factorSpinBox->setDecimals(0);
	rotationSpinBox->setNewUnit(6);
	rotationSpinBox->setDecimals(0);
	cornersSpinBox->setDecimals(0);
	cornersSpinBox->setDecimals(0);
	curvatureSpinBox->setDecimals(0);
	innerRotationSpinBox->setNewUnit(6);
	innerRotationSpinBox->setDecimals(0);
	outerCurvatureSpinBox->setDecimals(0);
	editMode = forEditMode;
	if (editMode)
		Preview->hide();
	setValues(polyCorners, polyF, polyUseConvexFactor, polyRotation, polyCurvature, polyInnerRot, polyOuterCurvature);
	updatePreview();
	// signals and slots connections
	connectSignals(true);
}

void PolygonWidget::connectSignals(bool conn)
{
	if (conn)
	{
		connect(factorSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setFactorSlider(double)));
		connect(rotationSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setRotationSlider(double)));
		connect(factorSlider, SIGNAL(valueChanged(int)), factorSpinBox, SLOT(setValue(int)));
		connect(factorSlider, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
		connect(rotationSlider, SIGNAL(valueChanged(int)), rotationSpinBox, SLOT(setValue(int)));
		connect(rotationSlider, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
		connect(cornersSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updatePreview()));
		connect(applyConvexGroupBox, SIGNAL(clicked()), this, SLOT(updatePreview()));
		connect(curvatureSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setCurvatureSlider(double)));
		connect(curvatureSlider, SIGNAL(valueChanged(int)), curvatureSpinBox, SLOT(setValue(int)));
		connect(curvatureSlider, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
		connect(innerRotationSlider, SIGNAL(valueChanged(int)), innerRotationSpinBox, SLOT(setValue(int)));
		connect(innerRotationSlider, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
		connect(innerRotationSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setInnerRotationSlider(double)));
		connect(outerCurvatureSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setOuterCurvatureSlider(double)));
		connect(outerCurvatureSlider, SIGNAL(valueChanged(int)), outerCurvatureSpinBox, SLOT(setValue(int)));
		connect(outerCurvatureSlider, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
	}
	else
	{
		disconnect(factorSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setFactorSlider(double)));
		disconnect(rotationSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setRotationSlider(double)));
		disconnect(factorSlider, SIGNAL(valueChanged(int)), factorSpinBox, SLOT(setValue(int)));
		disconnect(factorSlider, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
		disconnect(rotationSlider, SIGNAL(valueChanged(int)), rotationSpinBox, SLOT(setValue(int)));
		disconnect(rotationSlider, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
		disconnect(cornersSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updatePreview()));
		disconnect(applyConvexGroupBox, SIGNAL(clicked()), this, SLOT(updatePreview()));
		disconnect(curvatureSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setCurvatureSlider(double)));
		disconnect(curvatureSlider, SIGNAL(valueChanged(int)), curvatureSpinBox, SLOT(setValue(int)));
		disconnect(curvatureSlider, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
		disconnect(innerRotationSlider, SIGNAL(valueChanged(int)), innerRotationSpinBox, SLOT(setValue(int)));
		disconnect(innerRotationSlider, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
		disconnect(innerRotationSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setInnerRotationSlider(double)));
		disconnect(outerCurvatureSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setOuterCurvatureSlider(double)));
		disconnect(outerCurvatureSlider, SIGNAL(valueChanged(int)), outerCurvatureSpinBox, SLOT(setValue(int)));
		disconnect(outerCurvatureSlider, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
	}
}


void PolygonWidget::restoreDefaults(struct ItemToolPrefs *prefsData)
{
	connectSignals(false);
	PFactor = prefsData->polyFactor;
	cornersSpinBox->setValue(prefsData->polyCorners);
	int fGui = qRound(getUserValFromFactor(prefsData->polyFactor));
	rotationSpinBox->setValue(static_cast<int>(prefsData->polyRotation));
	rotationSlider->setValue(static_cast<int>(prefsData->polyRotation));
	applyConvexGroupBox->setChecked(prefsData->polyUseFactor);
	factorSpinBox->setValue(fGui);
	factorSlider->setValue(fGui);
	applyConvexGroupBox->setChecked(prefsData->polyUseFactor);
	curvatureSpinBox->setValue(qRound(prefsData->polyCurvature * 100));
	curvatureSlider->setValue(qRound(prefsData->polyCurvature * 100));
	outerCurvatureSpinBox->setValue(qRound(prefsData->polyOuterCurvature * 100));
	outerCurvatureSlider->setValue(qRound(prefsData->polyOuterCurvature * 100));
	innerRotationSpinBox->setValue(static_cast<int>(prefsData->polyInnerRot));
	innerRotationSlider->setValue(static_cast<int>(prefsData->polyInnerRot));
	updatePreview();
	connectSignals(true);
}

void PolygonWidget::saveGuiToPrefs(struct ItemToolPrefs *prefsData)
{
	prefsData->polyCorners = cornersSpinBox->value();
	prefsData->polyFactor = PFactor;
	prefsData->polyUseFactor = applyConvexGroupBox->isChecked();
	prefsData->polyRotation = rotationSpinBox->value();
	prefsData->polyInnerRot = innerRotationSpinBox->value();
	prefsData->polyCurvature = curvatureSpinBox->value() / 100.0;
	prefsData->polyOuterCurvature = outerCurvatureSpinBox->value() / 100.0;
}

void PolygonWidget::setValues(int polyCorners, double polyF, bool polyUseConvexFactor, double polyRotation, double polyCurvature, double polyInnerRot, double polyOuterCurvature)
{
	PFactor = polyF;
	cornersSpinBox->setValue(polyCorners);
	int fGui = qRound(getUserValFromFactor(polyF));
	rotationSpinBox->setValue(static_cast<int>(polyRotation));
	rotationSlider->setValue(static_cast<int>(polyRotation));
	innerRotationSpinBox->setValue(static_cast<int>(polyInnerRot));
	innerRotationSlider->setValue(static_cast<int>(polyInnerRot));
	applyConvexGroupBox->setChecked(polyUseConvexFactor);
	factorSpinBox->setValue(fGui);
	factorSlider->setValue(fGui);
	applyConvexGroupBox->setChecked(polyUseConvexFactor);
	curvatureSpinBox->setValue(qRound(polyCurvature * 100));
	curvatureSlider->setValue(qRound(polyCurvature * 100));
	outerCurvatureSpinBox->setValue(qRound(polyOuterCurvature * 100));
	outerCurvatureSlider->setValue(qRound(polyOuterCurvature * 100));
}

void PolygonWidget::getValues(int* polyCorners, double* polyF, bool* polyUseConvexFactor, double* polyRotation, double* polyCurvature, double* polyInnerRot, double* polyOuterCurvature)
{
	*polyCorners = cornersSpinBox->value();
	*polyF = PFactor;
	*polyUseConvexFactor = applyConvexGroupBox->isChecked();
	*polyRotation = rotationSpinBox->value();
	*polyInnerRot = innerRotationSpinBox->value();
	*polyCurvature = curvatureSpinBox->value() / 100.0;
	*polyOuterCurvature = outerCurvatureSpinBox->value() / 100.0;
}

void PolygonWidget::setFactorSlider(double a)
{
	disconnect(factorSlider, SIGNAL(valueChanged(int)), factorSpinBox, SLOT(setValue(int)));
	factorSlider->setValue(static_cast<int>(a));
	connect(factorSlider, SIGNAL(valueChanged(int)), factorSpinBox, SLOT(setValue(int)));
}

void PolygonWidget::setRotationSlider(double a)
{
	disconnect(rotationSlider, SIGNAL(valueChanged(int)), rotationSpinBox, SLOT(setValue(int)));
	rotationSlider->setValue(static_cast<int>(a));
	connect(rotationSlider, SIGNAL(valueChanged(int)), rotationSpinBox, SLOT(setValue(int)));
}

void PolygonWidget::setInnerRotationSlider(double a)
{
	disconnect(innerRotationSlider, SIGNAL(valueChanged(int)), innerRotationSpinBox, SLOT(setValue(int)));
	innerRotationSlider->setValue(static_cast<int>(a));
	connect(innerRotationSlider, SIGNAL(valueChanged(int)), innerRotationSpinBox, SLOT(setValue(int)));
}

void PolygonWidget::setCurvatureSlider(double a)
{
	disconnect(curvatureSlider, SIGNAL(valueChanged(int)), curvatureSpinBox, SLOT(setValue(int)));
	curvatureSlider->setValue(static_cast<int>(a));
	connect(curvatureSlider, SIGNAL(valueChanged(int)), curvatureSpinBox, SLOT(setValue(int)));
}

void PolygonWidget::setOuterCurvatureSlider(double a)
{
	disconnect(outerCurvatureSlider, SIGNAL(valueChanged(int)), outerCurvatureSpinBox, SLOT(setValue(int)));
	outerCurvatureSlider->setValue(static_cast<int>(a));
	connect(outerCurvatureSlider, SIGNAL(valueChanged(int)), outerCurvatureSpinBox, SLOT(setValue(int)));
}

void PolygonWidget::updatePreview()
{
	if (editMode)
	{
		emit NewVectors(cornersSpinBox->value(), GetFactor(), applyConvexGroupBox->isChecked(), rotationSpinBox->value(), curvatureSpinBox->value() / 100.0, innerRotationSpinBox->value(), outerCurvatureSpinBox->value() / 100.0);
		return;
	}
	double roundness = curvatureSpinBox->value() / 100.0;
	double innerround = outerCurvatureSpinBox->value() / 100.0;
	QPixmap pm = QPixmap(Preview->width() - 5, Preview->height() - 5);
	pm.fill(Qt::white);
	QPainter p;
	p.begin(&pm);
	p.setBrush(Qt::NoBrush);
	p.setPen(Qt::black);
	QPainterPath pp = regularPolygonPath(Preview->width() - 6, Preview->height() - 6, cornersSpinBox->value(), applyConvexGroupBox->isChecked(), GetFactor(), rotationSlider->value(), roundness, innerRotationSpinBox->value(), innerround);
	QRectF br = pp.boundingRect();
	if (br.x() < 0)
	{
		QTransform m;
		m.translate(-br.x(), 0);
		pp = pp * m;
	}
	if (br.y() < 0)
	{
		QTransform m;
		m.translate(0, -br.y());
		pp = pp * m;
	}
	br = pp.boundingRect();
	if ((br.height() > Preview->height() - 6) || (br.width() > Preview->width() - 6))
	{
		QTransform ma;
		double sca = static_cast<double>(qMax(Preview->height() - 6, Preview->width() - 6)) / static_cast<double>(qMax(br.width(), br.height()));
		ma.scale(sca, sca);
		pp = pp * ma;
	}
	p.strokePath(pp, p.pen());
	p.end();
	Preview->setPixmap(pm);
}

double PolygonWidget::getUserValFromFactor(double factor)
{
	double userVal = 0.0;
	double mi = GetZeroFactor();
	double ma = GetMaxFactor();
	if ((factor / mi * 100.0 - 100.0) > 0)
		userVal = (factor - mi) / (ma - mi) * 100.0;
	else
		userVal = factor / mi * 100.0 - 100.0;
	return userVal;
}

double PolygonWidget::GetZeroFactor()
{
	return sqrt(pow(1.0,2.0)-pow(((sin((360.0/(cornersSpinBox->value()*2))/180* M_PI)* 2.0)/2.0),2.0));
}

double PolygonWidget::GetMaxFactor()
{
	double win = (360.0/(cornersSpinBox->value()*2)) / 180.0 * M_PI;
	double ret;
	if ((360.0/(cornersSpinBox->value()*2)) > 45)
		ret = 1/sin(win);
	else
		ret = 1/cos(win);
	return ret;
}

double PolygonWidget::GetFactor()
{
	int val = factorSlider->value();
	if (val < 0)
		PFactor = GetZeroFactor() * (100.0 + val) / 100.0;
	else
	{
		double ma = GetMaxFactor();
		double mi = GetZeroFactor();
		PFactor = ((ma - mi) * val / 100.0) + mi;
	}
	return PFactor;
}
