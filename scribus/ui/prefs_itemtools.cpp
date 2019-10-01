/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include "ui/prefs_itemtools.h"
#include "ui/arrowchooser.h"
#include "ui/linkbutton.h"
#include "prefsstructs.h"
#include "scrspinbox.h"
#include "scribusdoc.h"
#include "units.h"
#include "commonstrings.h"
#include "prefsmanager.h"
#include "sampleitem.h"


Prefs_ItemTools::Prefs_ItemTools(QWidget* parent, ScribusDoc* doc)
	: Prefs_Pane(parent),
	m_doc(nullptr),
	showFontPreview(false)
{
	setupUi(this);
	languageChange();

	m_caption = tr("Item Tools");
	m_icon = "tools_16.png";

	lineEndArrowComboBox->setStartDirection(false);
	shapeLineWidthSpinBox->setNewUnit(0);
	shapeLineWidthSpinBox->setMaximum(36);
	shapeLineWidthSpinBox->setDecimals(1);
	lineWidthSpinBox->setNewUnit(0);
	lineWidthSpinBox->setMaximum(36);
	lineWidthSpinBox->setDecimals(1);
	calPenLineWidthSpinBox->setNewUnit(0);
	calPenLineWidthSpinBox->setMaximum(36);
	calPenLineWidthSpinBox->setDecimals(1);
	calPenWidthSpinBox->setNewUnit(0);
	calPenWidthSpinBox->setMaximum(100);
	calPenWidthSpinBox->setDecimals(1);
	calPenAngleSpinBox->setNewUnit(6);
	calPenAngleSpinBox->setMaximum(180);
	calPenAngleSpinBox->setMinimum(-180);
	calPenAngleSpinBox->setDecimals(0);
	shapeLineWidthSpinBox->setSpecialValueText( tr("Hairline"));
	lineWidthSpinBox->setSpecialValueText( tr("Hairline"));
	calPenLineWidthSpinBox->setSpecialValueText( tr("Hairline"));
	textSizeSpinBox->setNewUnit(0);
	textSizeSpinBox->setMinimum(0.5);
	textSizeSpinBox->setMaximum(2048);
}

Prefs_ItemTools::~Prefs_ItemTools() = default;


void Prefs_ItemTools::languageChange()
{
	textPreviewWidget->setText( tr( "Woven silk pyjamas exchanged for blue quartz" ));
	scalingLockToolButton->setToolTip( tr( "Keep horizontal and vertical scaling the same" ) );
}

void Prefs_ItemTools::unitChange(int newIndex)
{
	textColumnGapSpinBox->setNewUnit(newIndex);
	textTabWidthSpinBox->setNewUnit(newIndex);
	topTextDistanceSpinBox->setNewUnit(newIndex);
	bottomTextDistanceSpinBox->setNewUnit(newIndex);
	leftTextDistanceSpinBox->setNewUnit(newIndex);
	rightTextDistanceSpinBox->setNewUnit(newIndex);
}

