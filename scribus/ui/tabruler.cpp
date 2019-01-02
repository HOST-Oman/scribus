/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include "tabruler.h"

#include <QApplication>
#include <QColor>
#include <QComboBox>
#include <QCursor>
#include <QEvent>
#include <QLabel>
#include <QLayout>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QPolygon>
#include <QPushButton>
#include <QToolButton>
#include <QToolTip>
#include <QVariant>

#include "commonstrings.h"
#include "units.h"
#include "scribusstructs.h"
#include "scrspinbox.h"
#include "iconmanager.h"
#include "util.h"

RulerT::RulerT(QWidget *pa, int ein, const QList<ParagraphStyle::TabRecord>& Tabs, bool ind, double wid) : QWidget(pa),
	mousePressed(false),
	tabValues(Tabs),
	haveInd(ind),
	unitIndex(ein),
	offset(0),
	actTab(-1),
	leftIndent(0),
	firstLine(0),
	Width(0.0),
	rulerCode(0),
	mouseX(0),
	offsetIncrement(5)
{
	QPalette palette;
	palette.setColor(backgroundRole(), QColor(255,255,255));
	setPalette(palette);
	iter=unitRulerGetIter1FromIndex(unitIndex);
	iter2=unitRulerGetIter2FromIndex(unitIndex);

	setMouseTracking(true);
	if (wid < 0)
	{
		setMinimumSize(QSize(400,25));
		setMaximumSize(QSize(4000,25));
		Width = 4000;
		resize(400, 25);
	}
	else
	{
		Width = wid;
		setMinimumSize(QSize(qMin(static_cast<int>(Width), 400),25));
		setMaximumSize(QSize(4000,25));
		resize(qMin(static_cast<int>(wid), 400), 25);
	}
}

void RulerT::setTabs(const QList<ParagraphStyle::TabRecord>& Tabs, int dEin)
{
	unitIndex = dEin;
	iter  = unitRulerGetIter1FromIndex(unitIndex);
	iter2 = unitRulerGetIter2FromIndex(unitIndex);
	tabValues = Tabs;
	actTab    = -1;
	repaint();
}

