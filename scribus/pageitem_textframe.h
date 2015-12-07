/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          pageitem.h  -  description
                             -------------------
    copyright            : Scribus Team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PAGEITEMTEXTFRAME_H
#define PAGEITEMTEXTFRAME_H

#include <QMap>
#include <QRectF>
#include <QString>
#include <QKeyEvent>
#include <text/boxes.h>
#include "scribusapi.h"
#include "pageitem.h"
#include "marks.h"
#include "notesstyles.h"
#include <util_math.h>
class PageItem_NoteFrame;
class ScPainter;
class ScribusDoc;

typedef QMap<PageItem_NoteFrame*, QList<TextNote *> > NotesInFrameMap;


//cezaryece: I remove static statement and made it public as this function is used also by PageItem_NoteFrame
double calculateLineSpacing (const ParagraphStyle &style, PageItem *item);

static const bool legacy = true;
enum TabStatus {
	TabNONE    = 0,
	TabLEFT    = TabNONE,
	TabRIGHT   = 1,
	TabPOINT   = 2,
	TabCOMMA   = 3,
	TabCENTER  = 4
};


/**
fields which describe what type of tab is currently active
 */
struct TabControl {
	bool         active;
	int          status;
	double       xPos;
	QChar        fillChar;
	GlyphLayout* tabGlyph;
};

struct LineSpec
{
	qreal x;
	qreal y;
	qreal width;
	qreal ascent;
	qreal descent;
	qreal colLeft;

	int firstChar;
	int lastChar;
	qreal naturalWidth;
};


struct LineControl {
	LineSpec line;
	QList<GlyphRun> glyphRuns;
	int      glFirstChar;
	int      charsInLine;
	int      hyphenCount;
	double   colWidth;
	double   colGap;
	double   colLeft;
	double   colRight;
	int      column;
	bool     startOfCol;
	bool     hasDropCap;
	bool     afterOverflow;
	bool     addLine;
	bool     recalculateY;
	bool     lastInRowLine;
	bool     addLeftIndent;
	bool     wasFirstInRow;
	double   leftIndent;
	double   rightMargin;
	double   mustLineEnd;
	int      restartIndex;  //index of char where line computing should be restarted
	int      restartRowIndex;  //index of char where row of text is started
	double   restartX; //starting X position of line if must be restarted
	double   rowDesc;

	double   ascend;
	double   descend;
	double   width;
	double   xPos;
	double   yPos;
	int      breakIndex;
	double   breakXPos;

	double   maxShrink;
	double   maxStretch;


	/// remember frame dimensions and offsets
	void init(double w, double h, const MarginStruct& extra, double lCorr)
	{
		insets = extra;
		lineCorr = lCorr;
		frameWidth = w;
		frameHeight = h;
		hasDropCap = false;
	}

	/// start at column 0
	void initColumns(double width, double gap)
	{
		column = 0;
		colWidth = width;
		colGap = gap;
	}

	/// move position to next column
	void nextColumn(double asce = 0.0)
	{
		startOfCol = true;
		colLeft = (colWidth + colGap) * column + insets.left() + lineCorr;
		//now colRight is REAL column right edge
		colRight = colLeft + colWidth;
		if (legacy)
			colRight += lineCorr;
		xPos = colLeft;
		yPos = asce + insets.top() + lineCorr;
	}

	bool isEndOfCol(double morespace = 0)
	{
		return yPos + morespace + insets.bottom() + lineCorr > frameHeight;
	}

	/**
		init fields for a new line at current position
	 */
	void startLine(int first)
	{
		glyphRuns.clear();
		charsInLine = 0;
		glFirstChar = 0;
		line.x = xPos;
		line.y = yPos;
		line.firstChar = first;
		glFirstChar = -first;
		line.lastChar = 0;
		line.ascent = 0.0;
		line.descent = 0.0;
		line.width = 0.0;
		line.naturalWidth = 0.0;
		line.colLeft = colLeft;
		breakIndex = -1;
		breakXPos = 0.0;
		maxShrink = 0.0;
		maxStretch = 0.0;
		width = 0.0;
		leftIndent = 0.0;
		rightMargin = 0.0;
		rowDesc = 0.0;
	}


