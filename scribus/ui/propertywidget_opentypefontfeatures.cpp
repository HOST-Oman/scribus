#include "propertywidget_opentypefontfeatures.h"
#include "appmodes.h"
#include "pageitem_table.h"
#include "iconmanager.h"
#include "scribus.h"
#include "scribusdoc.h"
#include "selection.h"
#include "units.h"



PropertyWidget_OpenTypeFontFeatures::PropertyWidget_OpenTypeFontFeatures(QWidget* parent) : QFrame(parent)
{
	m_item = NULL;
	m_ScMW = NULL;
	m_unitIndex = 0;
	m_unitRatio = 1.0;
	setupUi(this);

	setFrameStyle(QFrame::Box | QFrame::Plain);
	setLineWidth(1);
	layout()->setAlignment( Qt::AlignTop );
	languageChange();

}

void PropertyWidget_OpenTypeFontFeatures::setMainWindow(ScribusMainWindow *mw)
{
	m_ScMW = mw;
}

void PropertyWidget_OpenTypeFontFeatures::changeEvent(QEvent *e)
{
	if (e->type() == QEvent::LanguageChange)
	{
		languageChange();
		return;
	}
	QWidget::changeEvent(e);
}

void PropertyWidget_OpenTypeFontFeatures::languageChange()
{
		CommonCheck->setChecked(true);
		NormalCaRadio->setChecked(true);
		NormalRadio->setChecked(true);
		DefaultStyleRadio->setChecked(true);
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
		ProporionalRadio->setChecked(false);
		TabularRadio->setChecked(false);
		DiagonalRadio->setChecked(false);
		StackedRadio->setChecked(false);
		OrdinalCheck->setChecked(false);
		SlashedZeroCheck->setChecked(false);
		groupBox->setToolTip(tr("Capitals"));
		groupBox_2->setToolTip(tr("Numbers"));
		groupBox_3->setToolTip(tr("Ligatures"));
		groupBox_4->setToolTip(tr("Position"));
}

void PropertyWidget_OpenTypeFontFeatures::showFontFeatures(QString s)
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
			ProporionalRadio->setChecked(true);
		else if (fontFeatures[i] == "+tnum")
			TabularRadio->setChecked(true);
		else if (fontFeatures[i] == "+afrc")
			DiagonalRadio->setChecked(true);
		else if (fontFeatures[i] == "+frac")
			StackedRadio->setChecked(true);
		else if (fontFeatures[i] == "+ordn")
			OrdinalCheck->setChecked(true);
		else if (fontFeatures[i] == "+zero")
			SlashedZeroCheck->setChecked(true);
	}
}

void PropertyWidget_OpenTypeFontFeatures::handlefontfeatures()
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
	if (ProporionalRadio->isChecked())
		font_feature << "+pnum";
	if (TabularRadio->isChecked())
		font_feature << "+tnum";
	if (DiagonalRadio->isChecked())
		font_feature << "+afrc";
	if (StackedRadio->isChecked())
		font_feature <<"+frac";

	if (OrdinalCheck->isChecked())
		font_feature << "+ordn";
	if (SlashedZeroCheck->isChecked())
		font_feature << "+zero";

	Selection tempSelection(this, false);
	tempSelection.addItem(m_item, true);
	m_doc->itemSelection_SetFontFeatures(font_feature.join(","), &tempSelection);
}

void PropertyWidget_OpenTypeFontFeatures::setDoc(ScribusDoc *d)
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

	m_unitRatio   = m_doc->unitRatio();
	m_unitIndex   = m_doc->unitIndex();
	connect(m_doc->m_Selection, SIGNAL(selectionChanged()), this, SLOT(handleSelectionChanged()));
	connect(m_doc             , SIGNAL(docChanged())      , this, SLOT(handleSelectionChanged()));
}

void PropertyWidget_OpenTypeFontFeatures::handleSelectionChanged()
{
	if (!m_doc || !m_ScMW || m_ScMW->scriptIsRunning())
		return;

	PageItem* currItem = currentItemFromSelection();
	setCurrentItem(currItem);
	updateGeometry();
	repaint();
}

void PropertyWidget_OpenTypeFontFeatures::updateCharStyle(const CharStyle& charStyle)
{
	if (!m_ScMW || m_ScMW->scriptIsRunning())
		return;
	showFontFeatures(charStyle.fontFeatures());
}

void PropertyWidget_OpenTypeFontFeatures::updateStyle(const ParagraphStyle& newCurrent)
{
	if (!m_ScMW || m_ScMW->scriptIsRunning())
		return;

	const CharStyle& charStyle = newCurrent.charStyle();
	showFontFeatures(charStyle.fontFeatures());
}

void PropertyWidget_OpenTypeFontFeatures::connectSignals()
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
	connect(ProporionalRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(TabularRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(DefaultFractionsRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(DiagonalRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(StackedRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(OrdinalCheck, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	connect(SlashedZeroCheck, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));

}

void PropertyWidget_OpenTypeFontFeatures::disconnectSignals()
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
	disconnect(ProporionalRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(TabularRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(DefaultFractionsRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(DiagonalRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(StackedRadio, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(OrdinalCheck, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
	disconnect(SlashedZeroCheck, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
}

void PropertyWidget_OpenTypeFontFeatures::configureWidgets(void)
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

void PropertyWidget_OpenTypeFontFeatures::setCurrentItem(PageItem *item)
{
	if (!m_ScMW || m_ScMW->scriptIsRunning())
		return;
	//CB We shouldn't really need to process this if our item is the same one
	//maybe we do if the item has been changed by scripter.. but that should probably
	//set some status if so.
	//FIXME: This won't work until when a canvas deselect happens, m_item must be NULL.
	//if (m_item == i)
	//	return;

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
