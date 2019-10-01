/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          storyeditor.h    -  description
                             -------------------
    begin                : Tue Nov 11 2003
    copyright            : (C) 2003 by Franz Schmid
    email                : Franz.Schmid@altmuehlnet.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STORYEDITOR_H
#define STORYEDITOR_H

#include <QTextEdit>
#include <QAction>
#include <QCheckBox>
#include <QCloseEvent>
#include <QEvent>
#include <QFocusEvent>
#include <QHideEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QList>
#include <QMainWindow>
#include <QMap>
#include <QMenu>
#include <QMenuBar>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPair>
#include <QPixmap>
#include <QPointer>
#include <QShowEvent>
#include <QSpinBox>
#include <QSplitter>
#include <QStack>
#include <QStatusBar>
#include <QStringList>
#include <QToolBar>
#include <QToolButton>
#include <QVariant>
#include <QWidgetAction>

class QEvent;
class QGridLayout;
class QHBoxLayout;
class QFrame;

#include "scribusapi.h"
#include "text/storytext.h"

class PageItem;
class MenuManager;
class ScrSpinBox;
class ParaStyleComboBox;
class StyleSelect;
class AlignSelect;
class DirectionSelect;
class FontCombo;
class ScrAction;
class ShadeButton;
class PrefsManager;
class PrefsContext;
class StoryEditor;
class ColorCombo;
class CharSelect;

class SCRIBUS_API SEditor : public QTextEdit
{
	Q_OBJECT

	friend class StoryEditor;
	friend class SideBar;

public:
	SEditor (QWidget* parent, ScribusDoc *docc, StoryEditor* parentSE);
	~SEditor() {}
	void setCurrentDocument(ScribusDoc *docc);
	void setAlign(int align);
	void setDirection(int align);
	void saveItemText(PageItem *currItem);
	void loadItemText(PageItem *currItem);
	void loadText(const QString& tx, PageItem *currItem);
	void updateAll();
	void updateFromChars(int p);
	void updateSel(const CharStyle& style);
	void updateSel(const ParagraphStyle& style);
	void deleteSel();
	void setEffects(int effects);
	void setColor(bool marker);

	void insertChars(const QString& text);
	void insertChars(const QString& styledText, const QString& editText);

	StoryText StyledText;

protected:

	void insertCharsInternal(const QString& t);
	void insertCharsInternal(const QString& t, int position);

	void insertStyledText(const StoryText& styledText);
	void insertStyledText(const StoryText& styledText, int position);

	void insertUpdate(int position, int len);

	void setAlign(QTextCursor& tCursor, int style);
	void setDirection(QTextCursor& tCursor, int style);
	void setEffects(QTextCursor& tCursor, int effects);

	int  blockContentsChangeHook;
	void keyPressEvent(QKeyEvent *k);
	void inputMethodEvent(QInputMethodEvent *event);
	void focusOutEvent(QFocusEvent *e);
	void focusInEvent(QFocusEvent *e);
	void scrollContentsBy(int dx, int dy);
	virtual bool canInsertFromMimeData( const QMimeData * source ) const;
	virtual QMimeData * createMimeDataFromSelection () const;
	virtual void insertFromMimeData ( const QMimeData * source );
	StoryEditor* parentStoryEditor;

	QList<PageItem*> FrameItems;
	ScribusDoc* doc;
	bool unicodeTextEditMode;
	bool wasMod;
	bool ready;
	int unicodeInputCount;
	StyleFlag CurrentEffects;
	QString currentParaStyle;
	int CurrAlign;
	int CurrDirection;
	double CurrFontSize;
	double CurrTextFillSh;
	double CurrTextStrokeSh;
	double CurrTextScaleH;
	double CurrTextScaleV;
	double CurrTextBase;
	double CurrTextShadowX;
	double CurrTextShadowY;
	double CurrTextOutline;
	double CurrTextUnderPos;
	double CurrTextUnderWidth;
	double CurrTextStrikePos;
	double CurrTextStrikeWidth;
	double CurrTextKern;
	QString CurrTextStroke;
	QString CurrTextFill;
	QString prevFont;
	QString CurrFont;
	QString unicodeInputString;

	QStack< QPair<int, int> > SelStack;

	int SelCharStart;
	int SelCharEnd;
	int SuspendContentsChange;	// input method

protected slots:
	void handleContentsChange(int position, int charsRemoved, int charsAdded); 

public slots:
	void cut();
	void copy();
	void paste();
	void ClipChange();
	void SelClipChange();

signals:
	void setProps(int, int);
	void SideBarUp(bool);
	void SideBarUpdate();
	void PasteAvail();
	void contentsMoving(int, int);
};