void Prefs_ItemTools::restoreDefaults(struct ApplicationPrefs *prefsData)
{
	enableSignals(false);
	int docUnitIndex = prefsData->docSetupPrefs.docUnitIndex;
	double unitRatio = unitGetRatioFromIndex(docUnitIndex);
	unitChange(docUnitIndex);

	PrefsManager& prefsManager=PrefsManager::instance();
	ColorList* colorList = (m_doc != nullptr) ? (&m_doc->PageColors) : prefsManager.colorSetPtr();

	//Text Tool
	for (int i=0; i<textFontComboBox->count(); ++i)
	{
		if (textFontComboBox->itemText(i) == prefsData->itemToolPrefs.textFont)
		{
			textFontComboBox->setCurrentIndex(i);
			break;
		}
	}
	textSizeSpinBox->setValue(prefsData->itemToolPrefs.textSize / 10);
	textColorComboBox->initColorList(colorList, m_doc, prefsData->itemToolPrefs.textColor);
	textColorShadingSpinBox->setValue(prefsData->itemToolPrefs.textShade);
	textStrokeColorComboBox->initColorList(colorList, m_doc, prefsData->itemToolPrefs.textStrokeColor);
	textStrokeShadingSpinBox->setValue(prefsData->itemToolPrefs.textStrokeShade);
	textFrameFillColorComboBox->initColorList(colorList, m_doc, prefsData->itemToolPrefs.textFillColor);
	textFrameFillShadingSpinBox->setValue(prefsData->itemToolPrefs.textFillColorShade);
	textFrameLineColorComboBox->initColorList(colorList, m_doc, prefsData->itemToolPrefs.textLineColor);
	textFrameLineShadingSpinBox->setValue(prefsData->itemToolPrefs.textLineColorShade);
	textTabFillCharComboBox->clear();
	textTabFillCharComboBox->addItem( tr("None", "tab fill" ));
	textTabFillCharComboBox->addItem( tr("Dot"));
	textTabFillCharComboBox->addItem( tr("Hyphen"));
	textTabFillCharComboBox->addItem( tr("Underscore"));
	textTabFillCharComboBox->addItem( tr("Custom"));
	if (prefsData->itemToolPrefs.textTabFillChar.isEmpty())
	{
		textTabFillCharComboBox->setCurrentIndex(0);
	}
	else if (prefsData->itemToolPrefs.textTabFillChar == ".")
	{
		textTabFillCharComboBox->setCurrentIndex(1);
	}
	else if (prefsData->itemToolPrefs.textTabFillChar == "-")
	{
		textTabFillCharComboBox->setCurrentIndex(2);
	}
	else if (prefsData->itemToolPrefs.textTabFillChar == "_")
	{
		textTabFillCharComboBox->setCurrentIndex(3);
	}
	else
	{
		textTabFillCharComboBox->setCurrentIndex(4);
		textTabFillCharComboBox->setEditable(true);
		textTabFillCharComboBox->setItemText(textTabFillCharComboBox->currentIndex(), CommonStrings::trCustomTabFill + prefsData->itemToolPrefs.textTabFillChar);
	}
	textTabWidthSpinBox->setValue(prefsData->itemToolPrefs.textTabWidth * unitRatio);
	textColumnsSpinBox->setValue(prefsData->itemToolPrefs.textColumns);
	textColumnGapSpinBox->setValue(prefsData->itemToolPrefs.textColumnGap * unitRatio);
	topTextDistanceSpinBox->setValue(prefsData->itemToolPrefs.textDistances.top() * unitRatio);
	bottomTextDistanceSpinBox->setValue(prefsData->itemToolPrefs.textDistances.bottom() * unitRatio);
	leftTextDistanceSpinBox->setValue(prefsData->itemToolPrefs.textDistances.left() * unitRatio);
	rightTextDistanceSpinBox->setValue(prefsData->itemToolPrefs.textDistances.right() * unitRatio);

	//Image Tool
	imageFreeScalingRadioButton->setChecked( prefsData->itemToolPrefs.imageScaleType );
	imageFrameScalingRadioButton->setChecked( !prefsData->itemToolPrefs.imageScaleType );
	imageHorizontalScalingSpinBox->setValue(qRound(prefsData->itemToolPrefs.imageScaleX * 100));
	imageVerticalScalingSpinBox->setValue(qRound(prefsData->itemToolPrefs.imageScaleY * 100));
	imageKeepAspectRatioCheckBox->setChecked(prefsData->itemToolPrefs.imageAspectRatio);
	scalingLockToolButton->setChecked(prefsData->itemToolPrefs.imageAspectRatio);
	imageFrameFillColorComboBox->initColorList(colorList, m_doc, prefsData->itemToolPrefs.imageFillColor);
	imageFrameFillShadingSpinBox->setValue(prefsData->itemToolPrefs.imageFillColorShade );
	imageFrameLineColorComboBox->initColorList(colorList, m_doc, prefsData->itemToolPrefs.imageStrokeColor);
	imageFrameLineShadingSpinBox->setValue(prefsData->itemToolPrefs.imageStrokeColorShade );
	imageUseEmbeddedClippingPathCheckBox->setChecked(prefsData->itemToolPrefs.imageUseEmbeddedPath);
	switch (prefsData->itemToolPrefs.imageLowResType)
	{
		case 0:
			onscreenResolutionFullRadioButton->setChecked(true);
			break;
		case 1:
			onscreenResolutionNormalRadioButton->setChecked(true);
			break;
		case 2:
			onscreenResolutionLowRadioButton->setChecked(true);
			break;
	}

	//Shape Tool
	shapeFrameLineColorComboBox->initColorList(colorList, m_doc, prefsData->itemToolPrefs.shapeLineColor);
	shapeFrameLineShadingSpinBox->setValue(prefsData->itemToolPrefs.shapeLineColorShade);
	shapeFrameFillColorComboBox->initColorList(colorList, m_doc, prefsData->itemToolPrefs.shapeFillColor);
	shapeFrameFillShadingSpinBox->setValue(prefsData->itemToolPrefs.shapeFillColorShade);
	shapeFrameLineStyleComboBox->setCurrentIndex(static_cast<int>(prefsData->itemToolPrefs.shapeLineStyle) - 1);
	shapeLineWidthSpinBox->setValue(prefsData->itemToolPrefs.shapeLineWidth);

	//Polygon Tool
	polygonWidget->restoreDefaults(&prefsData->itemToolPrefs);

	//Line Tool
	lineFillColorComboBox->initColorList(colorList, m_doc, prefsData->itemToolPrefs.lineColor);

	lineFillColorShadingSpinBox->setValue(prefsData->itemToolPrefs.lineColorShade);
	lineStyleComboBox->setCurrentIndex(static_cast<int>(prefsData->itemToolPrefs.lineStyle) - 1);
	if (m_doc != nullptr)
	{
		lineStartArrowComboBox->rebuildList(&m_doc->arrowStyles());
		lineEndArrowComboBox->rebuildList(&m_doc->arrowStyles());
	}
	else
	{
		lineStartArrowComboBox->rebuildList(&prefsData->arrowStyles);
		lineEndArrowComboBox->rebuildList(&prefsData->arrowStyles);
	}
	lineStartArrowComboBox->setCurrentIndex(prefsData->itemToolPrefs.lineStartArrow);
	lineEndArrowComboBox->setCurrentIndex(prefsData->itemToolPrefs.lineEndArrow);
	lineWidthSpinBox->setValue(prefsData->itemToolPrefs.lineWidth);
	//
	//Calligraphic Pen Tool
	calPenFillColorComboBox->initColorList(colorList, m_doc, prefsData->itemToolPrefs.calligraphicPenFillColor);
	calPenFillShadingSpinBox->setValue(prefsData->itemToolPrefs.calligraphicPenFillColorShade);
	calPenColorComboBox->initColorList(colorList, m_doc, prefsData->itemToolPrefs.calligraphicPenLineColor);
	calPenLineShadingSpinBox->setValue(prefsData->itemToolPrefs.calligraphicPenLineColorShade);
	calPenStyleComboBox->setCurrentIndex(static_cast<int>(prefsData->itemToolPrefs.calligraphicPenStyle) - 1);
	calPenLineWidthSpinBox->setValue(prefsData->itemToolPrefs.calligraphicPenLineWidth);
	calPenAngleSpinBox->setValue(prefsData->itemToolPrefs.calligraphicPenAngle);
	calPenWidthSpinBox->setValue(prefsData->itemToolPrefs.calligraphicPenWidth);

	// Arc Tool
	arcDisplay->restoreDefaults(&prefsData->itemToolPrefs);
	// Spiral Tool
	spiralDisplay->restoreDefaults(&prefsData->itemToolPrefs);
	imageScalingTypeChange();
	enableSignals(true);
}