	/// called when glyphs are placed on the line
	void rememberShrinkStretch(QChar ch, double wide, const ParagraphStyle& style)
	{
		if (SpecialChars::isExpandingSpace(ch))
			maxShrink += (1 - style.minWordTracking()) * wide;
		else
		{
			maxShrink += (1 - style.minGlyphExtension()) * wide;
		}
		maxStretch += (style.maxGlyphExtension() - 1) * wide;
	}

	/// called when a possible break is passed
	void rememberBreak(int index, double pos, double morespace = 0)
	{
		if (pos > colRight - morespace)
		{
			// only look for the first break behind the right edge
			//maxShrink = 0;

			// check if we already have a better break
			if (breakIndex >= 0)
			{
				double oldLooseness = qAbs(colRight - breakXPos);

				double newLooseness = pos - colRight;
				if (newLooseness >= oldLooseness)
					return;
			}
		}
		breakXPos = pos;
		breakIndex = index;
	}

	/// called when a mandatory break is found
	void breakLine(const StoryText& itemText, const ParagraphStyle& style, FirstLineOffsetPolicy offsetPolicy, int last)
	{
		breakIndex = last;
		breakXPos  = line.x;
		int nItems = glFirstChar + last + 1;
		for (int j = 0; j <= nItems; ++j)
		{
			if ( (glyphRuns[j].flags() & ScLayout_SuppressSpace) == 0 )
				breakXPos += glyphRuns[j].width();
		}
		// #8194, #8717 : update line ascent and descent with sensible values
		// so that endOfLine() returns correct result
		updateHeightMetrics(itemText);
		// #9060 : update line offset too
	//	updateLineOffset(itemText, style, offsetPolicy);
	}

	/// use the last remembered break to set line width and itemrange
	void finishLine(double endX)
	{
		line.lastChar = breakIndex;
		line.naturalWidth = breakXPos - line.x;
		line.width = endX - line.x;
		maxShrink = maxStretch = 0;
	}

	int restartRow(bool recalcY)
	{
		if (recalcY)
			yPos++;
		recalculateY = recalcY;
		xPos = restartX = colLeft;
		startLine(restartRowIndex);
		addLeftIndent = true;
		afterOverflow = false;
		return restartRowIndex -1;
	}

	int restartLine(bool recalcY, bool add)
	{
		recalculateY = recalcY;
		addLine = add;
		xPos = restartX;
		startLine(restartIndex);
		afterOverflow = false;
		return restartIndex -1;
	}

	bool isEndOfLine(double moreSpace = 0)
	{
		bool res;
		if (legacy)
			res = ceil(xPos + lineCorr - maxShrink) + ceil(moreSpace) >= floor(colRight);
		else
			res = ceil(xPos - maxShrink)  + ceil(moreSpace) >= floor(colRight);
		return res;
	}

	/// Keep old endOfLine code for reference
	/*double endOfLine_old(const QRegion& shape, const QTransform& pf2, double morespace, int Yasc, int Ydesc)
	{
		// if we aren't restricted further, we'll end at this maxX:
		double maxX = colRight - morespace;
		if (legacy) maxX -= lineCorr;

		double StartX = floor(qMax(line.x, qMin(maxX,breakXPos-maxShrink-1))-1);
		int xPos  = static_cast<int>(ceil(maxX));

		QPoint  pt12 (xPos, Yasc);
		QPoint  pt22 (xPos, Ydesc);
		QRect   pt(pt12,pt22);
		QRegion region;

		double EndX2 = StartX;
		double Interval = 0.25;
		do {
			int xP = static_cast<int>(ceil(EndX2 + morespace));
			pt.moveTopLeft(QPoint(xP, Yasc));
			region = QRegion(pf2.mapToPolygon(pt)).subtracted(shape);
			if (!region.isEmpty())
				break;
			EndX2 += Interval;
		} while ((EndX2 < maxX) && region.isEmpty());

		return qMin(EndX2, maxX);
	}*/