void RulerT::paintEvent(QPaintEvent *)
{
	double xl;
	QPainter p;
	p.begin(this);
	p.drawLine(0, 24, width(), 24);
	p.translate(-offset, 0);
	p.setBrush(Qt::black);
	p.setFont(font());
	p.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
	for (xl = 0; xl < width()+offset; xl += iter)
	{
		if (xl < offset)
			continue;
		p.drawLine(qRound(xl), 18, qRound(xl), 24);
	}
	for (xl = 0; xl < width()+(iter2/2)+offset; xl += iter2)
	{
		if (xl < offset)
			continue;
		p.drawLine(qRound(xl), 11, qRound(xl), 24);
		switch (unitIndex)
		{
			case 2:
			{
				QString tx;
				int num1 = static_cast<int>(xl / iter2);
				if (num1 != 0)
					tx = QString::number(num1);
				double frac = (xl / iter2) - num1;
				if ((frac > 0.24) && (frac < 0.26))
					tx += QChar(0xBC);
				if ((frac > 0.49) && (frac < 0.51))
					tx += QChar(0xBD);
				if ((frac > 0.74) && (frac < 0.76))
					tx += QChar(0xBE);
				p.drawText(qRound(xl+2), 17, tx);
				break;
			}
			case 3:
				p.drawText(qRound(xl+2), 17, QString::number(xl / iter));
				break;
			default:
				p.drawText(qRound(xl+2), 17, QString::number(xl / iter * 10));
				break;
		}
	}
	if (tabValues.count() != 0)
	{
		for (int yg = 0; yg < static_cast<int>(tabValues.count()); yg++)
		{
			if (yg == actTab)
				p.setPen(QPen(Qt::red, 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
			else
				p.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
			switch (static_cast<int>(tabValues[yg].tabType))
			{
				case 0:
					p.drawLine(qRound(tabValues[yg].tabPosition), 15, qRound(tabValues[yg].tabPosition), 23);
					p.drawLine(qRound(tabValues[yg].tabPosition), 23, qRound(tabValues[yg].tabPosition+8), 23);
					break;
				case 1:
					p.drawLine(qRound(tabValues[yg].tabPosition), 15, qRound(tabValues[yg].tabPosition), 23);
					p.drawLine(qRound(tabValues[yg].tabPosition), 23, qRound(tabValues[yg].tabPosition-8), 23);
					break;
				case 2:
				case 3:
					p.drawLine(qRound(tabValues[yg].tabPosition), 15, qRound(tabValues[yg].tabPosition), 23);
					p.drawLine(qRound(tabValues[yg].tabPosition-4), 23, qRound(tabValues[yg].tabPosition+4), 23);
					p.drawLine(qRound(tabValues[yg].tabPosition+3), 20, qRound(tabValues[yg].tabPosition+2), 20);
					break;
				case 4:
					p.drawLine(qRound(tabValues[yg].tabPosition), 15, qRound(tabValues[yg].tabPosition), 23);
					p.drawLine(qRound(tabValues[yg].tabPosition-4), 23, qRound(tabValues[yg].tabPosition+4), 23);
					break;
				default:
					break;
			}
		}
	}
	if (haveInd)
	{
		p.setPen(QPen(Qt::blue, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
		p.setBrush(Qt::blue);
		QPolygon cr;
		cr.setPoints(3, qRound(firstLine+leftIndent), 12, qRound(firstLine+leftIndent-4), 0, qRound(firstLine+leftIndent+4), 0);
		p.drawPolygon(cr);
		QPolygon cr2;
		cr2.setPoints(3, qRound(leftIndent), 12, qRound(leftIndent+4), 24, qRound(leftIndent-4), 24);
		p.drawPolygon(cr2);
	}
	p.end();
}

void RulerT::mousePressEvent(QMouseEvent *m)
{
	QRect fpo;
	mousePressed = true;
	qApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
	rulerCode = 0;
	if (haveInd)
	{
		fpo = QRect(static_cast<int>(firstLine+leftIndent-offset)-4, 0, 8, 12);
		if (fpo.contains(m->pos()))
		{
			rulerCode = 1;
			mouseX = m->x();
			return;
		}
		fpo = QRect(static_cast<int>(leftIndent-offset)-4, 12, 8, 12);
		if (fpo.contains(m->pos()))
		{
			rulerCode = 2;
			mouseX = m->x();
			return;
		}
	}
	if (tabValues.count() != 0)
	{
		for (int yg = 0; yg < static_cast<int>(tabValues.count()); yg++)
		{
			fpo = QRect(static_cast<int>(tabValues[yg].tabPosition-offset)-3, 15, 8, 8);
			if (fpo.contains(m->pos()))
			{
				rulerCode = 3;
				actTab = yg;
				mouseX = m->x();
				emit tabSelected();
				emit typeChanged(tabValues[actTab].tabType);
				emit tabMoved(tabValues[actTab].tabPosition);
				emit fillCharChanged(tabValues[actTab].tabFillChar);
				repaint();
				return;
			}
		}
	}
	if ((rulerCode == 0) && (m->button() == Qt::LeftButton))
	{
		ParagraphStyle::TabRecord tb;
		tb.tabPosition = static_cast<double>(m->x() + offset);
		tb.tabType = 0;
		tb.tabFillChar = QChar();
		tabValues.prepend(tb);
		actTab = 0;
		rulerCode = 3;
		updateTabList();
		repaint();
		emit newTab();
		emit typeChanged(tabValues[actTab].tabType);
		emit tabMoved(tabValues[actTab].tabPosition);
		emit fillCharChanged(tabValues[actTab].tabFillChar);
		qApp->changeOverrideCursor(QCursor(Qt::SizeHorCursor));
	}
	mouseX = m->x();
}

void RulerT::mouseReleaseEvent(QMouseEvent *m)
{
	mousePressed = false;
	qApp->restoreOverrideCursor();
	if ((m->y() < height()) && (m->y() > 0))
	{
		if (rulerCode == 3)
		{
			if (m->button() == Qt::RightButton)
			{
				tabValues[actTab].tabType += 1;
				if (tabValues[actTab].tabType > 4)
					tabValues[actTab].tabType = 0;
				emit typeChanged(tabValues[actTab].tabType);
				repaint();
			}
		}
	}
	else
	{
		if (rulerCode == 3)
		{
			tabValues.removeAt(actTab);
			actTab = 0;
			if (tabValues.count() != 0)
			{
				emit typeChanged(tabValues[actTab].tabType);
				emit tabMoved(tabValues[actTab].tabPosition);
				emit fillCharChanged(tabValues[actTab].tabFillChar);
			}
			else
				emit noTabs();
			repaint();
		}
	}
	rulerCode = 0;
	emit mouseReleased();
}

void RulerT::mouseMoveEvent(QMouseEvent *m)
{
	double oldInd;
	QRect fpo;
	if ((mousePressed) && (m->y() < height()) && (m->y() > 0) && (m->x() > 0) && (m->x() < width()))
	{
		qApp->changeOverrideCursor(QCursor(Qt::SizeHorCursor));
		switch (rulerCode)
		{
			case 1:
				firstLine -= mouseX - m->x();
				if (firstLine+leftIndent+offset < offset)
					firstLine += mouseX - m->x();
				if (firstLine+leftIndent > Width)
					firstLine  = Width-leftIndent;
				emit firstLineMoved(firstLine);
				repaint();
				break;
			case 2:
				oldInd = leftIndent+firstLine;
				leftIndent -= mouseX - m->x();
				if (leftIndent < 0)
					leftIndent = 0;
				if (leftIndent > Width-1)
					leftIndent  = Width-1;
				firstLine = oldInd - leftIndent;
				emit leftIndentMoved(leftIndent);
				emit firstLineMoved(firstLine);
				repaint();
				break;
			case 3:
				tabValues[actTab].tabPosition -= mouseX - m->x();
				if (tabValues[actTab].tabPosition < 0)
					tabValues[actTab].tabPosition = 0;
				if (tabValues[actTab].tabPosition > Width-1)
					tabValues[actTab].tabPosition = Width-1;
				updateTabList();
				emit tabMoved(tabValues[actTab].tabPosition);
				repaint();
				break;
			default:
				break;
		}
		mouseX = m->x();
		return;
	}
	if ((!mousePressed) && (m->y() < height()) && (m->y() > 0) && (m->x() > 0) && (m->x() < width()))
	{
		setCursor(IconManager::instance()->loadCursor("tab.png", 3));
		if (haveInd)
		{
			fpo = QRect(static_cast<int>(firstLine+leftIndent-offset)-4, 0, 8, 12);
			if (fpo.contains(m->pos()))
			{
				setCursor(QCursor(Qt::SizeHorCursor));
				return;
			}
			fpo = QRect(static_cast<int>(leftIndent-offset)-4, 12, 8, 12);
			if (fpo.contains(m->pos()))
			{
				setCursor(QCursor(Qt::SizeHorCursor));
				return;
			}
		}
		if (tabValues.count() != 0)
		{
			for (int yg = 0; yg < static_cast<int>(tabValues.count()); yg++)
			{
				fpo = QRect(static_cast<int>(tabValues[yg].tabPosition-offset)-3, 15, 8, 8);
				if (fpo.contains(m->pos()))
				{
					setCursor(QCursor(Qt::SizeHorCursor));
					return;
				}
			}
		}
	}
	if ((mousePressed) && ((m->y() > height()) || (m->y() < 0) || (m->x() < 0) || (m->x() > width())))
	{
		qApp->changeOverrideCursor(IconManager::instance()->loadCursor("DelPoint.png", 1, 1));
	}
}

void RulerT::leaveEvent(QEvent*)
{
	if (mousePressed)
	{
		if (rulerCode == 3)
			qApp->changeOverrideCursor(IconManager::instance()->loadCursor("DelPoint.png", 1, 1));
		else
			qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
	}
}

void RulerT::updateTabList()
{
	ParagraphStyle::TabRecord tb;
	tb.tabPosition = tabValues[actTab].tabPosition;
	tb.tabType = tabValues[actTab].tabType;
	tb.tabFillChar =  tabValues[actTab].tabFillChar;
	int gg = static_cast<int>(tabValues.count()-1);
	int g = gg;
	tabValues.removeAt(actTab);
	for (int yg = static_cast<int>(tabValues.count()-1); yg > -1; yg--)
	{
		if (tb.tabPosition < tabValues[yg].tabPosition)
			g = yg;
	}
	actTab = g;
	if (gg == g)
	{
		tabValues.append(tb);
		actTab = static_cast<int>(tabValues.count()-1);
	}
	else
	{
		tabValues.insert(actTab, tb);
	}
}

void RulerT::resetOffsetInc()
{
	offsetIncrement = 5;
}

void RulerT::increaseOffset()
{
	offset += offsetIncrement;
	offsetIncrement++;
	if (offsetIncrement > 30)
		offsetIncrement = 30;
	if (offset + width() > static_cast<int>(Width))
		offset -= 5;
	repaint();
}

void RulerT::decreaseOffset()
{
	offset -= offsetIncrement;
	offsetIncrement++;
	if (offsetIncrement > 30)
		offsetIncrement = 30;
	if (offset < 0)
		offset = 0;
	repaint();
}

void RulerT::changeTab(int t)
{
	if (actTab < 0 || actTab >= tabValues.count())
		return;
	tabValues[actTab].tabType = t;
	repaint();
}

void RulerT::changeTabChar(QChar t)
{
	if (actTab < 0 || actTab >= tabValues.count())
		return;
	tabValues[actTab].tabFillChar = t;
	repaint();
}

void RulerT::moveTab(double t)
{
	if (actTab < 0 || actTab >= tabValues.count())
		return;
	tabValues[actTab].tabPosition = t;
	updateTabList();
	repaint();
}

void RulerT::removeActTab()
{
	if (actTab > -1)
	{
		tabValues.removeAt(actTab);
		actTab = 0;
		if (tabValues.count() != 0)
		{
			emit typeChanged(tabValues[actTab].tabType);
			emit tabMoved(tabValues[actTab].tabPosition);
			emit fillCharChanged(tabValues[actTab].tabFillChar);
		}
		else
			emit noTabs();
	}
	repaint();
}

void RulerT::moveFirstLine(double t)
{
	firstLine = t;
	if (firstLine+leftIndent+offset < offset)
	{
		firstLine = 0-leftIndent;
		emit firstLineMoved(firstLine);
	}
	if (firstLine+leftIndent > Width)
	{
		firstLine = Width-leftIndent;
		emit firstLineMoved(firstLine);
	}
	repaint();
}

void RulerT::moveLeftIndent(double t)
{
	double oldInd = leftIndent+firstLine;
	leftIndent = t;
	if (leftIndent > Width-1)
	{
		leftIndent  = Width-1;
		emit leftIndentMoved(leftIndent);
	}
	firstLine = oldInd - leftIndent;
	emit firstLineMoved(firstLine);
	repaint();
}

Tabruler::Tabruler( QWidget* parent, bool haveFirst, int dEin, const QList<ParagraphStyle::TabRecord>& Tabs, double wid ) : QWidget( parent ),
	firstLineLabel(nullptr),
	leftIndentLabel(nullptr),
	rightIndentLabel(nullptr),
	firstLineData(nullptr),
	leftIndentData(nullptr),
	rightIndentData(nullptr)
{
	docUnitRatio=unitGetRatioFromIndex(dEin);
	double ww = (wid < 0) ? 4000 : wid;
	tabrulerLayout = new QVBoxLayout( this );
	tabrulerLayout->setMargin(0);
	tabrulerLayout->setSpacing(5);
	layout2 = new QHBoxLayout;
	layout2->setMargin(0);
	layout2->setSpacing(5);

	rulerScrollL = new QToolButton(this);
	rulerScrollL->setArrowType(Qt::LeftArrow);
	rulerScrollL->setAutoRepeat( true );
	layout2->addWidget( rulerScrollL );
	ruler = new RulerT( this, dEin, Tabs, haveFirst, wid );
	ruler->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	layout2->addWidget( ruler );
	rulerScrollR = new QToolButton(this);
	rulerScrollR->setArrowType(Qt::RightArrow);
	rulerScrollR->setAutoRepeat( true );
	layout2->addWidget( rulerScrollR );

	layout1 = new QHBoxLayout;
	layout1->setMargin(0);
	layout1->setSpacing(5);
	layout1->setAlignment( Qt::AlignTop );
	typeCombo = new QComboBox(this);
	typeCombo->setEditable(false);
	typeCombo->clear();
	typeCombo->addItem( tr( "Left" ) );
	typeCombo->addItem( tr( "Right" ) );
	typeCombo->addItem( tr( "Period" ) );
	typeCombo->addItem( tr( "Comma" ) );
	typeCombo->addItem( tr( "Center" ) );
	layout1->addWidget( typeCombo );
	tabData = new ScrSpinBox( 0, ww / docUnitRatio, this, dEin );
	tabData->setValue(0);
	positionLabel = new QLabel( tr("&Position:"), this );
	positionLabel->setBuddy(tabData);
	layout1->addWidget( positionLabel );
	layout1->addWidget( tabData );
	tabFillCombo = new QComboBox(this);
	tabFillCombo->setEditable(false);
	tabFillCombo->addItem( tr("None", "tab fill"));
	tabFillCombo->addItem( tr("Dot"));
	tabFillCombo->addItem( tr("Hyphen"));
	tabFillCombo->addItem( tr("Underscore"));
	tabFillCombo->addItem( tr("Custom"));
	tabFillComboT = new QLabel( tr( "Fill Char:" ), this );
	tabFillComboT->setBuddy(tabFillCombo);
	layout1->addWidget( tabFillComboT );
	layout1->addWidget( tabFillCombo );

	layout4 = new QHBoxLayout;
	layout4->setMargin(0);
	layout4->setSpacing(5);

	indentLayout = new QHBoxLayout;
	indentLayout->setMargin(0);
	indentLayout->setSpacing(5);
	if (haveFirst)
	{
		firstLineData = new ScrSpinBox( -3000, ww / docUnitRatio, this, dEin);
		firstLineData->setValue(0);
		firstLineLabel = new QLabel(this);
		firstLineLabel->setText("");
		firstLineLabel->setPixmap(IconManager::instance()->loadPixmap("firstline.png"));
		indentLayout->addWidget( firstLineLabel );
		indentLayout->addWidget( firstLineData );
		leftIndentData = new ScrSpinBox( 0, ww / docUnitRatio, this, dEin );
		leftIndentData->setValue(0);
		leftIndentLabel = new QLabel(this);
		leftIndentLabel->setText("");
		leftIndentLabel->setPixmap(IconManager::instance()->loadPixmap("leftindent.png"));
		layout4->addWidget( leftIndentLabel );
		layout4->addWidget( leftIndentData );
		layout4->addStretch(10);
		rightIndentLabel = new QLabel(this);
		rightIndentLabel->setText("");
		rightIndentLabel->setPixmap(IconManager::instance()->loadPixmap("rightindent.png"));
		rightIndentData = new ScrSpinBox(0, ww / docUnitRatio, this, dEin);
		rightIndentData->setValue(0);
		indentLayout->addWidget(rightIndentLabel);
		indentLayout->addWidget(rightIndentData);
	}
	clearButton = new QPushButton( this );
	clearButton->setText( tr( "Delete All" ) );
	indentLayout->addSpacing(20);
	indentLayout->addWidget( clearButton);
	clearOneButton = new QPushButton( this );
	clearOneButton->setText( tr( "Delete Selected" ) );
	indentLayout->addWidget( clearOneButton);
	indentLayout->addStretch(10);
	if (!haveFirst)
	{
		QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		layout1->addItem( spacer );
	}
	
	layout1->addStretch( 10 );
	tabrulerLayout->addLayout( layout1 );
	tabrulerLayout->addLayout( layout2 );
	indentLayout->addStretch( 10 );
	tabrulerLayout->addLayout( indentLayout );
	tabrulerLayout->addLayout(layout4);

	tabData->setEnabled(false);
	tabFillCombo->setEnabled(false);
	typeCombo->setEnabled(false);
	if (Tabs.count() == 0)
		clearButton->setEnabled(false);
	clearOneButton->setEnabled(false);
	resize( minimumSizeHint() );
	connect(rulerScrollL, SIGNAL(clicked()), ruler, SLOT(decreaseOffset()));
	connect(rulerScrollR, SIGNAL(clicked()), ruler, SLOT(increaseOffset()));
	connect(rulerScrollL, SIGNAL(released()), this, SLOT(resetOFfL()));
	connect(rulerScrollR, SIGNAL(released()), this, SLOT(resetOFfR()));
	connect(ruler, SIGNAL(typeChanged(int)) , this, SLOT(setTabType(int)));
	connect(ruler, SIGNAL(fillCharChanged(QChar)) , this, SLOT(setTabFillChar(QChar)));
	connect(typeCombo, SIGNAL(activated(int)), this, SLOT(setType()));
	connect(tabFillCombo, SIGNAL(activated(int)), this, SLOT(setFillChar()));
	connect(tabFillCombo, SIGNAL(editTextChanged(const QString &)), this, SLOT(setCustomFillChar(const QString &)));
	connect(ruler, SIGNAL(tabMoved(double)) , this, SLOT(setTabData(double)));
	connect(ruler, SIGNAL(tabSelected()), this, SLOT(tabAdded()));
	connect(ruler, SIGNAL(newTab()), this, SLOT(tabAdded()));
	connect(ruler, SIGNAL(noTabs()), this, SLOT(lastTabRemoved()));
	connect(tabData, SIGNAL(valueChanged(double)), this, SLOT(setTab()));
	connect(clearButton, SIGNAL(clicked()), this, SLOT(clearAll()));
	connect(clearOneButton, SIGNAL(clicked()), this, SLOT(clearOne()));

	tabFillCombo->setToolTip( tr( "Fill Character of Tab" ) );
	typeCombo->setToolTip( tr( "Type/Orientation of Tab" ) );
	tabData->setToolTip( tr( "Position of Tab" ) );

	if (haveFirst)
	{
		connect(ruler, SIGNAL(firstLineMoved(double)) , this, SLOT(setFirstLineData(double)));
		connect(ruler, SIGNAL(leftIndentMoved(double)) , this, SLOT(setLeftIndentData(double)));
		connect(ruler, SIGNAL(mouseReleased()), this, SIGNAL(tabrulerChanged()));
		connect(ruler, SIGNAL(mouseReleased()), this, SLOT(slotMouseReleased()));
		connect(firstLineData, SIGNAL(valueChanged(double)), this, SLOT(setFirstLine()));
		connect(leftIndentData, SIGNAL(valueChanged(double)), this, SLOT(setLeftIndent()));
		connect(rightIndentData, SIGNAL(valueChanged(double)), this, SLOT(setRightIndent()));
		firstLineData->setToolTip( tr( "Indentation for first line of the paragraph" ) );
		leftIndentData->setToolTip( tr( "Indentation from the left for the whole paragraph" ) );
		rightIndentData->setToolTip( tr( "Indentation from the right for the whole paragraph" ) );
		firstLineLabel->setToolTip(firstLineData->toolTip());
		leftIndentLabel->setToolTip(leftIndentData->toolTip());
		rightIndentLabel->setToolTip(rightIndentData->toolTip());
	}
	clearButton->setToolTip( tr( "Delete all Tabulators" ) );
	clearOneButton->setToolTip( tr("Delete selected Tabulator"));
	QString ein = unitGetSuffixFromIndex(dEin);
	if (dEin == 2)
	{
		if (haveFirst)
		{
			firstLineData->setDecimals(4);
			leftIndentData->setDecimals(4);
			rightIndentData->setDecimals(4);
		}
		tabData->setDecimals(4);
	}
	if (haveFirst)
	{
		firstLineData->setSuffix(ein);
		leftIndentData->setSuffix(ein);
		rightIndentData->setSuffix(ein);
	}
	tabData->setSuffix(ein);
	haveF = haveFirst;
}

void Tabruler::changeEvent(QEvent *e)
{
	if (e->type() == QEvent::LanguageChange)
	{
		languageChange();
		return;
	}
	QWidget::changeEvent(e);
}

void Tabruler::languageChange()
{
	int  oldTypeComboIndex = typeCombo->currentIndex();
	bool typeComboBlocked = typeCombo->blockSignals(true);
	typeCombo->clear();
	typeCombo->addItem( tr( "Left" ) );
	typeCombo->addItem( tr( "Right" ) );
	typeCombo->addItem( tr( "Period" ) );
	typeCombo->addItem( tr( "Comma" ) );
	typeCombo->addItem( tr( "Center" ) );
	typeCombo->setCurrentIndex(oldTypeComboIndex);
	typeCombo->blockSignals(typeComboBlocked);

	positionLabel->setText( tr("&Position:"));

	int  oldTabFillComboIndex = tabFillCombo->currentIndex();
	bool tabFillComboBlocked = tabFillCombo->blockSignals(true);
	tabFillCombo->clear();
	tabFillCombo->addItem( tr("None", "tab fill"));
	tabFillCombo->addItem( tr("Dot"));
	tabFillCombo->addItem( tr("Hyphen"));
	tabFillCombo->addItem( tr("Underscore"));
	tabFillCombo->addItem( tr("Custom"));
	tabFillCombo->setCurrentIndex(oldTabFillComboIndex);
	tabFillCombo->blockSignals(tabFillComboBlocked);

	tabFillComboT->setText( tr( "Fill Char:" ));

	clearButton->setText( tr( "Delete All" ) );
	clearOneButton->setText( tr( "Delete Selected" ) );

	tabFillCombo->setToolTip( tr( "Fill Character of Tab" ) );
	typeCombo->setToolTip( tr( "Type/Orientation of Tab" ) );
	tabData->setToolTip( tr( "Position of Tab" ) );

	if (haveF)
	{
		firstLineData->setToolTip( tr( "Indentation for first line of the paragraph" ) );
		leftIndentData->setToolTip( tr( "Indentation from the left for the whole paragraph" ) );
		rightIndentData->setToolTip( tr( "Indentation from the right for the whole paragraph" ) );
		firstLineLabel->setToolTip(firstLineData->toolTip());
		leftIndentLabel->setToolTip(leftIndentData->toolTip());
		rightIndentLabel->setToolTip(rightIndentData->toolTip());
	}
	clearButton->setToolTip( tr( "Delete all Tabulators" ) );
	clearOneButton->setToolTip( tr("Delete selected Tabulator"));

	QString unitSuffix = unitGetSuffixFromIndex(tabData->unitIndex());
	if (haveF)
	{
		firstLineData->setSuffix(unitSuffix);
		leftIndentData->setSuffix(unitSuffix);
		rightIndentData->setSuffix(unitSuffix);
	}
	tabData->setSuffix(unitSuffix);
}

void Tabruler::setTabs(const QList<ParagraphStyle::TabRecord>& Tabs, int dEin)
{
	docUnitRatio=unitGetRatioFromIndex(dEin);
	tabData->setNewUnit(dEin);
	if (haveF)
	{
		firstLineData->setNewUnit(dEin);
		leftIndentData->setNewUnit(dEin);
		rightIndentData->setNewUnit(dEin);
	}
	ruler->setTabs(Tabs, dEin);
	if (Tabs.count() == 0)
		clearButton->setEnabled(false);
	clearOneButton->setEnabled(false);
	tabData->setEnabled(false);
	tabFillCombo->setEnabled(false);
	typeCombo->setEnabled(false);
}

void Tabruler::resetOFfL()
{
	if (!rulerScrollL->isDown())
		ruler->resetOffsetInc();
	tabData->setEnabled(false);
	tabFillCombo->setEnabled(false);
	typeCombo->setEnabled(false);
}

void Tabruler::resetOFfR()
{
	if (!rulerScrollR->isDown())
		ruler->resetOffsetInc();
}

void Tabruler::clearAll()
{
	ruler->tabValues.clear();
	ruler->repaint();
	lastTabRemoved();
	emit tabrulerChanged();
	emit tabsChanged();
}

void Tabruler::clearOne()
{
	ruler->removeActTab();
	emit tabrulerChanged();
	emit tabsChanged();
}

void Tabruler::tabAdded()
{
	typeCombo->setEnabled(true);
	tabData->setEnabled(true);
	clearButton->setEnabled(true);
	clearOneButton->setEnabled(true);
	tabFillCombo->setEnabled(true);
	emit tabrulerChanged();
	emit tabsChanged();
}

void Tabruler::lastTabRemoved()
{
	typeCombo->setEnabled(false);
	tabData->setEnabled(false);
	clearButton->setEnabled(false);
	clearOneButton->setEnabled(false);
	tabFillCombo->setEnabled(false);
	emit tabrulerChanged();
	emit tabsChanged();
}

void Tabruler::setFillChar()
{
	tabFillCombo->blockSignals(true);
	QChar ret;
	switch (tabFillCombo->currentIndex())
	{
		case 0:
			tabFillCombo->setEditable(false);
			ret = QChar();
			break;
		case 1:
			tabFillCombo->setEditable(false);
			ret = '.';
			break;
		case 2:
			tabFillCombo->setEditable(false);
			ret = '-';
			break;
		case 3:
			tabFillCombo->setEditable(false);
			ret = '_';
			break;
		case 4:
			tabFillCombo->setEditable(true);
			setCurrentComboItem(tabFillCombo, CommonStrings::trCustomTabFill);
			break;
	}
	if (tabFillCombo->currentIndex() != 4)
		ruler->changeTabChar(ret);
	tabFillCombo->blockSignals(false);
	emit tabrulerChanged();
	emit tabsChanged();
}

void Tabruler::setCustomFillChar(const QString &txt)
{
	if (txt == CommonStrings::trCustomTabFill)
		return;
	tabFillCombo->blockSignals(true);
	QChar ret = (txt.length() > 0) ? txt[txt.length()-1] : QChar::Null;
	ruler->changeTabChar(ret);
	tabFillCombo->blockSignals(false);
	emit tabrulerChanged();
	emit tabsChanged();
}

void Tabruler::setTabFillChar(QChar t)
{
	if (t.isNull())
	{
		tabFillCombo->setEditable(false);
		tabFillCombo->setCurrentIndex(0);
	}
	else
		if (t == '.')
		{
			tabFillCombo->setEditable(false);
			tabFillCombo->setCurrentIndex(1);
		}
		else
			if (t == '-')
			{
				tabFillCombo->setEditable(false);
				tabFillCombo->setCurrentIndex(2);
			}
			else
				if (t == '_')
				{
					tabFillCombo->setEditable(false);
					tabFillCombo->setCurrentIndex(3);
				}
				else
				{
					tabFillCombo->setCurrentIndex(4);
					tabFillCombo->setEditable(true);
					if (!t.isNull())
					{
						bool sigBlocked = tabFillCombo->blockSignals(true);
						tabFillCombo->setEditText(QString(t));
						tabFillCombo->blockSignals(sigBlocked);
					}
				}
	emit tabrulerChanged();
	emit tabsChanged();
}

void Tabruler::setTabType(int t)
{
	typeCombo->setCurrentIndex(t);
	emit tabrulerChanged();
	emit tabsChanged();
}

void Tabruler::setType()
{
	typeCombo->blockSignals(true);
	ruler->changeTab(typeCombo->currentIndex());
	typeCombo->blockSignals(false);
	emit tabrulerChanged();
	emit tabsChanged();
}

void Tabruler::setTabData(double t)
{
	tabData->blockSignals(true);
	tabData->setValue(t * docUnitRatio);
	tabData->blockSignals(false);
	if (!ruler->mousePressed)
	{
		emit tabrulerChanged();
		emit tabsChanged();
	}
}

void Tabruler::setTab()
{
	ruler->moveTab(tabData->value() / docUnitRatio);
	emit tabrulerChanged();
	emit tabsChanged();
}

void Tabruler::setFirstLineData(double t)
{
//	firstLineData->blockSignals(true);
	firstLineData->setValue(t * docUnitRatio);
//	firstLineData->blockSignals(false);
	if (!ruler->mousePressed)
	{
		emit tabrulerChanged();
		double a, b, value;
		int c;
		firstLineData->getValues(&a, &b, &c, &value);
		emit firstLineChanged(value);
	}
}

void Tabruler::setFirstLine()
{
	ruler->moveFirstLine(firstLineData->value() / docUnitRatio);
	emit tabrulerChanged();
	double a, b, value;
	int c;
	firstLineData->getValues(&a, &b, &c, &value);
	emit firstLineChanged(value);
}

void Tabruler::setLeftIndentData(double t)
{
//	leftIndentData->blockSignals(true);
	leftIndentData->setValue(t * docUnitRatio);
//	leftIndentData->blockSignals(false);
	if (!ruler->mousePressed)
	{
		emit tabrulerChanged();
		double a, b, value;
		int c;
		leftIndentData->getValues(&a, &b, &c, &value);
		emit leftIndentChanged(value);
	}
}

void Tabruler::setLeftIndent()
{
	ruler->moveLeftIndent(leftIndentData->value() / docUnitRatio);
	emit tabrulerChanged();
	double a, b, value;
	int c;
	leftIndentData->getValues(&a, &b, &c, &value);
	emit leftIndentChanged(value);
}

QList<ParagraphStyle::TabRecord> Tabruler::getTabVals()
{
	return ruler->tabValues;
}

double Tabruler::getFirstLine()
{
	return firstLineData->value() / docUnitRatio;
}

double Tabruler::getLeftIndent()
{
	return leftIndentData->value() / docUnitRatio;
}

void Tabruler::setRightIndentData(double t)
{
	rightIndentData->blockSignals(true);
	rightIndentData->setValue(t * docUnitRatio);
	rightIndentData->blockSignals(false);
	if (!ruler->mousePressed)
	{
		emit tabrulerChanged();
		double a, b, value;
		int c;
		rightIndentData->getValues(&a, &b, &c, &value);
		emit rightIndentChanged(value);
	}
}

void Tabruler::setRightIndent()
{
	emit tabrulerChanged();
	double a, b, value;
	int c;
	rightIndentData->getValues(&a, &b, &c, &value);
	emit rightIndentChanged(value);
}

double Tabruler::getRightIndent()
{
	return rightIndentData->value() / docUnitRatio;
}

void Tabruler::slotMouseReleased()
{
	emit mouseReleased();
}
