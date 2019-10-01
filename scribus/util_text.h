/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef _UTIL_TEXT_H
#define _UTIL_TEXT_H

#include <QString>

#include "style.h"
#include "styles/charstyle.h"
#include "styles/paragraphstyle.h"
#include "scribusapi.h"
#include "text/storytext.h"

class  ScribusDoc;

#ifndef NLS_CONFORMANCE
int SCRIBUS_API findParagraphStyle(ScribusDoc* doc, const ParagraphStyle& parStyle);
int SCRIBUS_API findParagraphStyle(ScribusDoc* doc, const QString &name);
#endif

bool SCRIBUS_API localeAwareLessThan(const QString& s1, const QString& s2);

// returns StoryText from saxed string
StoryText SCRIBUS_API desaxeString(ScribusDoc* doc, const QString& saxedString);

//returns string with saxed story
QString SCRIBUS_API saxedText(StoryText* story);

#endif
