/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include "cmdutil.h"
#include "prefsmanager.h"
#include "resourcecollection.h"
#include "scpage.h"
#include "scribuscore.h"
#include "scribusdoc.h"
#include "scribusview.h"
#include "selection.h"
#include "tableborder.h"
#include "units.h"

#include <QMap>

ScribusMainWindow* Carrier;
ScribusDoc* doc;

/// Convert a value in points to a value in the current document units
double PointToValue(double Val)
{
	return pts2value(Val, ScCore->primaryMainWindow()->doc->unitIndex());
}

/// Convert a value in the current document units to a value in points
double ValueToPoint(double Val)
{
	return value2pts(Val, ScCore->primaryMainWindow()->doc->unitIndex());
}

/// Convert an X co-ordinate part in page units to a document co-ordinate
/// in system units.
double pageUnitXToDocX(double pageUnitX)
{
	return ValueToPoint(pageUnitX) + ScCore->primaryMainWindow()->doc->currentPage()->xOffset();
}

// Convert doc units to page units
double docUnitXToPageX(double pageUnitX)
{
	return PointToValue(pageUnitX - ScCore->primaryMainWindow()->doc->currentPage()->xOffset());
}

/// Convert a Y co-ordinate part in page units to a document co-ordinate
/// in system units. The document co-ordinates have their origin somewere
/// up and left of the first page, where page co-ordinates have their
/// origin on the top left of the current page.
double pageUnitYToDocY(double pageUnitY)
{
	return ValueToPoint(pageUnitY) + ScCore->primaryMainWindow()->doc->currentPage()->yOffset();
}

double docUnitYToPageY(double pageUnitY)
{
	return PointToValue(pageUnitY - ScCore->primaryMainWindow()->doc->currentPage()->yOffset());
}

PageItem *GetItem(QString Name)
{
	if (!Name.isEmpty())
	{
		for (int a = 0; a < ScCore->primaryMainWindow()->doc->Items->count(); a++)
		{
			if (ScCore->primaryMainWindow()->doc->Items->at(a)->itemName() == Name)
				return ScCore->primaryMainWindow()->doc->Items->at(a);
		}
	}
	else
	{
		if (ScCore->primaryMainWindow()->doc->m_Selection->count() != 0)
			return ScCore->primaryMainWindow()->doc->m_Selection->itemAt(0);
	}
	return nullptr;
}

void ReplaceColor(QString col, QString rep)
{
	QMap<QString, QString> colorMap;
	colorMap.insert(col, rep);

	ResourceCollection colorRsc;
	colorRsc.mapColor(col, rep);

	if (!ScCore->primaryMainWindow()->HaveDoc)
		return;
	ScribusDoc* doc = ScCore->primaryMainWindow()->doc;

	// Update tools colors
	PrefsManager::replaceToolColors(doc->itemToolPrefs(), colorRsc.colors());
	// Update objects and styles colors
	doc->replaceNamedResources(colorRsc);
	// Temporary code until LineStyle is effectively used
	doc->replaceLineStyleColors(colorMap);
}

/* 04/07/10 returns selection if is not name specified  pv  */
PageItem* GetUniqueItem(QString name)
{
	if (name.length()==0)
		if (ScCore->primaryMainWindow()->doc->m_Selection->count() != 0)
			return ScCore->primaryMainWindow()->doc->m_Selection->itemAt(0);
		else
		{
			PyErr_SetString(NoValidObjectError, QString("Cannot use empty string for object name when there is no selection").toLocal8Bit().constData());
			return nullptr;
		}
	else
		return getPageItemByName(name);
}

PageItem* getPageItemByName(QString name)
{
	if (name.length() == 0)
	{
		PyErr_SetString(PyExc_ValueError, QString("Cannot accept empty name for pageitem").toLocal8Bit().constData());
		return nullptr;
	}
	for (int j = 0; j<ScCore->primaryMainWindow()->doc->Items->count(); j++)
	{
		if (name==ScCore->primaryMainWindow()->doc->Items->at(j)->itemName())
			return ScCore->primaryMainWindow()->doc->Items->at(j);
	} // for items
	PyErr_SetString(NoValidObjectError, QString("Object not found").toLocal8Bit().constData());
	return nullptr;
}


