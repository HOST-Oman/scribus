/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include "newdocdialog.h"

#include <utility>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QFrame>
#include <QGroupBox>
#include <QLabel>
#include <QListWidgetItem>
#include <QPixmap>
#include <QPoint>
#include <QPushButton>
#include <QSpacerItem>
#include <QSpinBox>
#include <QStandardPaths>
#include <QStringList>
#include <QTabWidget>
#include <QToolTip>
#include <QVBoxLayout>
#include <QWindow>

#include "commonstrings.h"
#include "filedialogeventcatcher.h"
#include "fileloader.h"
#include "iconmanager.h"
#include "newmarginwidget.h"
#include "pagesize.h"
#include "pagestructs.h"
#include "prefsfile.h"
#include "prefsmanager.h"
#include "scrspinbox.h"
#include "units.h"
#include "ui/widgets/pagesizelist.h"


NewDocDialog::NewDocDialog(QWidget* parent, const QStringList& recentDocs, bool startUp, const QString& lang) : ScDialog(parent, "NewDocumentWindow"),
	prefsManager(PrefsManager::instance()),
	m_onStartup(startUp)
{
	setupUi(this);

	IconManager &iconManager = IconManager::instance();

	setModal(true);
	setWindowTitle( tr( "New Document" ) );

	m_labelVisibity = prefsManager.appPrefs.uiPrefs.showLabels;
	m_unitIndex = prefsManager.appPrefs.docSetupPrefs.docUnitIndex;
	m_unitRatio = unitGetRatioFromIndex(m_unitIndex);
	m_unitSuffix = unitGetSuffixFromIndex(m_unitIndex);
	m_orientation = prefsManager.appPrefs.docSetupPrefs.pageOrientation;

	buttonVertical->setIcon(iconManager.loadIcon("page-orientation-vertical"));
	buttonHorizontal->setIcon(iconManager.loadIcon("page-orientation-horizontal"));
	buttonSinglePage->setIcon(iconManager.loadIcon("page-simple"));
	buttonDoublePageLeft->setIcon(iconManager.loadIcon("page-first-left"));
	buttonDoublePageRight->setIcon(iconManager.loadIcon("page-doublesided"));
	buttonLTRBinding->setIcon(iconManager.loadIcon("page-binding-left"));
	buttonRTLBinding->setIcon(iconManager.loadIcon("page-binding-right"));
	labelColumns->setPixmap(iconManager.loadPixmap("paragraph-columns"));

	// for now we just hide buttonDoublePageLeft button
	buttonDoublePageLeft->setVisible(false);
	// disable LTR & RTL unless facing page is enabled
	buttonLTRBinding->setDisabled(true);
	buttonRTLBinding->setDisabled(true);

	createNewDocPage();
	if (startUp)
	{
		nftGui->setupSettings(lang);
		createOpenDocPage();
		recentDocList = recentDocs;
		createRecentDocPage();
		startUpDialog->setChecked(!prefsManager.appPrefs.uiPrefs.showStartupDialog);
	}
	else
	{
		tabWidget->removeTab(3);
		tabWidget->removeTab(2);
		tabWidget->removeTab(1);
	}


	tabWidget->setCurrentIndex(0);
	startUpDialog->setVisible(startUp);

	//tooltips
	listPageFormats->setToolTip( tr( "Document page size, either a standard size or a custom size" ) );
	buttonVertical->setToolTip( tr( "Vertical orientation of the document's pages" ) );
	buttonHorizontal->setToolTip( tr( "Horizontal orientation of the document's pages" ) );
	buttonSinglePage->setToolTip(tr("Single page document"));
	buttonDoublePageLeft->setToolTip(tr("Double page document, with the first page on the left side"));
	buttonDoublePageRight->setToolTip(tr("Double page document, with the first page on the right side"));
	buttonLTRBinding->setToolTip(tr("LTR binding direction"));
	buttonRTLBinding->setToolTip(tr("RTL Binding direction"));
	widthSpinBox->setToolTip( tr( "Width of the document's pages, editable if you have chosen a custom page size" ) );
	heightSpinBox->setToolTip( tr( "Height of the document's pages, editable if you have chosen a custom page size" ) );
	pageCountSpinBox->setToolTip( tr( "Initial number of pages of the document" ) );
	unitOfMeasureComboBox->setToolTip( tr( "Default unit of measurement for document editing" ) );
	autoTextFrame->setToolTip( tr( "Create text frames automatically when new pages are added" ) );
	numberOfCols->setToolTip( tr( "Number of columns to create in automatically created text frames" ) );
	Distance->setToolTip( tr( "Distance between automatically created columns" ) );

	// signals and slots connections
	connect(buttonBox, &QDialogButtonBox::accepted, this, &NewDocDialog::ExitOK);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &NewDocDialog::reject);

	connect(pageOrientationButtons, &QButtonGroup::idClicked, this, &NewDocDialog::setOrientation);
	connect(pageLayoutButtons, &QButtonGroup::idClicked, this, &NewDocDialog::setLayout);
	connect(unitOfMeasureComboBox, SIGNAL(activated(int)), this, SLOT(setUnit(int)));
	connect(Distance, SIGNAL(valueChanged(double)), this, SLOT(setDistance(double)));
	connect(autoTextFrame, SIGNAL(clicked()), this, SLOT(handleAutoFrame()));
	connect(listPageFormats, &PageSizeList::clicked, this, &NewDocDialog::changePageSize);
	connect(pageSizeSelector, &PageSizeSelector::pageCategoryChanged, this, &NewDocDialog::changeCategory);
	connect(marginGroup, &NewMarginWidget::marginChanged, this, &NewDocDialog::changeMargin);
	connect(bleedGroup, &NewMarginWidget::marginChanged, this, &NewDocDialog::changeBleed);
	connect(bleedGroup, &NewMarginWidget::valuesChanged, this, &NewDocDialog::changeBleed);
	connect(comboSortSizes, &QComboBox::currentIndexChanged, this, &NewDocDialog::changeSortMode);
	if (startUp)
	{
		connect(nftGui, SIGNAL(leaveOK()), this, SLOT(ExitOK()));
		connect(recentDocListBox, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(recentDocListBox_doubleClicked()));
		connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(adjustTitles(int)));
	}
}

