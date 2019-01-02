/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
02.01.2008: Joachim Neu - joachim_neu@web.de - http://www.joachim-neu.de
*/
#include "cmdstyle.h"
#include "cmdutil.h"

#include "qbuffer.h"
#include "qpixmap.h"
//Added by qt3to4:
#include <QList>

#include "scribuscore.h"
#include "styles/paragraphstyle.h"
#include "styles/charstyle.h"
#include "ui/stylemanager.h"

/*! 02.01.2007 - 05.01.2007 : Joachim Neu : Create a paragraph style.
			Special thanks go to avox for helping me! */
PyObject *scribus_createparagraphstyle(PyObject* /* self */, PyObject* args, PyObject* keywords)
{
	//TODO - new paragraph properties for bullets and numbering
	char* keywordargs[] = {
			const_cast<char*>("name"),
			const_cast<char*>("linespacingmode"),
			const_cast<char*>("linespacing"),
			const_cast<char*>("alignment"),
			const_cast<char*>("leftmargin"),
			const_cast<char*>("rightmargin"),
			const_cast<char*>("gapbefore"),
			const_cast<char*>("gapafter"),
			const_cast<char*>("firstindent"),
			const_cast<char*>("hasdropcap"),
			const_cast<char*>("dropcaplines"),
			const_cast<char*>("dropcapoffset"),
			const_cast<char*>("charstyle"),
			nullptr};
	char *Name = const_cast<char*>(""), *CharStyle = const_cast<char*>("");
	int LineSpacingMode = 0, Alignment = 0, DropCapLines = 2, HasDropCap = 0;
	double LineSpacing = 15.0, LeftMargin = 0.0, RightMargin = 0.0;
	double GapBefore = 0.0, GapAfter = 0.0, FirstIndent = 0.0, PEOffset = 0;
	if (!PyArg_ParseTupleAndKeywords(args, keywords, "es|ididddddiides",
		 keywordargs, "utf-8", &Name, &LineSpacingMode, &LineSpacing, &Alignment,
		&LeftMargin, &RightMargin, &GapBefore, &GapAfter, &FirstIndent,
		&HasDropCap, &DropCapLines, &PEOffset, "utf-8", &CharStyle))
		return nullptr;
	if (!checkHaveDocument())
		return nullptr;
	if (strlen(Name) == 0)
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Cannot have an empty paragraph style name.","python error").toLocal8Bit().constData());
		return nullptr;
	}

	ParagraphStyle TmpParagraphStyle;
	TmpParagraphStyle.setName(Name);
	TmpParagraphStyle.setLineSpacingMode((ParagraphStyle::LineSpacingMode)LineSpacingMode);
	TmpParagraphStyle.setLineSpacing(LineSpacing);
	TmpParagraphStyle.setAlignment((ParagraphStyle::AlignmentType)Alignment);
	TmpParagraphStyle.setLeftMargin(LeftMargin);
	TmpParagraphStyle.setFirstIndent(FirstIndent);
	TmpParagraphStyle.setRightMargin(RightMargin);
	TmpParagraphStyle.setGapBefore(GapBefore);
	TmpParagraphStyle.setGapAfter(GapAfter);
	if (HasDropCap == 0)
		TmpParagraphStyle.setHasDropCap(false);
	else if (HasDropCap == 1)
		TmpParagraphStyle.setHasDropCap(true);
	else
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("hasdropcap has to be 0 or 1.","python error").toLocal8Bit().constData());
		return nullptr;
	}
	TmpParagraphStyle.setDropCapLines(DropCapLines);
	TmpParagraphStyle.setParEffectOffset(PEOffset);
	TmpParagraphStyle.charStyle().setParent(CharStyle);

	StyleSet<ParagraphStyle> TmpStyleSet;
	TmpStyleSet.create(TmpParagraphStyle);
	ScCore->primaryMainWindow()->doc->redefineStyles(TmpStyleSet, false);
	// PV - refresh the Style Manager window.
	// I thought that this can work but it doesn't:
	// ScCore->primaryMainWindow()->styleMgr()->reloadStyleView();
	// So the brute force setDoc is called...
	ScCore->primaryMainWindow()->styleMgr()->setDoc(ScCore->primaryMainWindow()->doc);

	Py_RETURN_NONE;
}

