/*
 For general Scribus (>=1.3.2) copyright and licensing information please refer
 to the COPYING file provided with the program. Following this notice may exist
 a copyright and/or license notice that predates the release of Scribus 1.3.2
 for which a new license (GPL+exception) is in place.
 */


#include <QObject>
#include <QDebug>

#include "commonstrings.h"
#include "sctextstruct.h"
#include "scfonts.h"
#include "resourcecollection.h"

#include "styles/style.h"
#include "charstyle.h"
#include "desaxe/saxiohelper.h"
#include "desaxe/simple_actions.h"
#include "prefsmanager.h"
#include "util_math.h"

StyleFlag& StyleFlag::operator&= (const StyleFlag& right)
{
	int result = static_cast<int>(value) & static_cast<int>(right.value);
	value = static_cast<StyleFlagValue>(result);
	return *this;
}

StyleFlag& StyleFlag::operator|= (const StyleFlag& right)
{
	int result = static_cast<int>(value) | static_cast<int>(right.value);
	value = static_cast<StyleFlagValue>(result);
	return *this;
}

StyleFlag StyleFlag::operator& (const StyleFlag& right)
{
	int val = static_cast<int>(value) & static_cast<int>(right.value);
	StyleFlag result(static_cast<StyleFlagValue>(val)); 
	return result;
}

StyleFlag StyleFlag::operator& (int right)
{
	int val = static_cast<int>(value) & right;
	StyleFlag result(static_cast<StyleFlagValue>(val)); 
	return result;
}

StyleFlag StyleFlag::operator| (const StyleFlag& right)
{
	int val = static_cast<int>(value) | static_cast<int>(right.value);
	StyleFlag result(static_cast<StyleFlagValue>(val)); 
	return result;
}

StyleFlag StyleFlag::operator^ (const StyleFlag& right)
{
	int val = static_cast<int>(value) ^ static_cast<int>(right.value);
	StyleFlag result(static_cast<StyleFlagValue>(val)); 
	return result;
}

StyleFlag StyleFlag::operator^ (int right)
{
	int val = static_cast<int>(value) ^ right;
	StyleFlag result(static_cast<StyleFlagValue>(val)); 
	return result;
}

StyleFlag StyleFlag::operator~ ()
{
	int val = ~ static_cast<int>(value);
	StyleFlag result(static_cast<StyleFlagValue>(val)); 
	return result;
}

bool StyleFlag::equivForShaping(const StyleFlag& right) const
{
	int result = static_cast<int>( (value ^ right.value) & ScStyle_RunBreakingStyles);
	return (result == 0);
}

bool StyleFlag::operator== (const StyleFlag& right) const
{
	int result = static_cast<int>( (value ^ right.value) & ScStyle_UserStyles);
	return (result == 0);
}

bool StyleFlag::operator== (const StyleFlagValue right) const
{
	int result = static_cast<int>( (value ^ right) & ScStyle_UserStyles);
	return (result == 0);
}

bool StyleFlag::operator== (int right) const
{
	int result = static_cast<int>( (value ^ right) & ScStyle_UserStyles);
	return (result == 0);
}

bool StyleFlag::operator!= (const StyleFlag& right) const
{
	return !(*this == right);
}

bool StyleFlag::operator!= (const StyleFlagValue right) const
{
	return !(*this == right);
}