void NewDocDialog::createNewDocPage()
{
	int orientation = prefsManager.appPrefs.docSetupPrefs.pageOrientation;
	int pagePositioning = prefsManager.appPrefs.docSetupPrefs.pagePositioning;
	int docBindingDirection = prefsManager.appPrefs.docSetupPrefs.docBindingDirection;
	QString pageSize = prefsManager.appPrefs.docSetupPrefs.pageSize;
	double pageHeight = prefsManager.appPrefs.docSetupPrefs.pageHeight;
	double pageWidth = prefsManager.appPrefs.docSetupPrefs.pageWidth;

	comboSortSizes->addItem( tr("Name Asc"), PageSizeList::NameAsc);
	comboSortSizes->addItem( tr("Name Desc"), PageSizeList::NameDesc);
	comboSortSizes->addItem( tr("Size Asc"), PageSizeList::DimensionAsc);
	comboSortSizes->addItem( tr("Size Desc"), PageSizeList::DimensionDesc);
	comboSortSizes->setCurrentIndex(0);

	pageOrientationButtons = new QButtonGroup();
	pageOrientationButtons->addButton(buttonVertical, 0);
	pageOrientationButtons->addButton(buttonHorizontal, 1);
	pageOrientationButtons->button(orientation)->setChecked(true);

	pageLayoutButtons = new QButtonGroup();
	pageLayoutButtons->setExclusive(false);
	pageLayoutButtons->addButton(buttonSinglePage, 0);
	pageLayoutButtons->addButton(buttonDoublePageLeft, 1);
	pageLayoutButtons->addButton(buttonDoublePageRight, 2);
	pageLayoutButtons->addButton(buttonLTRBinding, 3);
	pageLayoutButtons->addButton(buttonRTLBinding, 4);
	if (pagePositioning == singlePage)
	{
		pageLayoutButtons->button(0)->setChecked(true);
		pageLayoutButtons->button(1)->setChecked(true);
	}
	else if (prefsManager.appPrefs.pageSets[pagePositioning].FirstPage == 0)
	{
		pageLayoutButtons->button(1)->setChecked(true);
	}
	else
	{
		pageLayoutButtons->button(2)->setChecked(true);
	}

	if (docBindingDirection == 1)
		pageLayoutButtons->button(4)->setChecked(true);
	else
		pageLayoutButtons->button(3)->setChecked(true);

	listPageFormats->setValues(pageSize, orientation, PageSizeInfo::Preferred, PageSizeList::NameAsc);

	pageSizeSelector->setHasFormatSelector(false);
	pageSizeSelector->setHasCustom(false);
	pageSizeSelector->setPageSize(pageSize);
	pageSizeSelector->setCurrentCategory(PageSizeInfo::Preferred);

	widthSpinBox->setMinimum(pts2value(1.0, m_unitIndex));
	widthSpinBox->setMaximum(16777215);
	widthSpinBox->setNewUnit(m_unitIndex);
	widthSpinBox->setSuffix(m_unitSuffix);
	widthSpinBox->setValue(pageWidth * m_unitRatio);

	heightSpinBox->setMinimum(pts2value(1.0, m_unitIndex));
	heightSpinBox->setMaximum(16777215);
	heightSpinBox->setNewUnit(m_unitIndex);
	heightSpinBox->setSuffix(m_unitSuffix);
	heightSpinBox->setValue(pageHeight * m_unitRatio);

	unitOfMeasureComboBox->addItems(unitGetTextUnitList());
	unitOfMeasureComboBox->setCurrentIndex(m_unitIndex);
	unitOfMeasureComboBox->setEditable(false);

	MarginStruct marg(prefsManager.appPrefs.docSetupPrefs.margins);
	marginGroup->setup(marg, !(pagePositioning == singlePage), m_unitIndex, NewMarginWidget::MarginWidgetFlags);
	marginGroup->toggleLabelVisibility(false);
	marginGroup->setPageHeight(pageHeight);
	marginGroup->setPageWidth(pageWidth);
	marginGroup->setFacingPages(!(pagePositioning == singlePage));
	marginGroup->setPageSize(pageSize);
	marginGroup->setMarginPreset(prefsManager.appPrefs.docSetupPrefs.marginPreset);

	MarginStruct bleed;
	bleed.resetToZero();
	bleedGroup->setup(bleed, !(pagePositioning == singlePage), m_unitIndex, NewMarginWidget::BleedWidgetFlags);
	bleedGroup->toggleLabelVisibility(false);
	bleedGroup->setPageHeight(pageHeight);
	bleedGroup->setPageWidth(pageWidth);
	bleedGroup->setFacingPages(!(pagePositioning == singlePage));
	bleedGroup->setPageSize(pageSize);
	bleedGroup->setMarginPreset(prefsManager.appPrefs.docSetupPrefs.marginPreset);

	pageCountSpinBox->setMaximum( 10000 );
	pageCountSpinBox->setMinimum( 1 );

	IconManager &iconManager = IconManager::instance();
	pageCountLabel->setPixmap(iconManager.loadPixmap("panel-page"));

	setDocLayout(pagePositioning);
	setSize(pageSize);
	setOrientation(orientation);
	setDocBindingDirection(docBindingDirection);

	numberOfCols->setButtonSymbols( QSpinBox::UpDownArrows );
	numberOfCols->setMinimum( 1 );
	numberOfCols->setValue( 1 );

	Distance->setMinimum(0);
	Distance->setMaximum(1000);
	Distance->setNewUnit(m_unitIndex);
	Distance->setValue(11 * m_unitRatio);

	labelColumns->setEnabled(false);
	labelGap->setEnabled(false);
	Distance->setEnabled(false);
	numberOfCols->setEnabled(false);

	startDocSetup->setText( tr( "Show Document Settings After Creation" ) );
	startDocSetup->setChecked(false);

	sectionPreview->collapse();
	sectionPreview->setCanSaveState(true);
	sectionPreview->restorePreferences();

	sectionDocument->expand();
	sectionDocument->setCanSaveState(true);
	sectionDocument->restorePreferences();

	sectionMargins->expand();
	sectionMargins->setCanSaveState(true);
	sectionMargins->restorePreferences();

	sectionBleeds->collapse();
	sectionBleeds->setCanSaveState(true);
	sectionBleeds->restorePreferences();

	sectionTextFrame->collapse();
	sectionTextFrame->setCanSaveState(true);
	sectionTextFrame->restorePreferences();

	labelColumns->setLabelVisibility(m_labelVisibity);
	labelGap->setLabelVisibility(m_labelVisibity);
	pageCountLabel->setLabelVisibility(m_labelVisibity);
	orientationLabel->setLabelVisibility(m_labelVisibity);
	pageLayoutLabel->setLabelVisibility(m_labelVisibity);

	// We have to install an event filter to resize the scroll container width based on the content width.
	// The content width can change after we calculated the initial ui layout.
	scrollAreaWidgetContents->installEventFilter(this);
	scrollAreaWidgetContents->adjustSize();

}

