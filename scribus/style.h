/*
 For general Scribus (>=1.3.2) copyright and licensing information please refer
 to the COPYING file provided with the program. Following this notice may exist
 a copyright and/or license notice that predates the release of Scribus 1.3.2
 for which a new license (GPL+exception) is in place.
 */
/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/


#ifndef STYLE_H
#define STYLE_H

#include <cassert>
#include <QString>
#include "scfonts.h"
#include "scribusapi.h"
#include "styles/stylecontext.h"
#include "desaxe/saxio.h"


/**
 *  This is the base class for all style-like objects: CharStyles, 
 *  ParagraphStyles, LineStyles, FrameStyles, CellStyles,
 *  FlowStyles,...
 *  It provides a name and an inheritance mechanism which uses a Stylecontext.
 *  Before any attribute is queried, you have to call validate(), which checks
 *  the stored m_contextversion against the StyleContext's version and updates all
 *  attributes if they are different.
 */
class SCRIBUS_API BaseStyle : public SaxIO {
protected:
	bool m_isDefaultStyle;
	QString m_name;
	const StyleContext* m_context;
	int m_contextversion;
	QString m_parent;
	QString m_shortcut;
public:
//	static const short NOVALUE = -16000;

	BaseStyle(): m_isDefaultStyle(false), m_name(""), m_context(nullptr), m_contextversion(-1), m_parent(""), m_shortcut() {}

	BaseStyle(StyleContext* b, QString n): m_isDefaultStyle(false), m_name(n), m_context(b), m_contextversion(-1), m_parent(""), m_shortcut() {}
	
	BaseStyle& operator=(const BaseStyle& o)
	{ //assert(typeinfo() == o.typeinfo()); 
		m_isDefaultStyle = o.m_isDefaultStyle;
		m_name = o.m_name;
//		m_context = o.m_context; 
		m_contextversion = -1; 
		m_parent = o.m_parent;
		m_shortcut = o.m_shortcut;
		return *this;
	}
	
	BaseStyle(const BaseStyle& o) : SaxIO(), m_isDefaultStyle(o.m_isDefaultStyle),m_name(o.m_name),
		m_context(o.m_context), m_contextversion(o.m_contextversion), m_parent(o.m_parent), m_shortcut(o.m_shortcut) {} 
	
	virtual ~BaseStyle()                 {}

	
	// this is an abstract class, so:
	// static const Xml_string saxxDefaultElem; 
	template<class SUBSTYLE>
		static void  desaxeRules(const Xml_string& prefixPattern, desaxe::Digester& ruleset, const Xml_string& elemtag);
	
	void saxxAttributes(Xml_attr& attr) const;
	//virtual void saxx(SaxHandler& handler, const Xml_string& elemtag) const;
	//virtual void saxx(SaxHandler& handler)                     const { saxx(handler, saxxDefaultElem); }
	
	void setDefaultStyle(bool ids);
	bool isDefaultStyle() const      { return m_isDefaultStyle; }
	
	QString name() const             { return m_name; }
	void setName(const QString& n)   { m_name = n.isEmpty() ? "" : n; }
	bool hasName() const             { return ! m_name.isEmpty(); }

	virtual QString displayName() const = 0;/*{ 	
		if ( hasName() || !hasParent() || !m_context)
			return name();
		//	else if ( inheritsAll() )
		//		return parent()->displayName();
		else 
			return parentStyle()->displayName();
	}*/
	
	QString parent() const           { return m_parent; }
	void setParent(const QString& p);
	bool hasParent() const           { return ! m_parent.isEmpty(); }
	const BaseStyle* parentStyle() const;

	bool canInherit(const QString& parentName) const;
	
	static const QString INHERIT_PARENT;
	
	virtual void setContext(const StyleContext* context);
	const StyleContext* context() const        { return m_context; }
	
	/**
		sets a new StyleContext if b is not nullptr and then uses the StyleContext
		to set all inherited attributes to their valid value.
	 */
	virtual void update(const StyleContext* b = nullptr);
	
	/**
		Checks if this BaseStyle needs an update
	 */
	inline void validate() const {
		if (m_context && m_contextversion != m_context->version()) {
			const_cast<BaseStyle*>(this)->update(m_context);
			assert( m_context->checkConsistency() );
		}
	}

	QString shortcut() const { return m_shortcut; }
	void setShortcut(const QString &shortcut) { m_shortcut = shortcut; }

	/**
		returns true if both Styles are of the same type, inherit the same attributes,
	    have the same parent, and agree on all attributes which are not inherited.
	    The StyleContext, the name and any inherited attrinutes may be different.
	 */
	virtual bool equiv(const BaseStyle& other) const = 0;
	/**
		returns true if both Styles are equivalent and have the same name.
	    Since the context is not tested, this does *not* ensure they will return
	    the same values for all attributes.
	 */
	virtual bool operator==(const BaseStyle& other) const { return name() == other.name() && equiv(other); }
	virtual bool operator!=(const BaseStyle& other) const { return ! ( (*this) == other ); }

	/**
		resets all attributes to their defaults and makes them inherited.
	    name and parent are not affected.
	 */
	virtual void erase() = 0;
	/**
		if other has a parent, replace this parent with the other ones
	 */
	void applyStyle(const BaseStyle& other) {
		if (other.hasParent())
			setParent( other.parent() == INHERIT_PARENT? "" :other.parent());
		m_contextversion = -1;
	}
	/** 
		if other has the same parent, remove this parent 
	 */
	void eraseStyle(const BaseStyle& other) {
		if (other.parent() == parent())
			setParent("");
		m_contextversion = -1;
	}
};

#endif
