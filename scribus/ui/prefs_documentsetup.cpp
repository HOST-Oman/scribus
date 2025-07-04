/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include <algorithm>

#include <QButtonGroup>
#include <QFileDialog>

#include "ui/prefs_documentsetup.h"
#include "commonstrings.h"
#include "ui/newmarginwidget.h"
#include "langmgr.h"
#include "pagesize.h"
#include "prefsfile.h"
#include "prefsmanager.h"
#include "prefsstructs.h"
#include "scribusdoc.h"
#include "undomanager.h"
#include "units.h"
#include "util.h"
#include "util_text.h"

Prefs_DocumentSetup::Prefs_DocumentSetup(QWidget* parent, ScribusDoc* doc)
	: Prefs_Pane(parent),
	  m_doc(doc)
{
	setupUi(this);
	
	scrollArea->viewport()->setAutoFillBackground(false);
	scrollArea->widget()->setAutoFillBackground(false);

	m_caption = tr("Document Setup");
	m_icon = "pref-document-setup";

	if (!m_doc)
	{
		applySizesToAllPagesCheckBox->hide();
		applySizesToAllMasterPagesCheckBox->hide();
		applyMarginsToAllPagesCheckBox->hide();
		applyMarginsToAllMasterPagesCheckBox->hide();
		pageSizeLinkToolButton->hide(); //temp
//		connect(pageSizeLinkToolButton, SIGNAL(clicked()), this, SLOT(emitSectionChange()));
	}
	else
	{
		pageSizeLinkToolButton->hide();
		emergencyCheckBox->hide();
	}

	QStringList languageList;
	LanguageManager::instance()->fillInstalledStringList(&languageList);
	std::sort(languageList.begin(), languageList.end(), localeAwareLessThan);
	languageComboBox->addItems( languageList );

	pageLayoutButtonGroup->setId(singlePageRadioButton,0);
	pageLayoutButtonGroup->setId(facingPagesRadioButton,1);
	pageLayoutButtonGroup->setId(threeFoldRadioButton,2);
	pageLayoutButtonGroup->setId(fourFoldRadioButton,3);
	singlePageRadioButton->setChecked(true);
	layoutFirstPageIsComboBox->clear();
	layoutFirstPageIsComboBox->addItem(" ");
	layoutFirstPageIsComboBox->setCurrentIndex(0);
	layoutFirstPageIsComboBox->setEnabled(false);

	layoutBindingIsComboBox->clear();
	layoutBindingIsComboBox->addItem(" ");
	layoutBindingIsComboBox->setCurrentIndex(0);
	layoutBindingIsComboBox->setEnabled(false);

	pageWidthSpinBox->setMaximum(16777215);
	pageHeightSpinBox->setMaximum(16777215);
	languageChange();

	connect(pageSizeSelector, SIGNAL(pageSizeChanged(QString)), this, SLOT(setPageSize()));
	connect(pageOrientationComboBox, SIGNAL(activated(int)), this, SLOT(setPageOrientation(int)));
	connect(pageWidthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setPageWidth(double)));
	connect(pageHeightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setPageHeight(double)));
	connect(pageLayoutButtonGroup, SIGNAL(idClicked(int)), this, SLOT(pageLayoutChanged(int)));
	connect(pageUnitsComboBox, SIGNAL(activated(int)), this, SLOT(unitChange()));
	connect(undoCheckBox, SIGNAL(toggled(bool)), this, SLOT(slotUndo(bool)));
	connect(changeAutoDir, SIGNAL(clicked()), this, SLOT(changeAutoDocDir()));
}

void Prefs_DocumentSetup::unitChange()
{
	pageWidthSpinBox->blockSignals(true);
	pageHeightSpinBox->blockSignals(true);

	int docUnitIndex = pageUnitsComboBox->currentIndex();
	pageWidthSpinBox->setNewUnit(docUnitIndex);
	pageHeightSpinBox->setNewUnit(docUnitIndex);
	unitRatio = unitGetRatioFromIndex(docUnitIndex);
	pageWidthSpinBox->setValue(pageW * unitRatio);
	pageHeightSpinBox->setValue(pageH * unitRatio);
	marginsWidget->setNewUnit(docUnitIndex);
	marginsWidget->setPageHeight(pageH);
	marginsWidget->setPageWidth(pageW);
	bleedsWidget->setNewUnit(docUnitIndex);
	bleedsWidget->setPageHeight(pageH);
	bleedsWidget->setPageWidth(pageW);

	pageWidthSpinBox->blockSignals(false);
	pageHeightSpinBox->blockSignals(false);

	emit prefsChangeUnits(docUnitIndex);
}