void NewDocDialog::createOpenDocPage()
{
	PrefsContext* docContext = prefsManager.prefsFile->getContext("docdirs", false);
	QString docDir = ".";
	QString prefsDocDir = prefsManager.documentDir();
	if (!prefsDocDir.isEmpty())
		docDir = docContext->get("docsopen", prefsDocDir);
	else
		docDir = docContext->get("docsopen", ".");
	QString formats(FileLoader::getLoadFilterString());
//	formats.remove("PDF (*.pdf *.PDF);;");
	QVBoxLayout *openDocLayout = new QVBoxLayout(tab_3);
	openDocLayout->setContentsMargins(0, 0, 0, 0);
	openDocLayout->setSpacing(4);
	m_selectedFile = "";

	// With Qt 5.15 we have to be in careful so that new document dialog doesn't display too large on startup.
	// To avoid this we have to use QFileDialog(QWidget *parent, Qt::WindowFlags flags) constructor, then
	// set the QFileDialog::DontUseNativeDialog option as early as possible, and nonetheless set again
	// the Qt::Widget window flag before adding the widget to layout.
	fileDialog = new QFileDialog(tab_3, Qt::Widget);
	fileDialog->setOption(QFileDialog::DontUseNativeDialog);
	fileDialog->setWindowTitle(tr("Open"));
	fileDialog->setDirectory(docDir);
	fileDialog->setNameFilter(formats);
	fileDialog->setFileMode(QFileDialog::ExistingFile);
	fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog->setIconProvider(new ImIconProvider());
	fileDialog->setOption(QFileDialog::HideNameFilterDetails, true);
	fileDialog->setOption(QFileDialog::ReadOnly, true);
	fileDialog->setSizeGripEnabled(false);
	fileDialog->setModal(false);
	QList<QPushButton *> pushButtons = fileDialog->findChildren<QPushButton *>();
	for (auto pushButton : std::as_const(pushButtons))
		pushButton->setVisible(false);
	fileDialog->setWindowFlags(Qt::Widget);
	openDocLayout->addWidget(fileDialog);


	FileDialogEventCatcher* keyCatcher = new FileDialogEventCatcher(this);
	QList<QListView *> listViews = fileDialog->findChildren<QListView *>();
	for (auto listView : std::as_const(listViews))
		listView->installEventFilter(keyCatcher);
	connect(keyCatcher, SIGNAL(escapePressed()), this, SLOT(reject()));
	connect(keyCatcher, SIGNAL(dropLocation(QString)), this, SLOT(locationDropped(QString)));
	connect(keyCatcher, SIGNAL(desktopPressed()), this, SLOT(gotoDesktopDirectory()));
	connect(keyCatcher, SIGNAL(homePressed()), this, SLOT(gotoHomeDirectory()));
	connect(keyCatcher, SIGNAL(parentPressed()), this, SLOT(gotoParentDirectory()));
	connect(keyCatcher, SIGNAL(enterSelectedPressed()), this, SLOT(gotoSelectedDirectory()));
	connect(fileDialog, SIGNAL(currentChanged(QString)), this, SLOT(openFileDialogFileClicked(QString)));
	connect(fileDialog, SIGNAL(filesSelected(QStringList)), this, SLOT(openFile()));
	connect(fileDialog, SIGNAL(rejected()), this, SLOT(reject()));
}

