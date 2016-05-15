/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include "propertywidget_fontfeatures.h"
#include "appmodes.h"
#include "pageitem_table.h"
#include "iconmanager.h"
#include "scribus.h"
#include "scribusdoc.h"
#include "selection.h"

PropertyWidget_FontFeatures::PropertyWidget_FontFeatures(QWidget* parent) : QFrame(parent)
{
	m_item = NULL;
	m_ScMW = NULL;
	setupUi(this);

	setFrameStyle(QFrame::Box | QFrame::Plain);
	setLineWidth(1);
	layout()->setAlignment( Qt::AlignTop );
	languageChange();
}

void PropertyWidget_FontFeatures::setMainWindow(ScribusMainWindow *mw)
{
	m_ScMW = mw;
}

void PropertyWidget_FontFeatures::changeEvent(QEvent *e)
{
	if (e->type() == QEvent::LanguageChange)
	{
		languageChange();
		return;
	}
	QWidget::changeEvent(e);
}

void PropertyWidget_FontFeatures::languageChange()
{
	CommonCheck->setChecked(true);
	NormalCaRadio->setChecked(true);
	NormalRadio->setChecked(true);
	DefaultStyleRadio->setChecked(true);
	DefaultWidthRadio->setChecked(true);
	DefaultFractionsRadio->setChecked(true);
	ContextualCheck->setChecked(true);
	DiscretinoryCheck->setChecked(false);
	HistoricalCheck->setChecked(false);
	SubscriptRadio->setChecked(false);
	SuperscriptRaido->setChecked(false);
	SmallRadio->setChecked(false);
	SmallFromCRadio->setChecked(false);
	PetiteRadio->setChecked(false);
	PetiteCapRadio->setChecked(false);
	UnicaseRadio->setChecked(false);
	TiltingRadio->setChecked(false);
	LininRadio->setChecked(false);
	OldStyleRadio->setChecked(false);
	ProportionalRadio->setChecked(false);
	TabularRadio->setChecked(false);
	DiagonalRadio->setChecked(false);
	StackedRadio->setChecked(false);
	OrdinalCheck->setChecked(false);
	SlashedZeroCheck->setChecked(false);
	StyleSet01->setChecked(false);
	StyleSet02->setChecked(false);
	StyleSet03->setChecked(false);
	StyleSet04->setChecked(false);
	StyleSet05->setChecked(false);
	StyleSet06->setChecked(false);
	StyleSet07->setChecked(false);
	StyleSet08->setChecked(false);
	StyleSet09->setChecked(false);
	StyleSet10->setChecked(false);
	StyleSet11->setChecked(false);
	StyleSet12->setChecked(false);
	StyleSet13->setChecked(false);
	StyleSet14->setChecked(false);
	StyleSet15->setChecked(false);
	StyleSet16->setChecked(false);
	StyleSet17->setChecked(false);
	StyleSet18->setChecked(false);
	StyleSet19->setChecked(false);
	StyleSet20->setChecked(false);
}

void PropertyWidget_FontFeatures::showFontFeatures(QString s)
{
	if (!m_ScMW || m_ScMW->scriptIsRunning())
		return;
	QStringList fontFeatures = s.split(',');
	ContextualCheck->setChecked(true);
	CommonCheck->setChecked(true);
	for (int i = 0; i < fontFeatures.count(); i++)
	{
		if (fontFeatures[i] == "-clig")
			ContextualCheck->setChecked(false);
		else if (fontFeatures[i] == "-liga")
			CommonCheck->setChecked(false);
		else if (fontFeatures[i] == "+dlig")
			DiscretinoryCheck->setChecked(true);
		else if (fontFeatures[i] == "+hlig")
			HistoricalCheck->setChecked(true);
		else if (fontFeatures[i] == "+subs")
			SubscriptRadio->setChecked(true);
		else if (fontFeatures[i] == "+sups")
			SuperscriptRaido->setChecked(true);
		else if (fontFeatures[i] == "+smcp")
			SmallRadio->setChecked(true);
		else if (fontFeatures[i] == "+c2sc")
			SmallFromCRadio->setChecked(true);
		else if (fontFeatures[i] == "+pcap")
			PetiteRadio->setChecked(true);
		else if (fontFeatures[i] == "+c2pc")
			PetiteCapRadio->setChecked(true);
		else if (fontFeatures[i] == "+unic")
			UnicaseRadio->setChecked(true);
		else if (fontFeatures[i] == "+titl")
			TiltingRadio->setChecked(true);
		else if (fontFeatures[i] == "+lnum")
			LininRadio->setChecked(true);
		else if (fontFeatures[i] == "+onum")
			OldStyleRadio->setChecked(true);
		else if (fontFeatures[i] == "+pnum")
			ProportionalRadio->setChecked(true);
		else if (fontFeatures[i] == "+tnum")
			TabularRadio->setChecked(true);
		else if (fontFeatures[i] == "+frac")
			DiagonalRadio->setChecked(true);
		else if (fontFeatures[i] == "+afrc")
			StackedRadio->setChecked(true);
		else if (fontFeatures[i] == "+ordn")
			OrdinalCheck->setChecked(true);
		else if (fontFeatures[i] == "+zero")
			SlashedZeroCheck->setChecked(true);
		else if (fontFeatures[i] == "+ss01")
			StyleSet01->setChecked(true);
		else if (fontFeatures[i] == "+ss02")
			StyleSet02->setChecked(true);
		else if (fontFeatures[i] == "+ss03")
			StyleSet03->setChecked(true);
		else if (fontFeatures[i] == "+ss04")
			StyleSet04->setChecked(true);
		else if (fontFeatures[i] == "+ss05")
			StyleSet05->setChecked(true);
		else if (fontFeatures[i] == "+ss06")
			StyleSet06->setChecked(true);
		else if (fontFeatures[i] == "+ss07")
			StyleSet07->setChecked(true);
		else if (fontFeatures[i] == "+ss08")
			StyleSet08->setChecked(true);
		else if (fontFeatures[i] == "+ss09")
			StyleSet09->setChecked(true);
		else if (fontFeatures[i] == "+ss10")
			StyleSet10->setChecked(true);
		else if (fontFeatures[i] == "+ss11")
			StyleSet11->setChecked(true);
		else if (fontFeatures[i] == "+ss12")
			StyleSet12->setChecked(true);
		else if (fontFeatures[i] == "+ss13")
			StyleSet13->setChecked(true);
		else if (fontFeatures[i] == "+ss14")
			StyleSet14->setChecked(true);
		else if (fontFeatures[i] == "+ss15")
			StyleSet15->setChecked(true);
		else if (fontFeatures[i] == "+ss16")
			StyleSet16->setChecked(true);
		else if (fontFeatures[i] == "+ss17")
			StyleSet17->setChecked(true);
		else if (fontFeatures[i] == "+ss18")
			StyleSet18->setChecked(true);
		else if (fontFeatures[i] == "+ss19")
			StyleSet19->setChecked(true);
		else if (fontFeatures[i] == "+ss20")
			StyleSet20->setChecked(true);
	}
}