void Prefs_DocumentSetup::languageChange()
{
	int i = 0;

	i = pageOrientationComboBox->currentIndex();
	pageOrientationComboBox->clear();
	pageOrientationComboBox->addItem( tr( "Portrait" ) );
	pageOrientationComboBox->addItem( tr( "Landscape" ) );
	pageOrientationComboBox->setCurrentIndex(i < 0 ? 0 : i);

	i = pageUnitsComboBox->currentIndex();
	pageUnitsComboBox->clear();
	pageUnitsComboBox->addItems(unitGetTextUnitList());
	pageUnitsComboBox->setCurrentIndex(i < 0 ? 0 : i);

	setupPageSets();

	pageWidthSpinBox->setToolTip( "<qt>" + tr( "Width of document pages, editable if you have chosen a custom page size" ) + "</qt>" );
	pageHeightSpinBox->setToolTip( "<qt>" + tr( "Height of document pages, editable if you have chosen a custom page size" ) + "</qt>" );
	pageSizeSelector->setToolTip( "<qt>" + tr( "Default page size, either a standard size or a custom size. More page sizes can be made visible by activating them in Preferences." ) + "</qt>" );
	pageSizeLinkToolButton->setToolTip( "<qt>" + tr( "Enable or disable more page sizes by jumping to Page Size preferences" ) + "</qt>" );
	pageOrientationComboBox->setToolTip( "<qt>" + tr( "Default orientation of document pages" ) + "</qt>" );
	pageUnitsComboBox->setToolTip( "<qt>" + tr( "Default unit of measurement for document editing" ) + "</qt>" );
	autosaveCheckBox->setToolTip( "<qt>" + tr( "When enabled, Scribus saves backup copies of your file each time the time period elapses" ) + "</qt>" );
	autosaveIntervalSpinBox->setToolTip( "<qt>" + tr( "Time period between saving automatically" ) + "</qt>" );
	undoLengthSpinBox->setToolTip( "<qt>" + tr("Set the length of the action history in steps. If set to 0 infinite amount of actions will be stored.") + "</qt>");
	applySizesToAllPagesCheckBox->setToolTip( "<qt>" + tr( "Apply the page size changes to all existing pages in the document" ) + "</qt>" );
	applyMarginsToAllPagesCheckBox->setToolTip( "<qt>" + tr( "Apply the page size changes to all existing master pages in the document" ) + "</qt>" );
	autosaveCountSpinBox->setToolTip("<qt>" + tr("Keep this many files during the editing session. Backup files will be removed when you close the document.") + "</qt>");
}