void NewDocDialog::openFile()
{
	ExitOK();
}

void NewDocDialog::createRecentDocPage()
{
	int max = qMin(prefsManager.appPrefs.uiPrefs.recentDocCount, recentDocList.count());
	for (int i = 0; i < max; ++i)
		recentDocListBox->addItem(QDir::toNativeSeparators(recentDocList[i]));
	if (max>0)
		recentDocListBox->setCurrentRow(0);
}

void NewDocDialog::setWidth(double)
{
	m_pageWidth = widthSpinBox->value() / m_unitRatio;
	marginGroup->setPageWidth(m_pageWidth);
	bleedGroup->setPageWidth(m_pageWidth);
	listPageFormats->clearSelection();
	m_pageSize = CommonStrings::customPageSize;
	pagePreview->setPage(m_pageHeight, m_pageWidth, marginGroup->margins(), bleedGroup->margins(), m_pageSize, m_choosenLayout, m_layoutFirstPage);

	int newOrientation = (widthSpinBox->value() > heightSpinBox->value()) ? landscapePage : portraitPage;
	if (newOrientation != m_orientation)
	{
		m_orientation = newOrientation;

		QSignalBlocker sigOri(pageOrientationButtons);
		pageOrientationButtons->button(newOrientation)->setChecked(true);
		QSignalBlocker sigFormats(listPageFormats);
		listPageFormats->setOrientation(m_orientation);
	}

}