void PropertyWidget_FontFeatures::handlefontfeatures()
{
	if (!m_doc || !m_item || !m_ScMW || m_ScMW->scriptIsRunning())
		return;
	QStringList font_feature ;
	if (!ContextualCheck->isChecked())
		font_feature << "-clig";
	if (!CommonCheck->isChecked())
		font_feature << "-liga";
	if (DiscretinoryCheck->isChecked())
		font_feature << "+dlig";
	if (HistoricalCheck->isChecked())
		font_feature << "+hlig";

	//Position

	if (SubscriptRadio->isChecked())
		font_feature << "+subs";
	if (SuperscriptRaido->isChecked())
		font_feature <<"+sups";


	//Capitals
	if (SmallRadio->isChecked())
		font_feature << "+smcp";
	if (SmallFromCRadio->isChecked())
		font_feature << "+c2sc";
	if (PetiteRadio->isChecked())
		font_feature << "+pcap";
	if (PetiteCapRadio->isChecked())
		font_feature << "+c2pc";
	if (UnicaseRadio->isChecked())
		font_feature << "+unic";
	if (TiltingRadio->isChecked())
		font_feature << "+titl";

	//Numeric
	if (LininRadio->isChecked())
		font_feature << "+lnum";
	if (OldStyleRadio->isChecked())
		font_feature << "+onum";
	if (ProportionalRadio->isChecked())
		font_feature << "+pnum";
	if (TabularRadio->isChecked())
		font_feature << "+tnum";
	if (DiagonalRadio->isChecked())
		font_feature << "+frac";
	if (StackedRadio->isChecked())
		font_feature <<"+afrc";

	if (OrdinalCheck->isChecked())
		font_feature << "+ordn";
	if (SlashedZeroCheck->isChecked())
		font_feature << "+zero";

	// Stylistic sets
	if (StyleSet01->isChecked())
		font_feature << "+ss01";
	if (StyleSet02->isChecked())
		font_feature << "+ss02";
	if (StyleSet03->isChecked())
		font_feature << "+ss03";
	if (StyleSet04->isChecked())
		font_feature << "+ss04";
	if (StyleSet05->isChecked())
		font_feature << "+ss05";
	if (StyleSet06->isChecked())
		font_feature << "+ss06";
	if (StyleSet07->isChecked())
		font_feature << "+ss07";
	if (StyleSet08->isChecked())
		font_feature << "+ss08";
	if (StyleSet09->isChecked())
		font_feature << "+ss09";
	if (StyleSet10->isChecked())
		font_feature << "+ss10";
	if (StyleSet11->isChecked())
		font_feature << "+ss11";
	if (StyleSet12->isChecked())
		font_feature << "+ss12";
	if (StyleSet13->isChecked())
		font_feature << "+ss13";
	if (StyleSet14->isChecked())
		font_feature << "+ss14";
	if (StyleSet15->isChecked())
		font_feature << "+ss15";
	if (StyleSet16->isChecked())
		font_feature << "+ss16";
	if (StyleSet17->isChecked())
		font_feature << "+ss17";
	if (StyleSet18->isChecked())
		font_feature << "+ss18";
	if (StyleSet19->isChecked())
		font_feature << "+ss19";
	if (StyleSet20->isChecked())
		font_feature << "+ss20";

	Selection tempSelection(this, false);
	tempSelection.addItem(m_item, true);
	m_doc->itemSelection_SetFontFeatures(font_feature.join(","), &tempSelection);
}

