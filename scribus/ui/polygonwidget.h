/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef POLYGONWIDGET_H
#define POLYGONWIDGET_H

#include <QWidget>
#include "scribusapi.h"
#include "ui_polygonwidgetbase.h"

struct ItemToolPrefs;

class SCRIBUS_API PolygonWidget : public QWidget, Ui::PolygonWidgetBase
{
	Q_OBJECT

	public:
		PolygonWidget(QWidget* parent);
		PolygonWidget(QWidget* parent, int polyCorners, double polyF, bool polyUseConvexFactor, double polyRotation, double polyCurvature, double polyInnerRot, double polyOuterCurvature, bool forEditMode = false);
		~PolygonWidget() {};

		void setValues(int polyCorners, double polyF, bool polyUseConvexFactor, double polyRotation, double polyCurvature, double polyInnerRot, double polyOuterCurvature);
		void getValues(int* polyCorners, double* polyF, bool* polyUseConvexFactor, double* polyRotation, double* polyCurvature, double* polyInnerRot, double* polyOuterCurvature);
		void restoreDefaults(const ItemToolPrefs *prefsData);
		void saveGuiToPrefs(ItemToolPrefs *prefsData);

	protected slots:
		void setFactorSlider(double a);
		void setRotationSlider(double a);
		void setInnerRotationSlider(double a);
		void setCurvatureSlider(double a);
		void setOuterCurvatureSlider(double a);
		void updatePreview();
		double GetZeroFactor();
		double GetMaxFactor();
		double GetFactor();

	protected:
		double getUserValFromFactor(double factor);
		double PFactor;
		void connectSignals(bool);
		bool editMode;

	signals:
		void NewVectors(int, double, bool, double, double, double, double);
};

#endif