void NewDocDialog::setHeight(double)
{
	m_pageHeight = heightSpinBox->value() / m_unitRatio;
	marginGroup->setPageHeight(m_pageHeight);
	bleedGroup->setPageHeight(m_pageHeight);	
	listPageFormats->clearSelection();
	m_pageSize = CommonStrings::customPageSize;
	pagePreview->setPage(m_pageHeight, m_pageWidth, marginGroup->margins(), bleedGroup->margins(), m_pageSize, m_choosenLayout, m_layoutFirstPage);

	int newOrientation = (widthSpinBox->value() > heightSpinBox->value()) ? landscapePage : portraitPage;
	if (newOrientation != m_orientation)
	{
		m_orientation = newOrientation;

		QSignalBlocker sigOri(pageOrientationButtons);
		pageOrientationButtons->button(newOrientation)->setChecked(true);
		QSignalBlocker sigFormats(listPageFormats);
		listPageFormats->setOrientation(m_orientation);
	}
}

void NewDocDialog::changePageSize(const QModelIndex &ic)
{
	int unit = ic.data(PageSizeList::Unit).toInt();
	QString sizeName = ic.data(PageSizeList::Name).toString();

	setUnit(unit);
	setPageSize(sizeName);

	QSignalBlocker sig(unitOfMeasureComboBox);
	unitOfMeasureComboBox->setCurrentIndex(unit);

}

void NewDocDialog::changeSortMode(int ic)
{
	Q_UNUSED(ic);
	listPageFormats->setSortMode(static_cast<PageSizeList::SortMode>(comboSortSizes->currentData().toInt()));
}