class SCRIBUS_API SideBar : public QLabel
{
	Q_OBJECT

public:
	SideBar(QWidget *pa);
	~SideBar() {};
	int offs;
	int currentPar;
	SEditor *editor;
	QMenu *pmen;
	QWidgetAction* paraStyleAct;
	bool noUpdt;
	bool inRep;

protected:
	void paintEvent(QPaintEvent *e);
	void mouseReleaseEvent(QMouseEvent *m);

signals:
	void ChangeStyle(int, const QString&);
	//! signal raised when user ask for style manager
	//void sigEditStyles();

public slots:
	void setPStyle(const QString&);
	void doMove(int x, int y);
	void doRepaint();
	void setRepaint(bool r);
	//! slot raising style manager
	//void editStyles();
};

class SCRIBUS_API SToolBColorF : public QToolBar
{
	Q_OBJECT

public:
	SToolBColorF(QMainWindow* parent, ScribusDoc *doc);
	~SToolBColorF() {};
	
	virtual void changeEvent(QEvent *e);
	
	void setCurrentDocument(ScribusDoc *doc);
	QLabel* FillIcon;
	ColorCombo* TxFill;
	ShadeButton *PM2;
	QAction* pm2Action;
	QAction* txFillAction;
	QAction* fillIconAction;

public slots:
	void SetColor(int c);
	void SetShade(double s);
	void newShadeHandler();
	void languageChange();

signals:
	void NewColor(int, int);
};

class SCRIBUS_API SToolBColorS : public QToolBar
{
	Q_OBJECT

public:
	SToolBColorS(QMainWindow* parent, ScribusDoc *doc);
	~SToolBColorS() {};
	
	virtual void changeEvent(QEvent *e);
	
	void setCurrentDocument(ScribusDoc *doc);
	QLabel* StrokeIcon;
	ColorCombo* TxStroke;
	ShadeButton *PM1;
	QAction* strokeIconAction;
	QAction* txStrokeAction;
	QAction* pm1Action;


public slots:
	void SetColor(int c);
	void SetShade(double s);
	void newShadeHandler();
	void languageChange();

signals:
	void NewColor(int, int);
};

class SCRIBUS_API SToolBStyle : public QToolBar
{
	Q_OBJECT

public:
	SToolBStyle(QMainWindow* parent);
	~SToolBStyle() {};
	
	virtual void changeEvent(QEvent *e);
	
	StyleSelect* SeStyle;
	QLabel* trackingLabel;
	ScrSpinBox* Extra;
	QAction* seStyleAction;
	QAction* trackingLabelAction;
	QAction* extraAction;

public slots:
	void newStrikeHandler();
	void newUnderlineHandler();
	void newOutlineHandler();
	void newShadowHandler();
	void newKernHandler();
	void SetShadow(double x, double y);
	void setStrike(double p, double w);
	void setUnderline(double p, double w);
	void setOutline(double o);
	void SetStyle(int s);
	void SetKern(double k);
	void languageChange();

signals:
	void NewKern(double);
	void NewShadow(double, double);
	void newOutline(double);
	void newStrike(double, double);
	void newUnderline(double, double);
	void newStyle(int);
};

class SCRIBUS_API SToolBAlign : public QToolBar
{
	Q_OBJECT

public:
	SToolBAlign(QMainWindow* parent);
	~SToolBAlign() {};
	
	virtual void changeEvent(QEvent *e);
	
	AlignSelect* GroupAlign;
	DirectionSelect* GroupDirection;
	ParaStyleComboBox *paraStyleCombo;
	QAction* groupAlignAction;
	QAction* groupDirectionAction;
	QAction* paraStyleComboAction;

public slots:
	void SetAlign(int s);
	void SetDirection(int s);
	void SetParaStyle(const QString& s);
	void languageChange();

signals:
	void newAlign(int);
	void newDirection(int);
	void newParaStyle(const QString&);
};

class SCRIBUS_API SToolBFont : public QToolBar
{
	Q_OBJECT

public:
	SToolBFont(QMainWindow* parent);
	~SToolBFont() {};
	
	virtual void changeEvent(QEvent *e);
	
	FontCombo* Fonts;
	ScrSpinBox* charScaleH;
	ScrSpinBox* charScaleV;
	QAction* fontsAction;
	QAction* chScaleHAction;
	QAction* chScaleVAction;

public slots:
	void SetFont(const QString& f);
	void SetSize(double s);
	void SetScaleH(double s);
	void SetScaleV(double s);

signals:
	void newFont(const QString &);
	void newSize(double);
	void newScaleH(double);
	void newScaleV(double);

private:
	ScrSpinBox* Size;
	QLabel*  lblScaleTxtH;
	QLabel*  lblScaleTxtV;
	QAction* sizeAction;
	QAction* scaleTxtHAction;
	QAction* scaleTxtVAction;

private slots:
	void languageChange();
};

class SCRIBUS_API StoryEditor : public QMainWindow
{
	Q_OBJECT
	friend class SEditor;

public:
	StoryEditor( QWidget* parent );
// 	StoryEditor( QWidget* parent, ScribusDoc *docc, PageItem* ite );
	~StoryEditor();

	virtual void changeEvent(QEvent *e);