	/// find x position where this line must end
	double endOfLine(const QRegion& shape, double morespace, int yAsc, int yDesc)
	{
		// if we aren't restricted further, we'll end at this maxX:
		double maxX = colRight - morespace;
		if (legacy) maxX -= lineCorr;

		double StartX = floor(qMax(line.x, qMin(maxX, breakXPos-maxShrink-1))-1);
		StartX = qMax(0.0, StartX);

		int xPos  = static_cast<int>(ceil(maxX));
		QPoint  pt12 (xPos, yAsc);
		QPoint  pt22 (xPos, yDesc);

		QPolygon p;
		p.append (QPoint (StartX, yAsc));
		p.append (QPoint (StartX, yDesc));
		p.append (pt12);
		p.append (pt22);
		// check if something gets in the way
		QRegion lineI = shape.intersected (p.boundingRect());
		// if the intersection only has 1 rectangle, then nothing gets in the way
		if (lineI.rectCount() == 1)
		{
			int   cPos = static_cast<int>(ceil(StartX + morespace));
			QRect cRect (QPoint(cPos, yAsc), QPoint(cPos, yDesc));
			QRegion qr2 = QRegion(cRect).subtracted(shape);
			if (qr2.isEmpty()) // qr2 == 0 <=> cRect subset of shape
			{
				QRect rect = lineI.rects().at(0);
				double  mx = qMax(rect.left(), rect.right()) /*- pf2.dx()*/;
				int steps  = static_cast<int>((mx - StartX - morespace - 2) / 0.25);
				if (steps > 0)
				{
					StartX += steps * 0.25;
				}
			}
		}

		QRect   pt(pt12, pt22);

		double EndX2 = StartX;
		double Interval = 0.25;
		do {
			int xP = static_cast<int>(ceil(EndX2 + morespace));
			pt.moveTopLeft(QPoint(xP, yAsc));
			if (!regionContainsRect(shape, pt))
				break;
			EndX2 += Interval;
		} while ((EndX2 < maxX) && regionContainsRect(shape, pt));

		/*double oldEndX2 = endOfLine_old(shape, pf2, morespace, yAsc, yDesc);
		if (oldEndX2 != qMin(EndX2, maxX))
		{
			qDebug() << "Different EndX : " << oldEndX2 << " (old) " << EndX2 << " (new) ";
		}*/

		return qMin(EndX2, maxX);
	}

	double getLineAscent(const StoryText& itemText)
	{
		double result = 0;
		QChar firstChar = itemText.text (line.firstChar);
		if ((firstChar == SpecialChars::PAGENUMBER) || (firstChar == SpecialChars::PAGECOUNT))
			firstChar = '8';
		PageItem *obj = itemText.object (line.firstChar);
		const CharStyle& fcStyle(itemText.charStyle());
		if ((firstChar == SpecialChars::PARSEP) || (firstChar == SpecialChars::LINEBREAK))
			result = fcStyle.font().ascent(fcStyle.fontSize() / 10.0);
		else if (obj)
			result = qMax(result, (obj->height() + obj->lineWidth()) * (fcStyle.scaleV() / 1000.0));
		else
			result = fcStyle.font().realCharAscent(firstChar, fcStyle.fontSize() / 10.0);
		for (int zc = 0; zc < charsInLine; ++zc)
		{
			QChar ch = itemText.text(line.firstChar + zc);
			if ((ch == SpecialChars::PAGENUMBER) || (ch == SpecialChars::PAGECOUNT))
				ch = '8'; // should have highest ascender even in oldstyle
			const CharStyle& cStyle(itemText.charStyle(line.firstChar + zc));
			if ((ch == SpecialChars::TAB) || (ch == QChar(10))
				|| SpecialChars::isBreak (ch, true) || (ch == SpecialChars::NBHYPHEN) || (ch.isSpace()))
				continue;
			double asce;
			PageItem *obj = itemText.object (line.firstChar + zc);
			if (obj)
				asce = obj->height() + obj->lineWidth() * (cStyle.scaleV() / 1000.0);
			else
				asce = cStyle.font().realCharAscent(ch, cStyle.fontSize() / 10.0);
			//	qDebug() << QString("checking char 'x%2' with ascender %1 > %3").arg(asce).arg(ch.unicode()).arg(result);
			result = qMax(result, asce);
		}
		return result;
	}