bool NewDocDialog::eventFilter(QObject *object, QEvent *event)
{
	if (object->objectName() == "scrollAreaWidgetContents" && event->type() == QEvent::Resize)
	{
		int currentWidth = scrollArea->minimumWidth();
		scrollArea->setMinimumWidth(qMax(scrollAreaWidgetContents->sizeHint().width() + qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent), currentWidth));
		return true;
	}
	return false;
}

void NewDocDialog::handleAutoFrame()
{
	bool setter = autoTextFrame->isChecked();
	labelColumns->setEnabled(setter);
	labelGap->setEnabled(setter);
	Distance->setEnabled(setter);
	numberOfCols->setEnabled(setter);
}

void NewDocDialog::setDistance(double)
{
	m_distance = Distance->value() / m_unitRatio;
}

void NewDocDialog::setUnit(int newUnitIndex)
{
	disconnect(widthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setWidth(double)));
	disconnect(heightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setHeight(double)));
	widthSpinBox->setNewUnit(newUnitIndex);
	heightSpinBox->setNewUnit(newUnitIndex);
	Distance->setNewUnit(newUnitIndex);
	m_unitRatio = unitGetRatioFromIndex(newUnitIndex);
	m_unitIndex = newUnitIndex;
	widthSpinBox->setValue(m_pageWidth * m_unitRatio);
	heightSpinBox->setValue(m_pageHeight * m_unitRatio);

	marginGroup->setNewUnit(m_unitIndex);
	marginGroup->setPageHeight(m_pageHeight);
	marginGroup->setPageWidth(m_pageWidth);
	bleedGroup->setNewUnit(m_unitIndex);
	bleedGroup->setPageHeight(m_pageHeight);
	bleedGroup->setPageWidth(m_pageWidth);
	connect(widthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setWidth(double)));
	connect(heightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setHeight(double)));


}

void NewDocDialog::ExitOK()
{
	m_pageWidth = widthSpinBox->value() / m_unitRatio;
	m_pageHeight = heightSpinBox->value() / m_unitRatio;
	m_bleedBottom = bleedGroup->margins().bottom();
	m_bleedTop = bleedGroup->margins().top();
	m_bleedLeft = bleedGroup->margins().left();
	m_bleedRight = bleedGroup->margins().right();
	if (m_onStartup)
	{
		m_tabSelected = tabWidget->currentIndex();
		if (m_tabSelected == NewDocDialog::NewFromTemplateTab) // new doc from template
		{
			if (nftGui->currentDocumentTemplate)
			{
				m_selectedFile = QDir::fromNativeSeparators(nftGui->currentDocumentTemplate->file);
				m_selectedFile = QDir::cleanPath(m_selectedFile);
			}
		}
		else if (m_tabSelected == NewDocDialog::OpenExistingTab) // open existing doc
		{
			QStringList files = fileDialog->selectedFiles();
			if (files.count() != 0)
				m_selectedFile = QDir::fromNativeSeparators(files[0]);
			QFileInfo fi(m_selectedFile);
			if (fi.isDir())
			{
				fileDialog->setDirectory(fi.absoluteFilePath());
				return;
			}
		}
		else if (m_tabSelected == NewDocDialog::OpenRecentTab) // open recent doc
		{
			if (recentDocListBox->currentItem() != nullptr)
			{
				QString fileName(recentDocListBox->currentItem()->text());
				if (!fileName.isEmpty())
					m_selectedFile = QDir::fromNativeSeparators(fileName);
			}
		}
	}
	else
		m_tabSelected = NewDocDialog::NewDocumentTab;
	accept();
}