void Prefs_DocumentSetup::restoreDefaults(struct ApplicationPrefs *prefsData)
{
	pageWidthSpinBox->blockSignals(true);
	pageHeightSpinBox->blockSignals(true);
	pageOrientationComboBox->blockSignals(true);
	pageSizeSelector->blockSignals(true);

	setCurrentComboItem(languageComboBox, LanguageManager::instance()->getLangFromAbbrev(prefsData->docSetupPrefs.language, true));

	unitRatio = unitGetRatioFromIndex(prefsData->docSetupPrefs.docUnitIndex);
	setupPageSizes(prefsData);

	pageOrientationComboBox->setCurrentIndex(prefsData->docSetupPrefs.pageOrientation);
	pageUnitsComboBox->setCurrentIndex(prefsData->docSetupPrefs.docUnitIndex);
	pageW = prefsData->docSetupPrefs.pageWidth;
	pageH = prefsData->docSetupPrefs.pageHeight;
	pageWidthSpinBox->setValue(pageW * unitRatio);
	pageHeightSpinBox->setValue(pageH * unitRatio);
	pageSets = prefsData->pageSets;
	if (prefsData->docSetupPrefs.pagePositioning < 2)
	{
		threeFoldRadioButton->hide();
		fourFoldRadioButton->hide();
	}
	switch (prefsData->docSetupPrefs.pagePositioning)
	{
		case 0:
			singlePageRadioButton->setChecked(true);
			break;
		case 1:
			facingPagesRadioButton->setChecked(true);
			break;
		case 2:
			threeFoldRadioButton->setChecked(true);
			break;
		case 3:
			fourFoldRadioButton->setChecked(true);
			break;
	}
	setupPageSets();

	layoutFirstPageIsComboBox->setCurrentIndex(prefsData->pageSets[prefsData->docSetupPrefs.pagePositioning].FirstPage);
	layoutBindingIsComboBox->setCurrentIndex(prefsData->docSetupPrefs.docBindingDirection);

	pageWidthSpinBox->blockSignals(false);
	pageHeightSpinBox->blockSignals(false);
	pageOrientationComboBox->blockSignals(false);
	pageSizeSelector->blockSignals(false);

	marginsWidget->setup(prefsData->docSetupPrefs.margins, prefsData->docSetupPrefs.pagePositioning, prefsData->docSetupPrefs.docUnitIndex, NewMarginWidget::MarginWidgetFlags);
	marginsWidget->setPageWidth(prefsData->docSetupPrefs.pageWidth);
	marginsWidget->setPageHeight(prefsData->docSetupPrefs.pageHeight);
//	marginsWidget->setPageSize(prefsPageSizeName);
	marginsWidget->setMarginPreset(prefsData->docSetupPrefs.marginPreset);
	bleedsWidget->setup(prefsData->docSetupPrefs.bleeds, prefsData->docSetupPrefs.pagePositioning, prefsData->docSetupPrefs.docUnitIndex, NewMarginWidget::BleedWidgetFlags);
	bleedsWidget->setPageWidth(prefsData->docSetupPrefs.pageWidth);
	bleedsWidget->setPageHeight(prefsData->docSetupPrefs.pageHeight);
//	bleedsWidget->setPageSize(prefsPageSizeName);
	bleedsWidget->setMarginPreset(prefsData->docSetupPrefs.marginPreset);
	saveCompressedCheckBox->setChecked(prefsData->docSetupPrefs.saveCompressed);
	emergencyCheckBox->setChecked(prefsData->miscPrefs.saveEmergencyFile);
	autosaveCheckBox->setChecked( prefsData->docSetupPrefs.AutoSave );
	autosaveIntervalSpinBox->setValue(prefsData->docSetupPrefs.AutoSaveTime / 1000 / 60);
	autosaveCountSpinBox->setValue(prefsData->docSetupPrefs.AutoSaveCount);
	autosaveKeepCheckBox->setChecked(prefsData->docSetupPrefs.AutoSaveKeep);
	autosaveDocRadio->setChecked(prefsData->docSetupPrefs.AutoSaveLocation);
	autosaveDirRadio->setChecked(!prefsData->docSetupPrefs.AutoSaveLocation);
	autosaveDirEdit->setText(prefsData->docSetupPrefs.AutoSaveDir);
	autosaveDirEdit->setEnabled(!prefsData->docSetupPrefs.AutoSaveLocation);
	changeAutoDir->setEnabled(!prefsData->docSetupPrefs.AutoSaveLocation);
	showAutosaveClockOnCanvasCheckBox->setChecked(prefsData->displayPrefs.showAutosaveClockOnCanvas);
	undoCheckBox->setChecked(PrefsManager::instance().prefsFile->getContext("undo")->getBool("enabled", true));
	int undoLength = UndoManager::instance()->getHistoryLength();
	if (undoLength == -1)
		undoLengthSpinBox->setEnabled(false);
	else
		undoLengthSpinBox->setValue(undoLength);
	unitChange();
}

void Prefs_DocumentSetup::saveGuiToPrefs(struct ApplicationPrefs *prefsData) const
{
	prefsData->docSetupPrefs.language = LanguageManager::instance()->getAbbrevFromLang(languageComboBox->currentText(), false);
	prefsData->docSetupPrefs.pageSize = prefsPageSizeName;
	prefsData->docSetupPrefs.pageOrientation = pageOrientationComboBox->currentIndex();
	prefsData->docSetupPrefs.docUnitIndex = pageUnitsComboBox->currentIndex();
	prefsData->docSetupPrefs.pageWidth = pageW;
	prefsData->docSetupPrefs.pageHeight = pageH;
	prefsData->docSetupPrefs.pagePositioning = pageLayoutButtonGroup->checkedId();
	prefsData->pageSets[prefsData->docSetupPrefs.pagePositioning].FirstPage = layoutFirstPageIsComboBox->currentIndex();
	prefsData->docSetupPrefs.docBindingDirection = layoutBindingIsComboBox->currentIndex();

	prefsData->docSetupPrefs.margins = marginsWidget->margins();
	prefsData->docSetupPrefs.bleeds = bleedsWidget->margins();
	prefsData->docSetupPrefs.saveCompressed = saveCompressedCheckBox->isChecked();
	prefsData->miscPrefs.saveEmergencyFile = emergencyCheckBox->isChecked();
	prefsData->docSetupPrefs.AutoSave = autosaveCheckBox->isChecked();
	prefsData->docSetupPrefs.AutoSaveTime = autosaveIntervalSpinBox->value() * 1000 * 60;
	prefsData->docSetupPrefs.AutoSaveCount = autosaveCountSpinBox->value();
	prefsData->docSetupPrefs.AutoSaveKeep = autosaveKeepCheckBox->isChecked();
	prefsData->docSetupPrefs.AutoSaveLocation = autosaveDocRadio->isChecked();
	prefsData->docSetupPrefs.AutoSaveDir = autosaveDirEdit->text();
	prefsData->displayPrefs.showAutosaveClockOnCanvas = showAutosaveClockOnCanvasCheckBox->isChecked();
	bool undoActive = undoCheckBox->isChecked();
	if (!undoActive)
		UndoManager::instance()->clearStack();
	UndoManager::instance()->setUndoEnabled(undoActive);
	UndoManager::instance()->setAllHistoryLengths(undoLengthSpinBox->value());
	static PrefsContext *undoPrefs = PrefsManager::instance().prefsFile->getContext("undo");
	undoPrefs->set("enabled", undoActive);
}