	double getLineDescent(const StoryText& itemText)
	{
		double result = 0;
		QChar  firstChar = itemText.text(line.firstChar);
		if ((firstChar == SpecialChars::PAGENUMBER) || (firstChar == SpecialChars::PAGECOUNT))
			firstChar = '8';
		const CharStyle& fcStyle(itemText.charStyle(line.firstChar));
		if ((firstChar == SpecialChars::PARSEP) || (firstChar == SpecialChars::LINEBREAK))
			result = fcStyle.font().descent(fcStyle.fontSize() / 10.0);
		else if (itemText.object(line.firstChar))
			result = 0.0;
		else
			result = fcStyle.font().realCharDescent(firstChar, fcStyle.fontSize() / 10.0);
		for (int zc = 0; zc < charsInLine; ++zc)
		{
			QChar ch = itemText.text(line.firstChar + zc);
			if ((ch == SpecialChars::PAGENUMBER) || (ch == SpecialChars::PAGECOUNT))
				ch = '8'; // should have highest ascender even in oldstyle
			const CharStyle& cStyle(itemText.charStyle(line.firstChar + zc));
			if ((ch == SpecialChars::TAB) || (ch == QChar(10))
				|| SpecialChars::isBreak (ch, true) || (ch == SpecialChars::NBHYPHEN) || (ch.isSpace()))
				continue;
			double desc;
			if (itemText.object(line.firstChar + zc))
				desc = 0.0;
			else
				desc = cStyle.font().realCharDescent(ch, cStyle.fontSize() / 10.0);
			//	qDebug() << QString("checking char 'x%2' with ascender %1 > %3").arg(asce).arg(ch.unicode()).arg(result);
			result = qMax(result, desc);
		}
		return result;
	}

	double getLineHeight(const StoryText& itemText)
	{
		double result = 0;
		const CharStyle& firstStyle(itemText.charStyle(line.firstChar));
		PageItem *obj = itemText.object (line.firstChar);
		if (obj)
			result = qMax(result, (obj->height() + obj->lineWidth()) * (firstStyle.scaleV() / 1000.0));
		else
			result = firstStyle.font().height(firstStyle.fontSize() / 10.0);
		for (int zc = 0; zc < charsInLine; ++zc)
		{
			QChar ch = itemText.text(line.firstChar+zc);
			if ((ch == SpecialChars::TAB) || (ch == QChar(10))
				|| SpecialChars::isBreak (ch, true) || (ch == SpecialChars::NBHYPHEN) || (ch.isSpace()))
				continue;
			const CharStyle& cStyle(itemText.charStyle(line.firstChar + zc));
			PageItem *obj = itemText.object (line.firstChar + zc);
			double asce;
			if (obj)
				asce = (obj->height() + obj->lineWidth()) * (cStyle.scaleV() / 1000.0);
			else
				asce = cStyle.font().height (cStyle.fontSize() / 10.0);
			//	qDebug() << QString("checking char 'x%2' with ascender %1 > %3").arg(asce).arg(ch.unicode()).arg(result);
			result = qMax(result, asce);
		}
		return result;
	}