void NewDocDialog::setOrientation(int ori)
{
	disconnect(widthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setWidth(double)));
	disconnect(heightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setHeight(double)));
	if (ori != m_orientation)
	{
		double w  = widthSpinBox->value(), h = heightSpinBox->value();
		double pw = m_pageWidth, ph = m_pageHeight;
		widthSpinBox->setValue((ori == portraitPage) ? qMin(w, h) : qMax(w, h));
		heightSpinBox->setValue((ori == portraitPage) ? qMax(w, h) : qMin(w, h));
		m_pageWidth  = (ori == portraitPage) ? qMin(pw, ph) : qMax(pw, ph);
		m_pageHeight = (ori == portraitPage) ? qMax(pw, ph) : qMin(pw, ph);
		listPageFormats->setOrientation(ori);
	}
	// #869 pv - defined constants added + code repeat (check w/h)
	(ori == portraitPage) ? m_orientation = portraitPage : m_orientation = landscapePage;
	// end of #869
	marginGroup->setPageHeight(m_pageHeight);
	marginGroup->setPageWidth(m_pageWidth);
	bleedGroup->setPageHeight(m_pageHeight);
	bleedGroup->setPageWidth(m_pageWidth);
	pagePreview->setPage(m_pageHeight, m_pageWidth, marginGroup->margins(), bleedGroup->margins(), m_pageSize, m_choosenLayout, m_layoutFirstPage);

	connect(widthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setWidth(double)));
	connect(heightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setHeight(double)));
}

void NewDocDialog::setLayout(int layoutId)
{
	switch (layoutId)
	{
		case 0:
			pageLayoutButtons->button(2)->setChecked(false);
			pageLayoutButtons->button(3)->setDisabled(true);
			pageLayoutButtons->button(4)->setDisabled(true);
			setDocLayout(0);
			break;
		case 1:
			// pageLayoutButtons->button(2)->setChecked(false);
			// pageLayoutButtons->button(3)->setDisabled(true);
			// pageLayoutButtons->button(4)->setDisabled(true);
			setDocLayout(1);
			pagePreview->setFirstPage(0);
			setDocFirstPage(0);
			break;
		case 2:
			pageLayoutButtons->button(0)->setChecked(false);
			pageLayoutButtons->button(3)->setDisabled(false);
			pageLayoutButtons->button(4)->setDisabled(false);
			pageLayoutButtons->button(3)->setChecked(true);
			pageLayoutButtons->button(4)->setChecked(false);

			setDocLayout(1);
			pagePreview->setFirstPage(1);
			setDocFirstPage(1);
			break;
		case 3:
			pageLayoutButtons->button(0)->setChecked(false);
			pageLayoutButtons->button(2)->setChecked(true);
			pageLayoutButtons->button(4)->setChecked(false);
			setDocLayout(1);
			setDocBindingDirection(0);
			setDocFirstPage(1);
			break;
		case 4:
			pageLayoutButtons->button(0)->setChecked(false);
			pageLayoutButtons->button(2)->setChecked(true);
			pageLayoutButtons->button(3)->setChecked(false);

			setDocLayout(1);
			setDocBindingDirection(1);
			setDocFirstPage(0);
			break;
	}
}

void NewDocDialog::setPageSize(const QString &size)
{
	setSize(size);

	if (size != CommonStrings::customPageSize)
		setOrientation(pageOrientationButtons->checkedId());

	marginGroup->setPageSize(size);
	bleedGroup->setPageSize(size);

}

void NewDocDialog::setSize(const QString& gr)
{
	m_pageWidth = widthSpinBox->value() / m_unitRatio;
	m_pageHeight = heightSpinBox->value() / m_unitRatio;
	m_pageSize = gr;

	disconnect(widthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setWidth(double)));
	disconnect(heightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setHeight(double)));
	if (m_pageSize == CommonStrings::trCustomPageSize || m_pageSize == CommonStrings::customPageSize)
	{
		widthSpinBox->setEnabled(true);
		heightSpinBox->setEnabled(true);
	}
	else
	{
		PageSize ps2(m_pageSize);
		if (pageOrientationButtons->checkedId() == portraitPage)
		{
			m_pageWidth = ps2.width();
			m_pageHeight = ps2.height();
		}
		else
		{
			m_pageWidth = ps2.height();
			m_pageHeight = ps2.width();
		}
	}
	widthSpinBox->setValue(m_pageWidth * m_unitRatio);
	heightSpinBox->setValue(m_pageHeight * m_unitRatio);
	marginGroup->setPageHeight(m_pageHeight);
	marginGroup->setPageWidth(m_pageWidth);
	bleedGroup->setPageHeight(m_pageHeight);
	bleedGroup->setPageWidth(m_pageWidth);
	pagePreview->setPage(m_pageHeight, m_pageWidth, marginGroup->margins(), bleedGroup->margins(), m_pageSize, m_choosenLayout, m_layoutFirstPage);

	connect(widthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setWidth(double)));
	connect(heightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setHeight(double)));

}