void Prefs_ItemTools::saveGuiToPrefs(struct ApplicationPrefs *prefsData) const
{

	//Text Tool
	prefsData->itemToolPrefs.textFont = textFontComboBox->currentText();
	prefsData->itemToolPrefs.textSize = textSizeSpinBox->value() * 10;
	prefsData->itemToolPrefs.textFillColor = textFrameFillColorComboBox->currentText();
	if (prefsData->itemToolPrefs.textFillColor == CommonStrings::tr_NoneColor)
		prefsData->itemToolPrefs.textFillColor = CommonStrings::None;
	prefsData->itemToolPrefs.textLineColor = textFrameLineColorComboBox->currentText();
	if (prefsData->itemToolPrefs.textLineColor == CommonStrings::tr_NoneColor)
		prefsData->itemToolPrefs.textLineColor = CommonStrings::None;
	prefsData->itemToolPrefs.textFillColorShade = textFrameFillShadingSpinBox->value();
	prefsData->itemToolPrefs.textLineColorShade = textFrameLineShadingSpinBox->value();
	prefsData->itemToolPrefs.textShade = textColorShadingSpinBox->value();
	prefsData->itemToolPrefs.textStrokeShade = textStrokeShadingSpinBox->value();
	prefsData->itemToolPrefs.textColor = textColorComboBox->currentText();
	if (prefsData->itemToolPrefs.textColor == CommonStrings::tr_NoneColor)
		prefsData->itemToolPrefs.textColor = CommonStrings::None;
	prefsData->itemToolPrefs.textStrokeColor = textStrokeColorComboBox->currentText();
	if (prefsData->itemToolPrefs.textStrokeColor == CommonStrings::tr_NoneColor)
		prefsData->itemToolPrefs.textStrokeColor = CommonStrings::None;
	prefsData->itemToolPrefs.textColumns = textColumnsSpinBox->value();
	int docUnitIndex = prefsData->docSetupPrefs.docUnitIndex;
	double unitRatio = unitGetRatioFromIndex(docUnitIndex);
	prefsData->itemToolPrefs.textColumnGap = textColumnGapSpinBox->value() / unitRatio;
	prefsData->itemToolPrefs.textTabWidth = textTabWidthSpinBox->value() / unitRatio;

	switch (textTabFillCharComboBox->currentIndex())
	{
		case 0:
			prefsData->itemToolPrefs.textTabFillChar = "";
			break;
		case 1:
			prefsData->itemToolPrefs.textTabFillChar = ".";
			break;
		case 2:
			prefsData->itemToolPrefs.textTabFillChar = "-";
			break;
		case 3:
			prefsData->itemToolPrefs.textTabFillChar = "_";
			break;
		case 4:
			prefsData->itemToolPrefs.textTabFillChar = textTabFillCharComboBox->currentText().right(1);
			break;
	}
	prefsData->itemToolPrefs.textDistances.setTop(topTextDistanceSpinBox->value() / unitRatio);
	prefsData->itemToolPrefs.textDistances.setBottom(bottomTextDistanceSpinBox->value() / unitRatio);
	prefsData->itemToolPrefs.textDistances.setLeft(leftTextDistanceSpinBox->value() / unitRatio);
	prefsData->itemToolPrefs.textDistances.setRight(rightTextDistanceSpinBox->value() / unitRatio);
	prefsData->itemToolPrefs.imageFillColor = imageFrameFillColorComboBox->currentText();
	if (prefsData->itemToolPrefs.imageFillColor == CommonStrings::tr_NoneColor)
		prefsData->itemToolPrefs.imageFillColor = CommonStrings::None;
	prefsData->itemToolPrefs.imageFillColorShade = imageFrameFillShadingSpinBox->value();
	prefsData->itemToolPrefs.imageStrokeColor = imageFrameLineColorComboBox->currentText();
	if (prefsData->itemToolPrefs.imageStrokeColor == CommonStrings::tr_NoneColor)
		prefsData->itemToolPrefs.imageStrokeColor = CommonStrings::None;
	prefsData->itemToolPrefs.imageStrokeColorShade = imageFrameFillShadingSpinBox->value();
	prefsData->itemToolPrefs.imageScaleX = static_cast<double>(imageHorizontalScalingSpinBox->value()) / 100.0;
	prefsData->itemToolPrefs.imageScaleY = static_cast<double>(imageVerticalScalingSpinBox->value()) / 100.0;
	prefsData->itemToolPrefs.imageScaleType = imageFreeScalingRadioButton->isChecked();
	prefsData->itemToolPrefs.imageAspectRatio = imageKeepAspectRatioCheckBox->isChecked();
	prefsData->itemToolPrefs.imageUseEmbeddedPath = imageUseEmbeddedClippingPathCheckBox->isChecked();
	int haRes = 0;
	if (onscreenResolutionFullRadioButton->isChecked())
		haRes = 0;
	if (onscreenResolutionNormalRadioButton->isChecked())
		haRes = 1;
	if (onscreenResolutionLowRadioButton->isChecked())
		haRes = 2;
	prefsData->itemToolPrefs.imageLowResType = haRes;


	//Shape tool
	prefsData->itemToolPrefs.shapeLineColor = shapeFrameLineColorComboBox->currentText();
	if (prefsData->itemToolPrefs.shapeLineColor == CommonStrings::tr_NoneColor)
		prefsData->itemToolPrefs.shapeLineColor = CommonStrings::None;
	prefsData->itemToolPrefs.shapeFillColor = shapeFrameFillColorComboBox->currentText();
	if (prefsData->itemToolPrefs.shapeFillColor == CommonStrings::tr_NoneColor)
		prefsData->itemToolPrefs.shapeFillColor = CommonStrings::None;
	prefsData->itemToolPrefs.shapeFillColorShade = shapeFrameFillShadingSpinBox->value();
	prefsData->itemToolPrefs.shapeLineColorShade = shapeFrameLineShadingSpinBox->value();
	prefsData->itemToolPrefs.shapeLineStyle = static_cast<Qt::PenStyle>(shapeFrameLineStyleComboBox->currentIndex()) + 1;
	prefsData->itemToolPrefs.shapeLineWidth = shapeLineWidthSpinBox->value();



	//Polygon Tool
	polygonWidget->saveGuiToPrefs(&prefsData->itemToolPrefs);

	//Line Tool
	prefsData->itemToolPrefs.lineColor = lineFillColorComboBox->currentText();
	if (prefsData->itemToolPrefs.lineColor == CommonStrings::tr_NoneColor)
		prefsData->itemToolPrefs.lineColor = CommonStrings::None;
	prefsData->itemToolPrefs.lineColorShade = lineFillColorShadingSpinBox->value();
	prefsData->itemToolPrefs.lineStyle = static_cast<Qt::PenStyle>(lineStyleComboBox->currentIndex()) + 1;
	prefsData->itemToolPrefs.lineWidth = lineWidthSpinBox->value();
	prefsData->itemToolPrefs.lineStartArrow = lineStartArrowComboBox->currentIndex();
	prefsData->itemToolPrefs.lineEndArrow = lineEndArrowComboBox->currentIndex();
	//Calligraphic Pen Tool
	prefsData->itemToolPrefs.calligraphicPenFillColor = calPenFillColorComboBox->currentText();
	if (prefsData->itemToolPrefs.calligraphicPenFillColor == CommonStrings::tr_NoneColor)
		prefsData->itemToolPrefs.calligraphicPenFillColor = CommonStrings::None;
	prefsData->itemToolPrefs.calligraphicPenLineColor = calPenColorComboBox->currentText();
	if (prefsData->itemToolPrefs.calligraphicPenLineColor == CommonStrings::tr_NoneColor)
		prefsData->itemToolPrefs.calligraphicPenLineColor = CommonStrings::None;
	prefsData->itemToolPrefs.calligraphicPenFillColorShade = calPenFillShadingSpinBox->value();
	prefsData->itemToolPrefs.calligraphicPenLineColorShade = calPenLineShadingSpinBox->value();
	prefsData->itemToolPrefs.calligraphicPenStyle = static_cast<Qt::PenStyle>(calPenStyleComboBox->currentIndex()) + 1;
	prefsData->itemToolPrefs.calligraphicPenLineWidth = calPenLineWidthSpinBox->value();
	prefsData->itemToolPrefs.calligraphicPenAngle = calPenAngleSpinBox->value();
	prefsData->itemToolPrefs.calligraphicPenWidth = calPenWidthSpinBox->value();

	//Arc Tool
	arcDisplay->saveGuiToPrefs(&prefsData->itemToolPrefs);
	//Spiral Tool
	spiralDisplay->saveGuiToPrefs(&prefsData->itemToolPrefs);
}