	void setCurrentDocumentAndItem(ScribusDoc *doc=nullptr, PageItem *item=nullptr);
	void setSpellActive(bool ssa);

	ScribusDoc* currentDocument() const;
	PageItem* currentItem() const;
	SEditor* Editor;
	bool activFromApp;
	MenuManager* seMenuMgr;
	QMap<QString, QPointer<ScrAction> > seActions;

public slots:
	void newStyle(const QString&);
	void newAlign(int st);
	void newDirection(int dir);
	void newTxFill(int c, int s);
	void newTxStroke(int c, int s);
	void newTxFont(const QString &f);
	void newTxSize(double s);
	void newTxStyle(int s);

signals:
	void DocChanged();
	void EditSt();

private:
	//int exec();
	void changeAlign(int align);
	void changeDirection(int align);
	bool textDataChanged() const;

	/*! \brief Enables/disables the "smart" selection (#1203) - 10/16/2004 pv */
	bool m_smartSelection;

	ScribusDoc* m_doc;
	PageItem* m_item;

	bool m_textChanged;
	bool m_firstSet;
	bool m_blockUpdate;
	
	int m_result;
//	int m_currPara;
//	int m_currChar;

protected slots:
	void setBackPref();
	void setFontPref();
	void newTxScale();
	void newTxScaleV();
	void newTxKern(double s);
	void newShadowOffs(double x, double y);
	void newTxtOutline(double o);
	void newTxtUnderline(double p, double w);
	void newTxtStrike(double p, double w);
	void updateProps();
	void updateProps(QTextCursor &cur);
	void updateProps(int p, int ch);

	void changeStyleSB(int pa, const QString&);
	void changeStyle();
	void updateStatus();
	void Do_leave();
	void Do_leave2();
	//! \brief Saves the document with editation continued. Signal called from menu.
	void Do_saveDocument();
	bool Do_new();
	void slotFileRevert();

	void Do_selectAll();
	void Do_copy();
	void Do_paste();
	void Do_cut();
	void Do_del();
	void Do_insSp();
	/*! \brief Insert a Lorem Ipsum (from dialog) into
	current position inthe SE.
	*/
	void insertSampleText();
	void Do_fontPrev();
	void CopyAvail(bool u);
	void PasteAvail();
	void updateTextFrame();
	void SearchText();
	//void slotEditStyles();
	void modifiedText();
	void LoadTextFile();
	void SaveTextFile();
	void setSmart(bool);
	void languageChange();

	void specialActionKeyEvent(int unicodevalue);
	/*! \brief Slot to insert special characters from charSelect widget. */
	void slot_insertSpecialChar();
	void slot_insertUserSpecialChar(QChar, const QString&);
	// 10/12/2004 - pv - #1203: wrong selection on double click
	void doubleClick(int para, int pos);

protected:
	void initActions();
	void updateUnicodeActions();
	void buildMenus();
	void buildGUI();
	void setupEditorGUI();
	void connectSignals();
	void disconnectSignals();
	/*! \brief Loading the preferences (position).
	It's in separate method due the 2 constructors. */
	void loadPrefs();
	/*! \brief Saving the preferences (position). */
	void savePrefs();

	/*! \brief Special Characters dialog.
	It uses a little bit ugly method to operate with.
	It's a duplication of the main window's charPalette due
	the lack of setParent() in qt3 and a ugly behaviour of
	the reparent().
	charSelect is created as a copy of the charPalette.
	\author Petr Vanek <petr@scribus.info>
	*/
	CharSelect *charSelect;
    //! True when there were some glyphs inserted via charSelect
	bool charSelectUsed;

	void showEvent(QShowEvent *);
	void hideEvent(QHideEvent *);

	void closeEvent(QCloseEvent *);
	void keyPressEvent (QKeyEvent * e);
	bool eventFilter( QObject* ob, QEvent* ev );

	QHBoxLayout* StoryEd2Layout;
	QGridLayout* ButtonGroup1Layout;
	QGridLayout* ButtonGroup2Layout;

	PrefsManager& prefsManager;
	PrefsContext* prefs;

	QStringList unicodeCharActionNames;
	QPixmap noIcon;

	QToolBar* FileTools;
	SToolBFont* FontTools;
	SToolBAlign* AlignTools;
	SToolBColorF* FillTools;
	SToolBColorS* StrokeTools;
	SToolBStyle* StyleTools;
	QSplitter* EdSplit;
	SideBar* EditorBar;
	QFrame* ButtonGroup1;
	QFrame* ButtonGroup2;
	QLabel* WordCT1;
	QLabel* WordCT3;
	QLabel* ParCT;
	QLabel* ParC;
	QLabel* WordCT;
	QLabel* WordC;
	QLabel* CharCT;
	QLabel* CharC;
	QLabel* WordCT2;
	QLabel* WordC2;
	QLabel* CharCT2;
	QLabel* CharC2;
	
	bool m_spellActive;
};

#endif