	void updateHeightMetrics(const StoryText& itemText)
	{
		double asce, desc;
		line.ascent  = 0;
		line.descent = 0;
		for (int zc = 0; zc < charsInLine; ++zc)
		{
			QChar ch = itemText.text(line.firstChar+zc);
			if ((ch == SpecialChars::TAB) || (ch == QChar(10))
				|| SpecialChars::isBreak (ch, true) || (ch == SpecialChars::NBHYPHEN) || (ch.isSpace()))
				continue;
			const CharStyle& cStyle(itemText.charStyle(line.firstChar + zc));
			double scaleV = cStyle.scaleV() / 1000.0;
			double offset = (cStyle.fontSize() / 10) * (cStyle.baselineOffset() / 1000.0);

			if (itemText.object(line.firstChar+zc) != 0)
			{
				asce = (itemText.object(line.firstChar+zc)->height() + itemText.object(line.firstChar+zc)->lineWidth()) * scaleV + offset;
				desc = 0.0;
			}
			else //if ((itemText.flags(current.line.firstChar+zc) & ScLayout_DropCap) == 0)
			{
				asce = cStyle.font().realCharAscent(ch, cStyle.fontSize() / 10.0) * scaleV + offset;
				desc = cStyle.font().realCharDescent(ch, cStyle.fontSize() / 10.0) * scaleV - offset;
			}
			//	qDebug() << QString("checking char 'x%2' with ascender %1 > %3").arg(asce).arg(ch.unicode()).arg(result);
			line.ascent  = qMax(line.ascent, asce);
			line.descent = qMax(line.descent, desc);
		}
	}

// yPos should not be changed when all line is already calculated - at new y position there can be overflow!!!
// edit: can't happen as it should only move upwards, and this is covered by the calculations done.
//void updateLineOffset(const StoryText& itemText, const ParagraphStyle& style, FirstLineOffsetPolicy offsetPolicy)
//{
//	if (charsInLine <= 0)
//		return;
//	if ((!hasDropCap) && (startOfCol) && (style.lineSpacingMode() != ParagraphStyle::BaselineGridLineSpacing))
//	{
//		//FIXME: use glyphs, not chars
//		double firstasce = itemText.charStyle(line.firstChar).font().ascent(itemText.charStyle(line.firstChar).fontSize() / 10.0);
//		double adj (0.0);
//		double currasce (this->getLineAscent(itemText));
//		if (offsetPolicy == FLOPRealGlyphHeight)
//		{
//			adj = firstasce - currasce;
//		}
//		else if (offsetPolicy == FLOPFontAscent)
//		{
//			adj = 0.0;
//		}
//		else if (offsetPolicy == FLOPLineSpacing)
//		{
//			adj = firstasce - style.lineSpacing();
//		}
//		line.ascent = currasce;
//		line.y -= adj;
//		yPos -= adj;
//	}
//	else if ((!startOfCol) && (style.lineSpacingMode() == ParagraphStyle::AutomaticLineSpacing))
//	{
//		QChar ch = itemText.text(line.firstChar);
//		double firstasce = style.lineSpacing();
//		double currasce  = getLineHeight(itemText);
//		double adj = firstasce - currasce;
//		qDebug() << QString("move2 line %1.. down by %2").arg(current.line.firstChar).arg(-adj);
//		line.ascent = currasce;
//		line.y -= adj;
//		yPos -= adj;
//	}
//}

	LineBox* createLineBox()
	{
		LineBox* result = new LineBox();
		result->moveTo(line.x, line.y);
		result->setWidth(line.width);
		result->setAscent(line.ascent);
		result->setDescent(line.descent);
		result->colLeft = line.colLeft;
		qreal pos = line.colLeft;
		for (int i = 0; i < glyphRuns.count(); ++i)
		{
			GlyphBox* glyphbox = createGlyphBox(glyphRuns.at(i));
			glyphbox->moveBy(pos, 0);
			pos += glyphbox->width();
			result->addBox(glyphbox);
		}
		return result;
	}

	GlyphBox* createGlyphBox(const GlyphRun& run)
	{
		GlyphBox* result = new GlyphBox(run);
		result->setWidth(run.width());
		return result;
	}

private:
	double frameWidth;
	double frameHeight;
	MarginStruct insets;
	double lineCorr;
};

class SCRIBUS_API PageItem_TextFrame : public PageItem
{
	Q_OBJECT

public:
	PageItem_TextFrame(ScribusDoc *pa, double x, double y, double w, double h, double w2, QString fill, QString outline);
	PageItem_TextFrame(const PageItem & p);
	~PageItem_TextFrame() {};

	virtual PageItem_TextFrame * asTextFrame() { return this; }
	virtual bool isTextFrame() const { return true; }
	
	virtual void clearContents();
	virtual void truncateContents();
	const GlyphBox* m_gb;
	/**
	* \brief Handle keyboard interaction with the text frame while in edit mode
	* @param k key event
	* @param keyRepeat a reference to the keyRepeat property
	*/
	virtual void handleModeEditKey(QKeyEvent *k, bool& keyRepeat);
	void deleteSelectedTextFromFrame();
	void setNewPos(int oldPos, int len, int dir);
	void ExpandSel(int dir, int oldPos);
	void deselectAll();
	
