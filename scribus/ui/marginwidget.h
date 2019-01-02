/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef MARGINWIDGET_H
#define MARGINWIDGET_H

#include "scribusapi.h"
#include "scribusstructs.h"

#include <QComboBox>
#include <QTabWidget>
#include "linkbutton.h"
#include "marginpresetlayout.h"
class QCheckBox;
class QPushButton;
class QLabel;
class QGridLayout;
class QHBoxLayout;
class ScrSpinBox;


/*! \brief Widget for Margins setting.
Used e.g. in "New Doc Dialog" or "Preferences".
*/
class SCRIBUS_API MarginWidget : public QTabWidget
{
	Q_OBJECT

public:
	/*! \brief It creates an user interface for base margin settings for the document.
	\param parent Parent widget (used as standard in Qt)
	\param title Title of the Group box
	\param margs A reference to the margins in the preferences.
	\param unitIndex and index of the unit combobox
	\param showChangeAll show "apply to all pages" related items
	*/
	MarginWidget(QWidget* parent, const QString& title, const MarginStruct* margs, int unitIndex, bool showChangeAll=false, bool showBleeds=true);
	~MarginWidget() {}
	/*! \brief Setup the labels by facing pages option */
	void setFacingPages(bool facing, int pagetype = 0);
	/*! \brief Setup the spinboxes properties (min/max value etc.) by width and height */
	void setPageWidthHeight(double width, double height);
	/*! \brief Setup the spinboxes properties (min/max value etc.) by width */
	void setPageWidth(double width);
	/*! \brief Setup the spinboxes properties (min/max value etc.) by height */
	void setPageHeight(double heigth);
	/*! \brief Setup the widgets by new options */
	void setNewUnit(int newUnitIndex);
	/*! \brief Set the page size for margin getting from cups */
	void setPageSize(const QString& pageSize);
	
	/*! \brief Margins
	\retval MarginStruct data */
	const MarginStruct& margins() const;
	/*! \brief Top Margin
	\retval double margin size */
	double top() const;
	/*! \brief Bottom Margin
	\retval double margin size */
	double bottom() const;
	/*! \brief Left Margin
	\retval double margin size */
	double left() const;
	/*! \brief Right Margin
	\retval double margin size */
	double right() const;

	/*! \brief set new margin values for the dialog
	It calls overridden setTop(), setBottom() etc. slots itself
	via signals emitted from spinboxes.
	\param m MarginStruct 
	*/
	void setNewMargins(MarginStruct& m);
	
	/*! \brief Setup the presetCombo without changing the margin values, only used by tabdocument */
	void setMarginPreset(int p);
	int getMarginPreset();
	
	/*! \brief Top Bleed
	\retval double bleed size */
	double topBleed() const;
	/*! \brief Bottom Bleed
	\retval double bleed size */
	double bottomBleed() const;
	/*! \brief Left Bleed
	\retval double bleed size */
	double leftBleed() const;
	/*! \brief Right Bleed
	\retval double bleed size */
	double rightBleed() const;

	/*! \brief set new bleed values for the dialog
	\param b MarginStruct
	*/
	void setNewBleeds(MarginStruct& b);
	
	/*! \brief Return marginsForAllPages property
	\retval bool true to apply for all pages */
	bool getMarginsForAllPages() const;
	bool getMarginsForAllMasterPages() const;

private:
	/*! Tabs */
	QWidget *marginPage;
	QWidget* bleedPage;
	/*! Spinboxes */
	ScrSpinBox* topR;
	ScrSpinBox* bottomR;
	ScrSpinBox* rightR;
	ScrSpinBox* leftR;
	ScrSpinBox* BleedBottom;
	ScrSpinBox* BleedLeft;
	ScrSpinBox* BleedRight;
	ScrSpinBox* BleedTop;
	PresetLayout* presetCombo;
	/*! Labels */
	QLabel* lText;
	QLabel* rText;
	QLabel* tText;
	QLabel* bText;
	QLabel* presetLabel;
	QLabel* marginsForPages;
	QLabel* BleedTxt1;
	QLabel* BleedTxt2;
	QLabel* BleedTxt3;
	QLabel* BleedTxt4;
	QCheckBox* marginsForAllPages;
	QCheckBox* marginsForAllMasterPages;
	QPushButton* usePrinterMarginsButton;
	LinkButton* linkBleeds;
	LinkButton* linkMargins;
	/*! \brief Margin values converted by unitRatio */
	MarginStruct marginData;
	/*
	//! \brief Top margin value converted by unitRatio
	double RandT;
	//! \brief Bottom margin value converted by unitRatio
	double RandB;
	//! \brief Left margin value converted by unitRatio
	double RandL;
	//! \brief Right margin value converted by unitRatio
	double RandR;
	*/
	/*! \brief Internally used page width */
	double pageWidth;
	/*! \brief Internally used page height */
	double pageHeight;

public slots:
	/*! \brief Recompute the values after spinbox change */
	void setTop();
	void setBottom();
	void setLeft();
	void setRight();
	/*! \brief Recompute margins in PresetLayout combobox and disable/enable widgets. */
	void setPreset();
	void slotLinkBleeds();
	void slotLinkMargins();
	void changeBleeds();
	
protected slots:	
	virtual void setMarginsToPrinterMargins();

protected:
	void updateMarginSpinValues();
	QGridLayout* GroupLayout;
	QHBoxLayout* marginsForPagesLayout;
	QGridLayout* BleedGroupLayout;
	QString m_pageSize;
	int m_unitIndex;
	double m_unitRatio;
	//! \brief if the outer facing palette points to the facing pages item
	bool facingPages;
	bool useBleeds;
	int pageType;
	MarginStruct savedMargins;
	int savedPresetItem;
};

#endif
