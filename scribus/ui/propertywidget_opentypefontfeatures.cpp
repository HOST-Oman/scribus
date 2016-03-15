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

void PropertyWidget_OpenTypeFontFeatures::showFontFeatures(QString s)
{
	if (!m_ScMW || m_ScMW->scriptIsRunning())
		return;
	fontfeatures->setText(s);
}

void PropertyWidget_OpenTypeFontFeatures::handlefontfeatures()
{
	if (!m_doc || !m_item || !m_ScMW || m_ScMW->scriptIsRunning())
		return;
	Selection tempSelection(this, false);
	tempSelection.addItem(m_item, true);
	m_doc->itemSelection_SetFontFeatures(fontfeatures->text(), &tempSelection);
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
	connect(submit, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
}

void PropertyWidget_OpenTypeFontFeatures::disconnectSignals()
{
	disconnect(submit, SIGNAL(clicked()), this, SLOT(handlefontfeatures()));
}

void PropertyWidget_OpenTypeFontFeatures::languageChange()
{
	fontfeatures->setText(tr("FontFeatures"));
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