	//for speed up updates when changed was only one frame from chain
	virtual void invalidateLayout(bool wholeChain);
	using PageItem::invalidateLayout;
	virtual void layout();
	//return true if all previouse frames from chain are valid (including that one)
	bool isValidChainFromBegin();
	//simplify conditions checking if frame is in chain
	//FIX: use it in other places
	bool isInChain() { return ((prevInChain() != NULL) || (nextInChain() != NULL)); }
	void setTextAnnotationOpen(bool open);

	double columnWidth();

    //enable/disable marks inserting actions depending on editMode
	void toggleEditModeActions();
	QRegion availableRegion() { return m_availableRegion; }

protected:
	QRegion calcAvailableRegion();
	QRegion m_availableRegion;
	virtual void DrawObj_Item(ScPainter *p, QRectF e);
	virtual void DrawObj_Post(ScPainter *p);
	virtual void DrawObj_Decoration(ScPainter *p);
	//void drawOverflowMarker(ScPainter *p);
	void drawUnderflowMarker(ScPainter *p);
	void drawColumnBorders(ScPainter *p);
	
	bool unicodeTextEditMode;
	int unicodeInputCount;
	QString unicodeInputString;

	void drawNoteIcon(ScPainter *p);
	virtual bool createInfoGroup(QFrame *, QGridLayout *);
	virtual void applicableActions(QStringList& actionList);
	virtual QString infoDescription();
	// Move incomplete lines from the previous frame if needed.
	bool moveLinesFromPreviousFrame ();
	void adjustParagraphEndings ();

private:
	bool cursorBiasBackward;
	// If the last paragraph had to be split, this is how many lines of the paragraph are in this frame.
	// Used for orphan/widow control
	int incompleteLines;
	// This holds the line splitting positions
	QList<int> incompletePositions;

	void setShadow();
	QString m_currentShadow;
	QMap<QString,StoryText> m_shadows;
	bool checkKeyIsShortcut(QKeyEvent *k);
	QRectF m_origAnnotPos;
	
private slots:
	void slotInvalidateLayout();

public:
	//for footnotes/endnotes
	bool hasNoteMark(NotesStyle* NS = NULL);
	bool hasNoteFrame(NotesStyle* NS, bool inChain = false);
	//bool hasNoteFrame(PageItem_NoteFrame* nF) { return m_notesFramesMap.contains(nF); }
	void delAllNoteFrames(bool doUpdate = false);
	void removeNoteFrame(PageItem_NoteFrame* nF) { m_notesFramesMap.remove(nF); }
	//layout notes frames /updates endnotes frames content if m_Doc->flag_updateEndNotes is set/
	void notesFramesLayout();
	//removing all marsk from text, returns number of removed marks
	int removeMarksFromText(bool doUndo);
	//return note frame for given notes style if current text frame has notes marks with this style
	PageItem_NoteFrame* itemNoteFrame(NotesStyle* nStyle);
	//list all notes frames for current text frame /with endnotes frames if endnotes marks are in that frame/
	QList<PageItem_NoteFrame*> notesFramesList() { return m_notesFramesMap.keys(); }
	//list of notes inserted by current text frame into given noteframe
	QList<TextNote*> notesList(PageItem_NoteFrame* nF) { return m_notesFramesMap.value(nF); }
	//insert note frame to list with empty notes list
	void setNoteFrame(PageItem_NoteFrame* nF);
	void invalidateNotesFrames();

private:
	NotesInFrameMap m_notesFramesMap;
	NotesInFrameMap updateNotesFrames(QMap<int, Mark*> noteMarksPosMap); //update notes frames content
	void updateNotesMarks(NotesInFrameMap notesMap);
	Mark* selectedMark(bool onlySelection = true);
    TextNote* selectedNoteMark(int& foundPos, bool onlySelection = true);
	TextNote* selectedNoteMark(bool onlySelection = true);
protected:
	// set text frame height to last line of text
	double maxY;
	void setMaxY(double y);

public:
	void setTextFrameHeight();
};

#endif
