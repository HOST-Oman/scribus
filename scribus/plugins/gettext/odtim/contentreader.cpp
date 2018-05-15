/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
 *   Copyright (C) 2004 by Riku Leino                                      *
 *   tsoots@gmail.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QGlobalStatic>
#include <QByteArray>
#include "contentreader.h"
#include "scribusstructs.h"
#include "gtwriter.h"
#include "gtstyle.h"
#include "gtparagraphstyle.h"
#include "stylereader.h"

ContentReader* ContentReader::creader = nullptr;

extern xmlSAXHandlerPtr cSAXHandler;

ContentReader::ContentReader(QString documentName, StyleReader *s, gtWriter *w, bool textOnly)
{
	creader = this;
	docname = documentName;
	sreader = s;
	writer  = w;
	importTextOnly = textOnly;
	defaultStyle = nullptr;
	currentStyle = nullptr;
	inList = false;
	inAnnotation = false;
	inSpan = false;
	append = 0;
	listIndex = 0;
	listLevel = 0;
	currentList = "";
	currentListStyle = 0;
	inT = false;
	tName = "";
}

bool ContentReader::startElement(const QString&, const QString&, const QString &name, const QXmlAttributes &attrs) 
{
	if ((name == "text:p") || (name == "text:h"))
	{
		++append;
		QString name = "";
		for (int i = 0; i < attrs.count(); ++i)
		{
			if (attrs.localName(i) == "text:style-name")
			{
				name = attrs.value(i);
				styleNames.push_back(attrs.value(i));
			}
		}
		if (!inList)
		{
			pstyle = sreader->getStyle(name);
			currentStyle = pstyle;
		}
		else
		{
			gtStyle *tmp = sreader->getStyle(getName());
			if ((tmp->getName()).indexOf("default-style") != -1)
				getStyle();
			else
				currentStyle = tmp;
		}
	}
	else if (name == "text:span")
	{
		inSpan = true;
		QString styleName = "";
		for (int i = 0; i < attrs.count(); ++i)
		{
			if (attrs.localName(i) == "text:style-name")
			{
				currentStyle = sreader->getStyle(attrs.value(i));
				styleName = attrs.value(i);
				styleNames.push_back(styleName);
			}
		}
		gtStyle *tmp = sreader->getStyle(getName());
		if ((tmp->getName()).indexOf("default-style") != -1)
			getStyle();
		else
			currentStyle = tmp;
	}
	else if (name == "text:list")
	{
		inList = true;
		++listLevel;
		if (static_cast<int>(listIndex2.size()) < listLevel)
			listIndex2.push_back(0);
		for (int i = 0; i < attrs.count(); ++i)
		{
			if (attrs.localName(i) == "text:style-name")
			{
				currentList = attrs.value(i);
			}
		}
		currentStyle = sreader->getStyle(QString(currentList + "_%1").arg(listLevel));
		currentListStyle = sreader->getList(currentList);
		if (currentListStyle)
			currentListStyle->setLevel(listLevel);
		styleNames.clear();
		styleNames.push_back(QString(currentList + "_%1").arg(listLevel));
	}
	else if (name == "text:list-item")
	{
		if (currentListStyle)
		{
			currentListStyle->advance();
			//write(currentListStyle->bullet());
		}
	}
	else if (name == "office:annotation")
		inAnnotation = true;
	else if (name == "text:note")
		writer->inNote = true;
	else if (name == "text:note-body")
		writer->inNoteBody = true;
	else if (name == "style:style")
	{
		QString sname = "";
		bool isTextStyle = false;
		for (int i = 0; i < attrs.count(); ++i)
		{
			if (attrs.localName(i) == "style:name")
				sname = attrs.value(i);
			else if ((attrs.localName(i) == "style:family") && (attrs.value(i) == "text"))
				isTextStyle = true;
		}
		if (isTextStyle)
		{
			tName = sname;
			inT = true;
		}
	}
	else if ((name == "style:paragraph-properties" ||
	          name == "style:text-properties" ||
	          name == "style:list-level-properties") && (inT))
	{
		Properties p;
		for (int i = 0; i < attrs.count(); ++i)
		{
			std::pair<QString, QString> pair(attrs.localName(i), attrs.value(i));
			p.push_back(pair);
		}
		tmap[tName] = p;
	}
	else if (name == "text:s")
	{
		int count = 1;
		for (int i = 0; i < attrs.count(); ++i)
		{
			if (attrs.localName(i) == "text:c")
			{
				bool ok = false;
				int tmpcount = (attrs.value(i)).toInt(&ok);
				if (ok)
					count = tmpcount;
			}
		}
		for (int i = 0; i < count; ++i)
			write(" ");
	}
	return true;
}

bool ContentReader::characters(const QString &ch) 
{
	QString tmp = ch;
	tmp = tmp.remove("\n");
	tmp = tmp.remove(""); // Remove all OO.o hyphenation chars
	// Unneeded as scribus now also use standard unicode non-breakable space
	// tmp = tmp.replace(QChar(160), SpecialChars::NBSPACE); // replace OO.o nbsp with Scribus nbsp
	if (append > 0)
		write(tmp);
	return true;
}