void Prefs_DocumentSetup::setupPageSets()
{
	int i = layoutFirstPageIsComboBox->currentIndex();
	if (!layoutFirstPageIsComboBox->isEnabled())
		i = -1;
	int currIndex = pageLayoutButtonGroup->checkedId() < 0 ? 0 :pageLayoutButtonGroup->checkedId();
	layoutFirstPageIsComboBox->clear();
	layoutBindingIsComboBox->clear();
	if (currIndex > 0 && currIndex < pageSets.count())
	{
		const PageSet& pageSet = pageSets.at(currIndex);
		const QStringList& pageNames = pageSet.pageNames;
		layoutFirstPageIsComboBox->setEnabled(true);
		layoutBindingIsComboBox->setEnabled(true);
		for (const QString& pageName : pageNames)
		{
			layoutFirstPageIsComboBox->addItem(CommonStrings::translatePageSetLocString(pageName));
		}
		layoutBindingIsComboBox->addItem(CommonStrings::translateDocBindingLocString(CommonStrings::docLoc_LTR_Binding));
		layoutBindingIsComboBox->addItem(CommonStrings::translateDocBindingLocString(CommonStrings::docLoc_RTL_Binding));

		int firstPageIndex = i < 0 ? pageSet.FirstPage : i;
		int bindingIndex = i < 0 ? m_doc->docBindingDirection() : i;
		firstPageIndex = qMax(0, qMin(firstPageIndex, pageSet.pageNames.count() - 1));
		bindingIndex = qMax(0, qMin(bindingIndex, pageSet.pageNames.count() - 1));

		layoutFirstPageIsComboBox->setCurrentIndex(firstPageIndex);
		layoutBindingIsComboBox->setCurrentIndex(bindingIndex);
	}
	else
	{
		layoutFirstPageIsComboBox->addItem(" ");
		layoutFirstPageIsComboBox->setCurrentIndex(0);
		layoutFirstPageIsComboBox->setEnabled(false);
		layoutBindingIsComboBox->addItem(" ");
		layoutBindingIsComboBox->setCurrentIndex(0);
		layoutBindingIsComboBox->setEnabled(false);
	}
}

void Prefs_DocumentSetup::setupPageSizes(struct ApplicationPrefs *prefsData)
{
	prefsPageSizeName = prefsData->docSetupPrefs.pageSize;

	PageSize ps(prefsPageSizeName);

	// try to find coresponding page size by dimensions
	if (ps.name() == CommonStrings::customPageSize)
	{
		PageSizeInfoMap pages = ps.sizesByDimensions(QSize(prefsData->docSetupPrefs.pageWidth, prefsData->docSetupPrefs.pageHeight));
		if (pages.count() > 0)
			prefsPageSizeName = pages.firstKey();
	}

	pageSizeSelector->setPageSize(prefsPageSizeName);
	marginsWidget->setPageSize(prefsPageSizeName);
	bleedsWidget->setPageSize(prefsPageSizeName);
}

void Prefs_DocumentSetup::pageLayoutChanged(int i)
{
	setupPageSets();
	marginsWidget->setFacingPages(i != singlePage);
	//layoutFirstPageIsComboBox->setCurrentIndex(pageSets[pageLayoutButtonGroup->checkedId()].FirstPage);
}

