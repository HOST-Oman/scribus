/*
 *  saxXML.cpp
 *  
 *
 *  Created by Andreas Vox on 21.09.06.
 *  Copyright 2006 under GPL2. All rights reserved.
 *
 */

#include <fstream>
#include <cassert>
#include "saxXML.h"

using namespace std;

SaxXML::SaxXML(std::ostream& file, bool pretty) :
	m_stream(file),
	m_pretty(pretty)
{}

SaxXML::SaxXML(const char* filename, bool pretty) :
	m_file(filename, ios::out | ios::binary),
	m_stream(m_file),
	m_pretty(pretty)
{}

SaxXML::~SaxXML() { m_stream.flush(); m_file.close(); }

void SaxXML::beginDoc()
{
	m_stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
}


void SaxXML::endDoc()
{
	m_stream << "\n";
	m_stream.flush();
	m_file.close();
}


void SaxXML::finalizePendingEmptyTag()
{
	if (!m_pendingEmptyTag)
		return;
	if (m_pretty && m_manyAttributes)
	{
		m_stream << "\n";
		for (int i = 0; i < m_indentLevel * 4; ++i)
			m_stream << " ";
		m_stream << ">";
	}
	else
		m_stream << " >";
	m_pendingEmptyTag = false;
}

void SaxXML::begin(const Xml_string& tag, Xml_attr attr)
{
	finalizePendingEmptyTag();
	assert(!tag.isNull());
	if (m_pretty)
	{
		// indent tag
		m_stream << "\n";
		for (int i = 0; i  < m_indentLevel * 4; ++i)
			m_stream << " ";
	}
	m_stream << "<" << fromXMLString(tag);
	m_manyAttributes = false;
	uint i = 0;
	for (auto it = attr.begin(); it != attr.end(); ++it)
	{
		// newline and indent every 4 attributes
		if (i > 0 && (i % 4) == 0 && m_pretty)
		{
			m_stream << "\n";
			for (int j = 0; j  < m_indentLevel * 4 + 1 + tag.length(); ++j)
				m_stream << " ";
			m_manyAttributes = true;
		}
		++i;
		if (Xml_data(it).isNull())
			m_stream << " " << fromXMLString(Xml_key(it)) << "=\"\"";
		else
		{
			QString txt(Xml_data(it));
			txt.replace("&", "&amp;");
			txt.replace("\"", "&quot;");
			txt.replace("<", "&lt;");
			txt.replace(">", "&gt;"); 
			m_stream << " " << fromXMLString(Xml_key(it)) << "=\"" << fromXMLString(txt) << "\"";
		}
	}
	m_pendingEmptyTag = true;
	++m_indentLevel;
}


void SaxXML::end(const Xml_string& tag)
{
	--m_indentLevel;
	if (m_pendingEmptyTag)
	{
		if (m_pretty && m_manyAttributes)
		{
			m_stream << "\n";
			for (int i = 0; i  < m_indentLevel * 4; ++i)
				m_stream << " ";
		}
		m_stream << " />"; 
		m_pendingEmptyTag = false;
	}
	else
	{
		if (m_pretty)
		{
			m_stream << "\n";
			for (int i = 0; i  < m_indentLevel * 4; ++i)
				m_stream << " ";
		}
		m_stream << "</" << fromXMLString(tag) << ">";
	}
}


void SaxXML::chars(const Xml_string& text)
{
	finalizePendingEmptyTag();
	QString txt(text);
	txt.replace("&", "&amp;");
	txt.replace("<", "&lt;");
	txt.replace(">", "&gt;");
	m_stream << fromXMLString(txt);
}