bool ContentReader::endElement(const QString&, const QString&, const QString &name)
{
	if ((name == "text:p") || (name == "text:h"))
	{
// 		qDebug("TPTH");
		write("\n");
		--append;
		if (inList || inAnnotation)
		{
			if(static_cast<int>(styleNames.size()) > 0)
				styleNames.pop_back();
		}
		else
			styleNames.clear();
	}
	else if (name == "text:span")
	{
// 		qDebug("TS");
		inSpan = false;
		currentStyle = pstyle;
		if (styleNames.size() != 0)
			styleNames.pop_back();	
		currentStyle = sreader->getStyle(getName());
	}
	else if (name == "office:annotation")
	{
		inAnnotation = false;
	}
	else if (name == "text:note")
	{
// 		qDebug("TN");
		writer->inNote = false;
	}
	else if (name == "text:note-body")
	{
// 		qDebug("TNB");
		write(SpecialChars::OBJECT);
		writer->inNoteBody = false;
	}
	else if (name == "text:line-break")
	{
// 		qDebug("TLB");
		write(SpecialChars::LINEBREAK);
	}
	else if (name == "text:tab")
	{
// 		qDebug("TT");
		write("\t");
	}
	else if (name == "text:list")
	{
// 		qDebug("TL");
		--listLevel;
		styleNames.clear();
		if (listLevel == 0)
		{
			inList = false;
			listIndex2.clear();
			currentListStyle = 0;
		}
		else
		{
			currentStyle = sreader->getStyle(QString(currentList + "_%1").arg(listLevel));
			styleNames.push_back(QString(currentList + "_%1").arg(listLevel));
			if (currentListStyle)
				currentListStyle->resetLevel();
			currentListStyle = sreader->getList(currentList);
			if (currentListStyle)
				currentListStyle->setLevel(listLevel);
		}
	}
	else if ((name == "style:style") && (inT))
	{
// 		qDebug("SS");
		inT = false;
		tName = "";
	}
	return true;
}

void ContentReader::write(const QString& text)
{
	if (!inAnnotation)
	{
		if (importTextOnly)
			writer->appendUnstyled(text);
		else if (inSpan)
			writer->append(text, currentStyle, false);
		else
			writer->append(text, currentStyle);
	}
	lastStyle = currentStyle;
}

void ContentReader::parse(QString fileName)
{
	sreader->parse(fileName);
#if defined(_WIN32)
	QString fname = QDir::toNativeSeparators(fileName);
	QByteArray fn = (QSysInfo::WindowsVersion & QSysInfo::WV_NT_based) ? fname.toUtf8() : fname.toLocal8Bit();
#else
	QByteArray fn(fileName.toLocal8Bit());
#endif
	xmlSAXParseFile(cSAXHandler, fn.data(), 1);
}

xmlSAXHandler cSAXHandlerStruct = {
    nullptr, // internalSubset,
    nullptr, // isStandalone,
    nullptr, // hasInternalSubset,
    nullptr, // hasExternalSubset,
    nullptr, // resolveEntity,
    nullptr, // getEntity,
    nullptr, // entityDecl,
    nullptr, // notationDecl,
    nullptr, // attributeDecl,
    nullptr, // elementDecl,
    nullptr, // unparsedEntityDecl,
    nullptr, // setDocumentLocator,
    nullptr, // startDocument,
    nullptr, // endDocument,
	ContentReader::startElement,
	ContentReader::endElement,
    nullptr, // reference,
	ContentReader::characters,
    nullptr, // ignorableWhitespace,
    nullptr, // processingInstruction,
    nullptr, // comment,
    nullptr, // warning,
    nullptr, // error,
    nullptr, // fatalError,
    nullptr, // getParameterEntity,
    nullptr, // cdata,
    nullptr,
	1
#ifdef HAVE_XML26
	,
    nullptr,
    nullptr,
    nullptr,
    nullptr
#endif
};

xmlSAXHandlerPtr cSAXHandler = &cSAXHandlerStruct;

void ContentReader::startElement(void*, const xmlChar *fullname, const xmlChar ** atts)
{
	QString name(QString((const char*) fullname).toLower());
	QXmlAttributes attrs;
	if (atts)
	{
		for(const xmlChar** cur = atts; cur && *cur; cur += 2)
			attrs.append(QString((char*)*cur), nullptr, QString((char*)*cur), QString((char*)*(cur + 1)));
	}
	creader->startElement(nullptr, nullptr, name, attrs);
}

void ContentReader::characters(void*, const xmlChar *ch, int len)
{
	QString chars = QString::fromUtf8((const char*) ch, len);
	creader->characters(chars);
}

void ContentReader::endElement(void*, const xmlChar *name)
{
	QString nname(QString((const char*) name).toLower());
	creader->endElement(nullptr, nullptr, nname);
}

QString ContentReader::getName()
{
	QString s = "";
	for (uint i = 0; i < styleNames.size(); ++i)
		s += styleNames[i];
	return s;
}

void ContentReader::getStyle()
{
	gtStyle *style = nullptr, *tmp = nullptr;
	if (styleNames.size() == 0)
		style = sreader->getStyle("default-style");
	else
		style = sreader->getStyle(styleNames[0]);
	assert (style != nullptr);
	gtParagraphStyle* par = dynamic_cast<gtParagraphStyle*>(style);
	if (par)
		tmp = new gtParagraphStyle(*par);
	else
		tmp = new gtStyle(*style);
	for (uint i = 1; i < styleNames.size(); ++i)
	{
		Properties& p = tmap[styleNames[i]];
		for (uint j = 0; j < p.size(); ++j)
			sreader->updateStyle(tmp, sreader->getStyle(styleNames[i - 1]), p[j].first, p[j].second);
	}
	currentStyle = tmp;
	sreader->setStyle(getName(), tmp);
}

ContentReader::~ContentReader()
{
	creader = nullptr;
	delete defaultStyle;
}