/*! 03.01.2007 - 05.01.2007 : Joachim Neu : Create a char style.
			Special thanks go to avox for helping me! */
PyObject *scribus_createcharstyle(PyObject* /* self */, PyObject* args, PyObject* keywords)
{
	char* keywordargs[] = {
					  							const_cast<char*>("name"),
					  							const_cast<char*>("font"),
					  							const_cast<char*>("fontsize"),
												const_cast<char*>("fontfeatures"),
					  							const_cast<char*>("features"),
					  							const_cast<char*>("fillcolor"),
					  							const_cast<char*>("fillshade"),
					  							const_cast<char*>("strokecolor"),
					  							const_cast<char*>("strokeshade"),
					  							const_cast<char*>("baselineoffset"),
					  							const_cast<char*>("shadowxoffset"),
					  							const_cast<char*>("shadowyoffset"),
					  							const_cast<char*>("outlinewidth"),
					  							const_cast<char*>("underlineoffset"),
					  							const_cast<char*>("underlinewidth"),
					  							const_cast<char*>("strikethruoffset"),
					  							const_cast<char*>("strikethruwidth"),
					  							const_cast<char*>("scaleh"),
					  							const_cast<char*>("scalev"),
					  							const_cast<char*>("tracking"),
					  							const_cast<char*>("language"),
											nullptr};
	char *Name = const_cast<char*>(""), *Font = const_cast<char*>("Times"), *Features = const_cast<char*>("inherit"), *FillColor = const_cast<char*>("Black"), *FontFeatures = const_cast<char*>(""), *StrokeColor = const_cast<char*>("Black"), *Language = const_cast<char*>("");
	double FontSize = 200, FillShade = 1, StrokeShade = 1, ScaleH = 1, ScaleV = 1, BaselineOffset = 0, ShadowXOffset = 0, ShadowYOffset = 0, OutlineWidth = 0, UnderlineOffset = 0, UnderlineWidth = 0, StrikethruOffset = 0, StrikethruWidth = 0, Tracking = 0;
	if (!PyArg_ParseTupleAndKeywords(args, keywords, "es|esdesesdesddddddddddddes", keywordargs,
																									"utf-8", &Name, "utf-8", &Font, &FontSize, "utf-8", &Features,
																									"utf-8", &FillColor, &FillShade, "utf-8", &StrokeColor, &StrokeShade, &BaselineOffset, &ShadowXOffset,
																									&ShadowYOffset, &OutlineWidth, &UnderlineOffset, &UnderlineWidth, &StrikethruOffset, &StrikethruWidth,
																									&ScaleH, &ScaleV, &Tracking, "utf-8", &Language))
		return nullptr;
	if (!checkHaveDocument())
		return nullptr;
	if (strlen(Name) == 0)
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Cannot have an empty char style name.","python error").toLocal8Bit().constData());
		return nullptr;
	}

	QStringList FeaturesList = QString(Features).split(QString(","));

	CharStyle TmpCharStyle;
	TmpCharStyle.setName(Name);
	TmpCharStyle.setFont((*ScCore->primaryMainWindow()->doc->AllFonts)[QString(Font)]);
	TmpCharStyle.setFontSize(FontSize * 10);
	TmpCharStyle.setFontFeatures(FontFeatures);
	TmpCharStyle.setFeatures(FeaturesList);
	TmpCharStyle.setFillColor(QString(FillColor));
	TmpCharStyle.setFillShade(FillShade * 100);
	TmpCharStyle.setStrokeColor(QString(StrokeColor));
	TmpCharStyle.setStrokeShade(StrokeShade * 100);
	TmpCharStyle.setBaselineOffset(BaselineOffset);
	TmpCharStyle.setShadowXOffset(ShadowXOffset);
	TmpCharStyle.setShadowYOffset(ShadowYOffset);
	TmpCharStyle.setOutlineWidth(OutlineWidth);
	TmpCharStyle.setUnderlineOffset(UnderlineOffset);
	TmpCharStyle.setUnderlineWidth(UnderlineWidth);
	TmpCharStyle.setStrikethruOffset(StrikethruOffset);
	TmpCharStyle.setStrikethruWidth(StrikethruWidth);
	TmpCharStyle.setScaleH(ScaleH * 1000);
	TmpCharStyle.setScaleV(ScaleV * 1000);
	TmpCharStyle.setTracking(Tracking);
	TmpCharStyle.setLanguage(QString(Language));

	StyleSet<CharStyle> TmpStyleSet;
	TmpStyleSet.create(TmpCharStyle);
	ScCore->primaryMainWindow()->doc->redefineCharStyles(TmpStyleSet, false);
	// PV - refresh the Style Manager window.
	// I thought that this can work but it doesn't:
	// ScCore->primaryMainWindow()->styleMgr()->reloadStyleView();
	// So the brute force setDoc is called...
	ScCore->primaryMainWindow()->styleMgr()->setDoc(ScCore->primaryMainWindow()->doc);

	Py_RETURN_NONE;
}

