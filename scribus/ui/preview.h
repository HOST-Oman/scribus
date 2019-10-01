/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef PRVIEW_H
#define PRVIEW_H

#include <QDialog>
#include <QCheckBox>
#include <QMap>

class QHBoxLayout;
class QVBoxLayout;
class QGroupBox;
class QTableWidget;
class QScrollArea;
class QLabel;
class QPushButton;
class QComboBox;
#include "ui/scrspinbox.h"
#include "scribusapi.h"
#include "scribusstructs.h"

class PageSelector;
class ScribusDoc;
class ScribusView;
class ScImage;
class ScColor;
class PrefsManager;
// enum  PrintEngine;



//! \brief Print Preview dialog
class SCRIBUS_API PPreview : public QDialog
{
	Q_OBJECT

public:
	/*!
	\author Franz Schmid
	\brief Create the Print Preview window
	\param parent QWidget *
	\param vin ScribusView *
	\param docu ScribusDoc *
	\param printer a name of the printer
	\param engine a printer engine
	*/
	PPreview(QWidget* parent, ScribusView *vin, ScribusDoc *docu, const QString& printer, PrintEngine engine );
	~PPreview() {};
	/*!
	\author Franz Schmid
	\brief Renders the Preview to a file on Disk
	\param pageIndex int page number
	\param res int resolution
	\retval int Flag indicating error
	*/
	int RenderPreview(int pageIndex, int res);
	int RenderPreviewSep(int pageIndex, int res);
	void blendImages(QImage &target, ScImage &source, ScColor col);
	void blendImagesSumUp(QImage &target, ScImage &scsource);
	static bool usePostscriptPreview(const QString& printerName, PrintEngine engine);
	/*!
	\author Franz Schmid
	\brief Creates the Preview of the Actual Page
	\param pageIndex int page number
	\param res int resolution
	\retval pixmap QPixmap print preview
	*/
	QPixmap CreatePreview(int pageIndex, int res);
	PageSelector *PGSel;
	QCheckBox* AntiAlias;
	QCheckBox* AliasTr;
	QCheckBox* EnableCMYK;
	QCheckBox* EnableGCR;
	QCheckBox* MirrorHor;
	QCheckBox* MirrorVert;
	QCheckBox* ClipMarg;
	QCheckBox* spotColors;
	QCheckBox* useGray;
	QCheckBox* EnableInkCover;
	ScrSpinBox* CoverThresholdValue;
	QLabel* ThresLabel;
	QScrollArea* Anzeige;
	QLabel* Anz;
	QGroupBox* devTitle;
	QGroupBox* jobTitle;
	QPushButton *closeButton;
	QPushButton *printButton;
	/*! scaling GUI */
	QLabel* scaleLabel;
	QComboBox* scaleBox;
	ScribusView *view;
	ScribusDoc *doc;
	bool HavePngAlpha;
	bool HaveTiffSep;
	int APage;
	int MPage;
	int SMode;
	int GsVersion;
	int inkMax;
	bool CMode;
	bool GsAl;
	bool Trans;
	bool GMode;
	bool mHor;
	bool mVer;
	bool fClip;
	bool fSpot;
	bool fGray;
	bool postscriptPreview;
	QMap<QString, int> sepsToFileNum;
	QMap<QString, QCheckBox*> flagsVisible;
	QTableWidget* Table;

public slots:
	/*!
	\author Franz Schmid
	\brief Jump to newly selected page and create the new preview
	\param num int Page Number
	*/
	void ToSeite(int num);
	/*!
	\author Franz Schmid
	\brief Create the new preview
	*/
	void redisplay();
	/*!
	\author Craig Bradney
	\brief When CMYK preview is toggled, (dis)enable the CMYK controls and create the new preview
	*/
	void ToggleCMYK();
	/*!
	\author Craig Bradney
	\brief If CMYK preview is enabled, create a new preview with the new CMYK plate settings
	*/
	void ToggleCMYK_Colour();
	void doSpotTable(int row);
	void toggleAllfromHeader();
	/*!
	\author Petr Vanek
	\date 09/03/2005
	\brief Recompute scaling factor of the preview image
	\param value spinbox value from signal
	*/
	void scaleBox_valueChanged(int value);

signals:
	void doPrint();

protected:
	/*! \brief Percentage value of the scaling widget */
	double scaleFactor;
	QVBoxLayout* PLayout;
	QVBoxLayout* Layout1;
	QVBoxLayout* Layout2;
	QHBoxLayout* Layout5;
	QHBoxLayout* Layout6;
	QHBoxLayout* Layout7;
	QVBoxLayout* settingsBarLayout;
	PrefsManager& prefsManager;

	void setValues();
	void getUserSelection(int);
	void imageLoadError(QPixmap &, int);

	//! \brief repaint sample on the dialog change
	void resizeEvent(QResizeEvent * event);

};

#endif