void Prefs_ItemTools::enableSignals(bool on)
{
	if (on)
	{
		connect(textFontComboBox, SIGNAL(activated(int)), this, SLOT(updateFontPreview()));
		connect(textSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateFontPreview()));
		connect(textColorComboBox, SIGNAL(activated(int)), this, SLOT(updateFontPreview()));
		connect(textFrameFillColorComboBox, SIGNAL(activated(int)), this, SLOT(updateFontPreview()));
		connect(textFrameFillShadingSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateFontPreview()));
		connect(textColorShadingSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateFontPreview()));
		connect(scalingLockToolButton, SIGNAL(clicked()), this, SLOT(toggleImagesScalingChain()));
		connect(imageHorizontalScalingSpinBox, SIGNAL(valueChanged(int)), this, SLOT(imageHorizontalScalingChange()));
		connect(imageKeepAspectRatioCheckBox, SIGNAL(toggled(bool)), scalingLockToolButton, SLOT(setChecked(bool)));
		connect(imageVerticalScalingSpinBox, SIGNAL(valueChanged(int)), this, SLOT(imageVerticalScalingChange()));
		connect(imageFreeScalingRadioButton, SIGNAL(clicked(bool)), this, SLOT(imageScalingTypeChange()));
		connect(imageFrameScalingRadioButton, SIGNAL(clicked(bool)), this, SLOT(imageScalingTypeChange()));
	}
	else
	{
		disconnect(textFontComboBox, SIGNAL(activated(int)), this, SLOT(updateFontPreview()));
		disconnect(textSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateFontPreview()));
		disconnect(textColorComboBox, SIGNAL(activated(int)), this, SLOT(updateFontPreview()));
		disconnect(textFrameFillColorComboBox, SIGNAL(activated(int)), this, SLOT(updateFontPreview()));
		disconnect(textFrameFillShadingSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateFontPreview()));
		disconnect(textColorShadingSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateFontPreview()));
		disconnect(scalingLockToolButton, SIGNAL(clicked()), this, SLOT(toggleImagesScalingChain()));
		disconnect(imageHorizontalScalingSpinBox, SIGNAL(valueChanged(int)), this, SLOT(imageHorizontalScalingChange()));
		disconnect(imageVerticalScalingSpinBox, SIGNAL(valueChanged(int)), this, SLOT(imageVerticalScalingChange()));
	}
}