PyObject *scribus_createcustomlinestyle(PyObject * /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	PyObject *obj;

	if (!PyArg_ParseTuple(args, "esO", "utf-8", &Name, &obj))
		return nullptr;

	if (!PyList_Check(obj)) {
		PyErr_SetString(PyExc_TypeError, "'style' must be list.");
		return nullptr;
	}

	multiLine ml;
	for (int i = 0; i < PyList_Size(obj); i++) {
		PyObject *line = PyList_GetItem(obj, i);
		if (!PyDict_Check(line)) {
			PyErr_SetString(PyExc_TypeError, "elements of list must be Dictionary.");
			return nullptr;
		}
		struct SingleLine sl;
		PyObject *val;
		val = PyDict_GetItemString(line, "Color");
		if (val) {
			sl.Color = PyString_AsString(val);
		} else 
			sl.Color = ScCore->primaryMainWindow()->doc->itemToolPrefs().lineColor;;
		val = PyDict_GetItemString(line, "Dash");
		if (val) {
			sl.Dash = PyInt_AsLong(val);
		} else 
			sl.Dash = Qt::SolidLine;
		val = PyDict_GetItemString(line, "LineEnd");
		if (val) {
			sl.LineEnd = PyInt_AsLong(val);
		} else 
			sl.LineEnd = Qt::FlatCap;
		val = PyDict_GetItemString(line, "LineJoin");
		if (val) {
			sl.LineJoin = PyInt_AsLong(val);
		} else 
			sl.LineJoin = Qt::MiterJoin;
		val = PyDict_GetItemString(line, "Shade");
		if (val) {
			sl.Shade = PyInt_AsLong(val);
		} else 
			sl.Shade = ScCore->primaryMainWindow()->doc->itemToolPrefs().lineColorShade;
		val = PyDict_GetItemString(line, "Width");
		if (val) {
			sl.Width = PyFloat_AsDouble(val);
		} else 
			sl.Width = ScCore->primaryMainWindow()->doc->itemToolPrefs().lineWidth;

		val = PyDict_GetItemString(line, "Shortcut");
		if (val) {
			ml.shortcut = PyString_AsString(val);
		} else 
			ml.shortcut = "";
		ml.push_back(sl);
	}
	if (!ml.empty())
		ScCore->primaryMainWindow()->doc->MLineStyles[Name] = ml;
	Py_RETURN_NONE;
}


/*! HACK: this removes "warning: 'blah' defined but not used" compiler warnings
with header files structure untouched (docstrings are kept near declarations)
PV */
void cmdstyledocwarnings()
{
	QStringList s;
	s << scribus_createparagraphstyle__doc__ << scribus_createcharstyle__doc__;
	s << scribus_createcustomlinestyle__doc__;
}