/*!
 * Checks to see if a pageItem named 'name' exists and return true
 * if it does exist. Returns false if there is no such object, or
 * if the empty string ("") is passed.
 */
bool ItemExists(QString name)
{
	if (name.length() == 0)
		return false;
	for (int j = 0; j<ScCore->primaryMainWindow()->doc->Items->count(); j++)
	{
		if (name==ScCore->primaryMainWindow()->doc->Items->at(j)->itemName())
			return true;
	} // for items
	return false;
}

/*!
 * Checks to see if there is a document open.
 * If there is an open document, returns true.
 * If there is no open document, sets a Python
 * exception and returns false.
 * 2004-10-27 Craig Ringer
 */
bool checkHaveDocument()
{
	if (ScCore->primaryMainWindow()->HaveDoc)
		return true;
	// Caller is required to check for false return from this function
	// and return nullptr.
	PyErr_SetString(NoDocOpenError, QString("Command does not make sense without an open document").toLocal8Bit().constData());
	return false;
}

QStringList getSelectedItemsByName()
{
	/*
	QStringList names;
	QPtrListIterator<PageItem> it(ScCore->primaryMainWindow()->view->SelItem);
	for ( ; it.current() != 0 ; ++it)
		names.append(it.current()->itemName());
	return names;
	*/
	return ScCore->primaryMainWindow()->doc->m_Selection->getSelectedItemsByName();
}

bool setSelectedItemsByName(QStringList& itemNames)
{
	ScCore->primaryMainWindow()->view->Deselect();
	// For each named item
	for (QStringList::Iterator it = itemNames.begin() ; it != itemNames.end() ; it++)
	{
		// Search for the named item
		PageItem* item = 0;
		for (int j = 0; j < ScCore->primaryMainWindow()->doc->Items->count(); j++)
			if (*it == ScCore->primaryMainWindow()->doc->Items->at(j)->itemName())
				item = ScCore->primaryMainWindow()->doc->Items->at(j);
		if (!item)
			return false;
		// and select it
		ScCore->primaryMainWindow()->view->SelectItem(item);
	}
	return true;
}

TableBorder parseBorder(PyObject* borderLines, bool* ok)
{
	TableBorder border;

	if (!PyList_Check(borderLines))
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Expected a list of border lines", "python error").toLocal8Bit().constData());
		*ok = false;
		return border;
	}

	// Get the sequence of border lines.
	PyObject* borderLinesList = PySequence_List(borderLines);
	if (borderLinesList == nullptr)
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Expected a list of border lines", "python error").toLocal8Bit().constData());
		*ok = false;
		return border;
	}

	// Parse each tuple decribing a border line and append it to the border.
	int nBorderLines = PyList_Size(borderLinesList);
	for (int i = 0; i < nBorderLines; i++) {
		double width = 0.0;
		double shade = 100.0;
		int style;
		char* color;
		PyObject* props = PyList_GET_ITEM(borderLinesList, i);
		if (!PyArg_ParseTuple(props, "dies|d", &width, &style, "utf-8", &color, &shade))
		{
			PyErr_SetString(PyExc_ValueError, QObject::tr("Border lines are specified as (width,style,color,shade) tuples", "python error").toLocal8Bit().constData());
			*ok = false;
			return border;
		}
		if (width <= 0.0)
		{
			PyErr_SetString(PyExc_ValueError, QObject::tr("Border line width must be > 0.0", "python error").toLocal8Bit().constData());
			*ok = false;
			return border;
		}
		border.addBorderLine(TableBorderLine(width, static_cast<Qt::PenStyle>(style), QString::fromUtf8(color), shade));
	}
	Py_DECREF(borderLinesList);

	*ok = true;
	return border;
}