void Prefs_ItemTools::enableFontPreview(bool showPreview)
{
	showFontPreview = showPreview;
	updateFontPreview();
}

void Prefs_ItemTools::updateFontPreview()
{
	if (!showFontPreview)
		return;

	SampleItem si;
	si.setText( tr("Woven silk pyjamas exchanged for blue quartz"));
	if (textFrameFillColorComboBox->currentText() != CommonStrings::tr_NoneColor)
	{
		if (m_doc != nullptr)
		{
			si.setBgColor(m_doc->PageColors[textFrameFillColorComboBox->currentText()].getRawRGBColor());
		}
		else
		{
			PrefsManager& prefsManager=PrefsManager::instance();
			ColorList* colorList=prefsManager.colorSetPtr();
			si.setBgColor((*colorList)[textFrameFillColorComboBox->currentText()].getRawRGBColor());
		}
		si.setBgShade(textFrameFillShadingSpinBox->value());
	}
	else
		si.setBgColor(palette().color(QPalette::Window));

	if (textColorComboBox->currentText() != CommonStrings::tr_NoneColor)
	{
		if (m_doc != nullptr)
		{
			si.setTxColor(m_doc->PageColors[textColorComboBox->currentText()].getRawRGBColor());
		}
		else
		{
			PrefsManager& prefsManager=PrefsManager::instance();
			ColorList* colorList=prefsManager.colorSetPtr();
			si.setTxColor((*colorList)[textColorComboBox->currentText()].getRawRGBColor());
		}
		si.setTxShade(textColorShadingSpinBox->value());
	}
	else
		si.setTxColor(palette().color(QPalette::Window));
	si.setFont(textFontComboBox->currentText());
	si.setFontSize(textSizeSpinBox->value() * 10, true);
	textPreviewWidget->setPixmap(si.getSample(textPreviewWidget->width(), textPreviewWidget->height()));
}

void Prefs_ItemTools::toggleImagesScalingChain()
{
	imageHorizontalScalingChange();
	imageKeepAspectRatioCheckBox->setChecked(scalingLockToolButton->isChecked());
}

void Prefs_ItemTools::imageHorizontalScalingChange()
{
	if (scalingLockToolButton->isChecked())
		imageVerticalScalingSpinBox->setValue(imageHorizontalScalingSpinBox->value());
}

void Prefs_ItemTools::imageVerticalScalingChange()
{
	if (scalingLockToolButton->isChecked())
		imageHorizontalScalingSpinBox->setValue(imageVerticalScalingSpinBox->value());
}

void Prefs_ItemTools::imageScalingTypeChange()
{
	bool b=imageFreeScalingRadioButton->isChecked();
	imageKeepAspectRatioCheckBox->setEnabled(!b);
	imageHorizontalScalingSpinBox->setEnabled(b);
	imageVerticalScalingSpinBox->setEnabled(b);
	scalingLockToolButton->setEnabled(b);
}