void PropertyWidget_FontFeatures::setDoc(ScribusDoc *d)
{
	if((d == (ScribusDoc*) m_doc) || (m_ScMW && m_ScMW->scriptIsRunning()))
		return;

	if (m_doc)
	{
		disconnect(m_doc->m_Selection, SIGNAL(selectionChanged()), this, SLOT(handleSelectionChanged()));
		disconnect(m_doc             , SIGNAL(docChanged())      , this, SLOT(handleSelectionChanged()));
	}

	m_doc  = d;
	m_item = NULL;

	if (m_doc.isNull())
	{
		disconnectSignals();
		return;
	}

	connect(m_doc->m_Selection, SIGNAL(selectionChanged()), this, SLOT(handleSelectionChanged()));
	connect(m_doc             , SIGNAL(docChanged())      , this, SLOT(handleSelectionChanged()));
}

void PropertyWidget_FontFeatures::handleSelectionChanged()
{
	if (!m_doc || !m_ScMW || m_ScMW->scriptIsRunning())
		return;

	PageItem* currItem = currentItemFromSelection();
	setCurrentItem(currItem);
	updateGeometry();
	repaint();
}

void PropertyWidget_FontFeatures::updateCharStyle(const CharStyle& charStyle)
{
	if (!m_ScMW || m_ScMW->scriptIsRunning())
		return;
	showFontFeatures(charStyle.fontFeatures());
}

void PropertyWidget_FontFeatures::updateStyle(const ParagraphStyle& newCurrent)
{
	if (!m_ScMW || m_ScMW->scriptIsRunning())
		return;

	const CharStyle& charStyle = newCurrent.charStyle();
	showFontFeatures(charStyle.fontFeatures());
}

void PropertyWidget_FontFeatures::connectSignals()
{
	connect(ContextualCheck, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(CommonCheck, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(DiscretinoryCheck, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(HistoricalCheck, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(NormalRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(SubscriptRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(SuperscriptRaido, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(NormalCaRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(SmallRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(SmallFromCRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(PetiteRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(PetiteCapRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(UnicaseRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(TiltingRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(DefaultStyleRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(LininRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(OldStyleRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(DefaultWidthRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(ProportionalRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(TabularRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(DefaultFractionsRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(DiagonalRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StackedRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(OrdinalCheck, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(SlashedZeroCheck, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet01, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet02, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet03, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet04, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet05, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet06, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet07, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet08, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet09, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet10, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet11, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet12, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet13, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet14, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet15, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet16, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet17, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet18, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet19, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StyleSet20, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
}

void PropertyWidget_FontFeatures::disconnectSignals()
{
	disconnect(ContextualCheck, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(CommonCheck, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(DiscretinoryCheck, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(HistoricalCheck, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(NormalRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(SubscriptRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(SuperscriptRaido, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(NormalCaRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(SmallRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(SmallFromCRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(PetiteRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(PetiteCapRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(UnicaseRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(TiltingRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(DefaultStyleRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(LininRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(OldStyleRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(DefaultWidthRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(ProportionalRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(TabularRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(DefaultFractionsRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(DiagonalRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StackedRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(OrdinalCheck, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(SlashedZeroCheck, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet01, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet02, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet03, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet04, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet05, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet06, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet07, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet08, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet09, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet10, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet11, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet12, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet13, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet14, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet15, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet16, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet17, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet18, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet19, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StyleSet20, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
}

void PropertyWidget_FontFeatures::configureWidgets(void)
{
	bool enabled = false;
	if (m_item && m_doc)
	{
		if (m_item->asPathText() || m_item->asTextFrame() || m_item->asTable())
			enabled = true;
		if ((m_item->isGroup()) && (!m_item->isSingleSel))
			enabled = false;
		if (m_item->asOSGFrame() || m_item->asSymbolFrame())
			enabled = false;
		if (m_doc->m_Selection->count() > 1)
			enabled = false;
	}
	setEnabled(enabled);
}

void PropertyWidget_FontFeatures::setCurrentItem(PageItem *item)
{
	if (!m_ScMW || m_ScMW->scriptIsRunning())
		return;

	if (item && m_doc.isNull())
		setDoc(item->doc());

	m_item = item;

	disconnectSignals();
	configureWidgets();

	if (m_item)
	{
		if (m_item->asTextFrame() || m_item->asPathText() || m_item->asTable())
		{
			ParagraphStyle parStyle =  m_item->itemText.defaultStyle();
			if (m_doc->appMode == modeEdit)
				m_item->currentTextProps(parStyle);
			else if (m_doc->appMode == modeEditTable)
				m_item->asTable()->activeCell().textFrame()->currentTextProps(parStyle);
			updateStyle(parStyle);
		}
		connectSignals();
	}
}