void Prefs_DocumentSetup::setPageWidth(double w)
{
	pageW = pageWidthSpinBox->value() / unitRatio;
	marginsWidget->setPageWidth(pageW);
	QString psText = pageSizeSelector->pageSizeTR();
	if (psText != CommonStrings::trCustomPageSize && psText != CommonStrings::customPageSize)
		pageSizeSelector->setPageSize(CommonStrings::customPageSize);
	int newOrientation = (pageWidthSpinBox->value() > pageHeightSpinBox->value()) ? landscapePage : portraitPage;
	if (newOrientation != pageOrientationComboBox->currentIndex())
	{
		pageOrientationComboBox->blockSignals(true);
		pageOrientationComboBox->setCurrentIndex(newOrientation);
		pageOrientationComboBox->blockSignals(false);
	}
}

void Prefs_DocumentSetup::setPageHeight(double h)
{
	pageH = pageHeightSpinBox->value() / unitRatio;
	marginsWidget->setPageHeight(pageH);
	QString psText = pageSizeSelector->pageSizeTR();
	if (psText != CommonStrings::trCustomPageSize && psText != CommonStrings::customPageSize)
		pageSizeSelector->setPageSize(CommonStrings::customPageSize);
	int newOrientation = (pageWidthSpinBox->value() > pageHeightSpinBox->value()) ? landscapePage : portraitPage;
	if (newOrientation != pageOrientationComboBox->currentIndex())
	{
		pageOrientationComboBox->blockSignals(true);
		pageOrientationComboBox->setCurrentIndex(newOrientation);
		pageOrientationComboBox->blockSignals(false);
	}
}

void Prefs_DocumentSetup::setPageOrientation(int orientation)
{
	setSize(pageSizeSelector->pageSize());
	pageWidthSpinBox->blockSignals(true);
	pageHeightSpinBox->blockSignals(true);
	if ((orientation==0 && pageSizeSelector->pageSizeTR() == CommonStrings::trCustomPageSize) || orientation!=0)
	{
		double w = pageWidthSpinBox->value(), h = pageHeightSpinBox->value();
		pageWidthSpinBox->setValue((orientation == portraitPage) ? qMin(w, h) : qMax(w, h));
		pageHeightSpinBox->setValue((orientation == portraitPage) ? qMax(w, h) : qMin(w, h));
	}
	pageW = pageWidthSpinBox->value() / unitRatio;
	pageH = pageHeightSpinBox->value() / unitRatio;
	pageWidthSpinBox->blockSignals(false);
	pageHeightSpinBox->blockSignals(false);
}

void Prefs_DocumentSetup::setPageSize()
{
	setPageOrientation(pageOrientationComboBox->currentIndex());
}

void Prefs_DocumentSetup::setSize(const QString &newSize)
{
	pageW = pageWidthSpinBox->value() / unitRatio;
	pageH = pageHeightSpinBox->value() / unitRatio;

	PageSize ps2(newSize);
	prefsPageSizeName = ps2.name();
	if (newSize != CommonStrings::customPageSize && newSize != CommonStrings::trCustomPageSize)
	{
		pageW = ps2.width();
		pageH = ps2.height();
	}
	else
		prefsPageSizeName = CommonStrings::customPageSize;

	pageWidthSpinBox->blockSignals(true);
	pageHeightSpinBox->blockSignals(true);
	pageWidthSpinBox->setValue(pageW * unitRatio);
	pageHeightSpinBox->setValue(pageH * unitRatio);
	marginsWidget->setPageHeight(pageH);
	marginsWidget->setPageWidth(pageW);
	marginsWidget->setPageSize(newSize);
	pageWidthSpinBox->blockSignals(false);
	pageHeightSpinBox->blockSignals(false);
}

void Prefs_DocumentSetup::slotUndo(bool isEnabled)
{
	undoLengthSpinBox->setEnabled(isEnabled);
}

void Prefs_DocumentSetup::getResizeDocumentPages(bool &resizePages, bool &resizeMasterPages, bool &resizePageMargins, bool &resizeMasterPageMargins)
{
	resizePages = applySizesToAllPagesCheckBox->isChecked();
	resizeMasterPages = applySizesToAllMasterPagesCheckBox->isChecked();
	resizePageMargins = applyMarginsToAllPagesCheckBox->isChecked();
	resizeMasterPageMargins = applyMarginsToAllMasterPagesCheckBox->isChecked();
}

void Prefs_DocumentSetup::changeAutoDocDir()
{
	QString s = QFileDialog::getExistingDirectory(this, tr("Choose a Directory"), autosaveDirEdit->text());
	if (!s.isEmpty())
		autosaveDirEdit->setText( QDir::toNativeSeparators(s) );
}

void Prefs_DocumentSetup::emitSectionChange()
{
	emit changeToOtherSection("Prefs_PageSizes");
}