void CharStyle::applyCharStyle(const CharStyle & other)
{
	if (other.hasParent() && (other.parent() != BaseStyle::INHERIT_PARENT))
	{
		setStyle(other);
		return;
	}
	BaseStyle::applyStyle(other);
#define ATTRDEF(attr_TYPE, attr_GETTER, attr_NAME, attr_DEFAULT, attr_BREAKSHAPING) \
	if (! other.inh_##attr_NAME) \
		set##attr_NAME(other.m_##attr_NAME);
#include "charstyle.attrdefs.cxx"
#undef ATTRDEF
	updateFeatures();
}

void CharStyle::eraseCharStyle(const CharStyle & other)
{
	other.validate();
	BaseStyle::eraseStyle(other);
#define ATTRDEF(attr_TYPE, attr_GETTER, attr_NAME, attr_DEFAULT, attr_BREAKSHAPING) \
	if (!inh_##attr_NAME && m_##attr_NAME == other.m_##attr_NAME) \
		reset##attr_NAME();
#include "charstyle.attrdefs.cxx"
#undef ATTRDEF
	updateFeatures();
}

void CharStyle::eraseDirectFormatting()
{
#define ATTRDEF(attr_TYPE, attr_GETTER, attr_NAME, attr_DEFAULT, attr_BREAKSHAPING) \
	if (!inh_##attr_NAME) \
		reset##attr_NAME();
#include "charstyle.attrdefs.cxx"
#undef ATTRDEF
	updateFeatures();
}

bool CharStyle::equiv(const BaseStyle & other) const
{
	other.validate();
	const CharStyle * oth = reinterpret_cast<const CharStyle*> ( & other );
	return  oth &&
		parent() == oth->parent() 
#define ATTRDEF(attr_TYPE, attr_GETTER, attr_NAME, attr_DEFAULT, attr_BREAKSHAPING) \
		&& (inh_##attr_NAME == oth->inh_##attr_NAME) \
		&& (inh_##attr_NAME || isequiv(m_##attr_NAME, oth->m_##attr_NAME))
#include "charstyle.attrdefs.cxx"
#undef ATTRDEF
	    ;
}

bool CharStyle::equivForShaping(const CharStyle& other) const
{
	other.validate();

#define ATTRDEF(attr_TYPE, attr_GETTER, attr_NAME, attr_DEFAULT, attr_BREAKSHAPING) \
	if (attr_BREAKSHAPING && !isequiv(m_##attr_NAME, other.m_##attr_NAME)) \
		return false;
#include "charstyle.attrdefs.cxx"
#undef ATTRDEF
	if (!m_Effects.equivForShaping(other.m_Effects))
		return false;
	return true;
}

QString CharStyle::displayName() const
{
	if (isDefaultStyle())
		return CommonStrings::trDefaultCharacterStyle;
	if (hasName() || !hasParent() || ! m_context)
		return name();
//	else if ( inheritsAll() )
//		return parent()->displayName();
	return parentStyle()->displayName() + "+";
}

QString CharStyle::asString() const
{
	//This function creates the string used for text properties changes in Action History
	QString result;
	if (!inh_Font)
		result += (QObject::tr("font %1").arg(font().scName()) + " ");
	if (!inh_FontSize)
		result += (QObject::tr("size %1").arg(fontSize()) + " ");
	if (!inh_FontFeatures)
		result += (QObject::tr("+fontfeatures %1").arg(fontFeatures()) + " ");
	if (!inh_Features)
		result += (QObject::tr("+style") + " ");
	if (!inh_StrokeColor || !inh_StrokeShade || !inh_FillColor || !inh_FillShade)
		result += (QObject::tr("+color") + " ");
	if (!inh_UnderlineWidth || !inh_UnderlineOffset)
		result += ((underlineWidth() > 0 ? QObject::tr("+underline") : QObject::tr("-underline")) + " ");
	if (!inh_StrikethruWidth || !inh_StrikethruOffset )
		result += ((strikethruWidth() > 0 ? QObject::tr("+strikeout") : QObject::tr("-strikeout")) + " ");
	if (!inh_ShadowXOffset || !inh_ShadowYOffset )
		result += ((shadowXOffset() != 0 || shadowYOffset() != 0 ? QObject::tr("+shadow") : QObject::tr("-shadow")) + " ");
	if (!inh_OutlineWidth)
		result += ((outlineWidth() > 0 ? QObject::tr("+outline") : QObject::tr("-outline")) + " ");
	if (!inh_Tracking)
		result += ((tracking() > 0 ? QObject::tr("+tracking %1").arg(tracking()) : QObject::tr("-tracking")) + " ");
	if (!inh_BaselineOffset)
		result += (QObject::tr("+baseline %1").arg(baselineOffset()) + " ");
	if (!inh_ScaleH || !inh_ScaleV)
		result += (QObject::tr("+stretch") + " ");
	if (hasParent())
		result += QObject::tr("parent= %1").arg(parent());
	return result.trimmed();
}


void CharStyle::update(const StyleContext* context)
{
	BaseStyle::update(context);
	const CharStyle * oth = dynamic_cast<const CharStyle*> ( parentStyle() );
	if (oth) {
#define ATTRDEF(attr_TYPE, attr_GETTER, attr_NAME, attr_DEFAULT, attr_BREAKSHAPING) \
		if (inh_##attr_NAME) \
			m_##attr_NAME = oth->attr_GETTER();
#include "charstyle.attrdefs.cxx"
#undef ATTRDEF
	}
	updateFeatures();
}


const QString CharStyle::INHERIT = "inherit";
const QString CharStyle::BOLD = "bold";
const QString CharStyle::ITALIC = "italic";
const QString CharStyle::UNDERLINE = "underline";
const QString CharStyle::UNDERLINEWORDS = "underlinewords";
const QString CharStyle::STRIKETHROUGH = "strike";
const QString CharStyle::SUPERSCRIPT = "superscript";
const QString CharStyle::SUBSCRIPT = "subscript";
const QString CharStyle::OUTLINE = "outline";
const QString CharStyle::SHADOWED = "shadowed";
const QString CharStyle::ALLCAPS = "allcaps";
const QString CharStyle::SMALLCAPS = "smallcaps";
// This is for loading legacy docs only. Scribus 1.3.4 should write soft hyphens in another way 
static const QString SHYPHEN = "shyphen";


QStringList StyleFlag::featureList() const
{
	QStringList result(CharStyle::INHERIT);
	if (*this & ScStyle_Underline)
		result << CharStyle::UNDERLINE;
	if (*this & ScStyle_UnderlineWords)
		result << CharStyle::UNDERLINEWORDS;
	if (*this & ScStyle_Strikethrough)
		result << CharStyle::STRIKETHROUGH;
	if (*this & ScStyle_Superscript)
		result << CharStyle::SUPERSCRIPT;
	if (*this & ScStyle_Subscript)
		result << CharStyle::SUBSCRIPT;
	if (*this & ScStyle_Outline)
		result << CharStyle::OUTLINE;
	if (*this & ScStyle_Shadowed)
		result << CharStyle::SHADOWED;
	if (*this & ScStyle_AllCaps)
		result << CharStyle::ALLCAPS;
	if (*this & ScStyle_SmallCaps)
		result << CharStyle::SMALLCAPS;
	if (*this & ScStyle_HyphenationPossible)
		result << SHYPHEN;
	return result;
}


void CharStyle::updateFeatures()
{
	m_Effects &= ~ScStyle_UserStyles;
	runFeatures(m_Features, reinterpret_cast<const CharStyle*>(parentStyle()));
/* need to access global fontlist :-/
	if (!font().name().endsWith(fontVariant()))
	{
		m_font = ScFonts.instance().findFont(font().family() + fontVariant());
	}
 */
}


void CharStyle::runFeatures(const QStringList& featureList, const CharStyle* parent)
{
	QStringList::ConstIterator it;
	QStringList::ConstIterator itEnd = featureList.end();
	for (it = featureList.begin(); it != itEnd; ++it)
	{
		QString feature = it->trimmed();
		if (feature == INHERIT)
		{
			if (parent)
				runFeatures(parent->features(), reinterpret_cast<const CharStyle*>(parent->parentStyle()));
			continue;
		}
		int set = feature.startsWith('-') ? 0 : 1;
		int invert = 1 - set;
		if (invert)
			feature = feature.mid(1);

//		if (feature == BOLD)
//		{
//			// (de)select bolder font
//		}
//		else if (feature == ITALIC)
//		{
//			// (de)select italic font
//		}
//		else
		if (feature == UNDERLINE)
		{
			m_Effects &= ~(invert * ScStyle_Underline);
			m_Effects |= set * ScStyle_Underline;
		}
		else if (feature == UNDERLINEWORDS)
		{
			m_Effects &= ~(invert * ScStyle_UnderlineWords);
			m_Effects |= set * ScStyle_UnderlineWords;
		}
		else if (feature == STRIKETHROUGH)
		{
			m_Effects &= ~(invert * ScStyle_Strikethrough);
			m_Effects |= set * ScStyle_Strikethrough;
		}
		else if (feature == SUPERSCRIPT)
		{
			m_Effects &= ~(invert * ScStyle_Superscript);
			m_Effects |= set * ScStyle_Superscript;
		}
		else if (feature == SUBSCRIPT)
		{
			m_Effects &= ~(invert * ScStyle_Subscript);
			m_Effects |= set * ScStyle_Subscript;
		}
		else if (feature == OUTLINE)
		{
			m_Effects &= ~(invert * ScStyle_Outline);
			m_Effects |= set * ScStyle_Outline;
		}
		else if (feature == SHADOWED)
		{
			m_Effects &= ~(invert * ScStyle_Shadowed);
			m_Effects |= set * ScStyle_Shadowed;
		}
		else if (feature == ALLCAPS)
		{
			m_Effects &= ~(invert * ScStyle_AllCaps);
			m_Effects |= set * ScStyle_AllCaps;
		}
		else if (feature == SMALLCAPS)
		{
			m_Effects &= ~(invert * ScStyle_SmallCaps);
			m_Effects |= set * ScStyle_SmallCaps;
		}
		else if (feature == SHYPHEN)
		{
			m_Effects &= ~(invert * ScStyle_HyphenationPossible);
			m_Effects |= set * ScStyle_HyphenationPossible;
		}
		else {
			qDebug("CharStyle: unknown feature: %s", feature.toLocal8Bit().constData());
		}
		
	}
}


void CharStyle::setStyle(const CharStyle& other) 
{
	other.validate();
	setParent(other.parent());
	m_contextversion = -1; 
#define ATTRDEF(attr_TYPE, attr_GETTER, attr_NAME, attr_DEFAULT, attr_BREAKSHAPING) \
	inh_##attr_NAME = other.inh_##attr_NAME; \
	m_##attr_NAME = other.m_##attr_NAME;
#include "charstyle.attrdefs.cxx"
#undef ATTRDEF
	updateFeatures();
}

void CharStyle::getNamedResources(ResourceCollection& lists) const
{
	for (const BaseStyle* sty = parentStyle(); sty != nullptr; sty = sty->parentStyle())
		lists.collectCharStyle(sty->name());
	lists.collectColor(fillColor());
	lists.collectFontfeatures(fontFeatures());
	lists.collectColor(strokeColor());
	lists.collectColor(backColor());
	lists.collectFont(font().scName());
}

void CharStyle::replaceNamedResources(ResourceCollection& newNames)
{
	QMap<QString, QString>::ConstIterator it;

	if (!inh_FillColor && (it = newNames.colors().find(fillColor())) != newNames.colors().end())
		setFillColor(it.value());

	if (!inh_FontFeatures && (it = newNames.fontfeatures().find(fontFeatures())) != newNames.fontfeatures().end())
		setFontFeatures(it.value());

	if (!inh_StrokeColor && (it = newNames.colors().find(strokeColor())) != newNames.colors().end())
		setStrokeColor(it.value());

	if (!inh_BackColor && (it = newNames.colors().find(backColor())) != newNames.colors().end())
		setBackColor(it.value());

	if (hasParent() && (it = newNames.charStyles().find(parent())) != newNames.charStyles().end())
		setParent(it.value());

	if (!inh_Font && (it = newNames.fonts().find(font().scName())) != newNames.fonts().end())
		setFont(newNames.availableFonts->findFont(it.value(), nullptr));
	updateFeatures();
}

/*
bool CharStyle::definesAll() const
{
	return definesLineSpacing() && 
	definesLeftMargin() && 
	definesRightMargin() && 
	definesFirstIndent() &&
	definesAlignment() && 
	definesGapBefore()  &&
	definesLineSpacingMode()  && 
	definesGapAfter()  && 
	definesHasDropCap() && 
	definesDropCapOffset() && 
	definesDropCapLines() && 
	definesUseBaselineGrid() && 
	charStyle().definesAll() ;
	
}

// equiv. to "*this == CharStyle()"
bool CharStyle::inheritsAll() const
{
	return inheritsLineSpacing() && 
	inheritsLeftMargin() && 
	inheritsRightMargin() && 
	inheritsFirstIndent() &&
	inheritsAlignment() && 
	inheritsGapBefore()  &&
	inheritsLineSpacingMode()  && 
	inheritsGapAfter()  && 
	inheritsHasDropCap() && 
	inheritsDropCapOffset() && 
	inheritsDropCapLines() && 
	inheritsUseBaselineGrid() && 
	charStyle().inheritsAll() ;
}
*/


Xml_string toXMLString(StyleFlag val)
{
	return toXMLString(static_cast<unsigned int>(val & ScStyle_UserStyles));
}


void CharStyle::saxx(SaxHandler& handler, const Xml_string& elemtag) const
{
	Xml_attr att;
	BaseStyle::saxxAttributes(att);
#define ATTRDEF(attr_TYPE, attr_GETTER, attr_NAME, attr_DEFAULT, attr_BREAKSHAPING) \
	if (!inh_##attr_NAME) \
		att.insert(# attr_NAME, toXMLString(m_##attr_NAME));
#include "charstyle.attrdefs.cxx"
#undef ATTRDEF
	if (!name().isEmpty())
		att["id"] = mkXMLName(elemtag + name());
	handler.begin(elemtag, att);
//	if (hasParent() && parentStyle())
//		parentStyle()->saxx(handler);	
	handler.end(elemtag);
}


template<>
StyleFlag parse<StyleFlag>(const Xml_string& str)
{
	return StyleFlag(parseInt(str));
}

template<>
ScFace parse<ScFace>(const Xml_string& str)
{
	// FIXME: enable font substitution here
	return PrefsManager::instance().appPrefs.fontPrefs.AvailFonts[str];
}


using namespace desaxe;


const Xml_string CharStyle::saxxDefaultElem("charstyle");

void CharStyle::desaxeRules(const Xml_string& prefixPattern, Digester& ruleset, const Xml_string& elemtag)
{
	Xml_string stylePrefix(Digester::concat(prefixPattern, elemtag));
	ruleset.addRule(stylePrefix, Factory<CharStyle>());
	ruleset.addRule(stylePrefix, IdRef<CharStyle>());
	BaseStyle::desaxeRules<CharStyle>(prefixPattern, ruleset, elemtag);
#define ATTRDEF(attr_TYPE, attr_GETTER, attr_NAME, attr_DEFAULT, attr_BREAKSHAPING) \
	ruleset.addRule(stylePrefix, SetAttributeWithConversion<CharStyle, attr_TYPE> ( & CharStyle::set##attr_NAME,  # attr_NAME, &parse<attr_TYPE> ));
#include "charstyle.attrdefs.cxx"
#undef ATTRDEF
}

//kate: replace-tabs 0;