void NewDocDialog::setDocLayout(int layout)
{
	marginGroup->setFacingPages(layout != singlePage);
	bleedGroup->setFacingPages(layout != singlePage);
	m_choosenLayout = layout;
	m_layoutFirstPage = prefsManager.appPrefs.pageSets[m_choosenLayout].FirstPage;
	pagePreview->setPage(m_pageHeight, m_pageWidth, marginGroup->margins(), bleedGroup->margins(), m_pageSize, m_choosenLayout, m_layoutFirstPage);
}

void NewDocDialog::setDocFirstPage(int firstPage)
{
	m_layoutFirstPage = firstPage;
}

void NewDocDialog::setDocBindingDirection(int bindingDirection)
{
	m_bindingDirection = bindingDirection;
}

void NewDocDialog::recentDocListBox_doubleClicked()
{
	/* Yep. There is nothing to solve. ScribusMainWindow handles all
	openings etc. It's Franz's programming style ;) */
	ExitOK();
}

void NewDocDialog::adjustTitles(int tab)
{
	if (tab == 0)
		setWindowTitle(tr("New Document"));
	else if (tab == 1)
		setWindowTitle(tr("New from Template"));
	else if (tab == 2)
		setWindowTitle(tr("Open Existing Document"));
	else if (tab == 3)
		setWindowTitle(tr("Open Recent Document"));
	else
		setWindowTitle(tr("New Document"));
	//okButton->setEnabled(tab!=2);
}

void NewDocDialog::locationDropped(const QString& fileUrl)
{
	QFileInfo fi(fileUrl);
	if (fi.isDir())
		fileDialog->setDirectory(fi.absoluteFilePath());
	else
	{
		fileDialog->setDirectory(fi.absolutePath());
		fileDialog->selectFile(fi.fileName());
	}
}

void NewDocDialog::gotoParentDirectory()
{
	QDir d(fileDialog->directory());
	d.cdUp();
	fileDialog->setDirectory(d);
}


void NewDocDialog::gotoSelectedDirectory()
{
	QStringList s(fileDialog->selectedFiles());
	if (s.isEmpty())
		return;
	QFileInfo fi(s.first());
	if (fi.isDir())
		fileDialog->setDirectory(fi.absoluteFilePath());
}

void NewDocDialog::gotoDesktopDirectory()
{
	QString dp = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	QFileInfo fi(dp);
	if (fi.exists())
		fileDialog->setDirectory(dp);
}


void NewDocDialog::gotoHomeDirectory()
{
	QString dp = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	QFileInfo fi(dp);
	if (fi.exists())
		fileDialog->setDirectory(dp);
}

void NewDocDialog::openFileDialogFileClicked(const QString& path)
{
	//okButton->setEnabled(!path.isEmpty());
}

void NewDocDialog::changeMargin(MarginStruct margin)
{
	pagePreview->setMargins(margin);
}

void NewDocDialog::changeBleed(MarginStruct bleed)
{
	pagePreview->setBleeds(bleed);
}

void NewDocDialog::changeCategory(PageSizeInfo::Category category)
{
	if (listPageFormats->category() == category)
		return;

	listPageFormats->setFormat(m_pageSize);
	listPageFormats->setCategory(category);
}
