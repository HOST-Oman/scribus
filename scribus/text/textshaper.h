#ifndef TEXTSHAPER_H
#define TEXTSHAPER_H

#include <QList>
#include <QMap>
#include <QString>
#include <fonts/scface.h>
#include "storytext.h"
#include <unicode/uscript.h>

class GlyphCluster;
class StoryText;
class PageItem;

class TextShaper
{
public:
	TextShaper(PageItem *item, StoryText &story, int first, bool singlePar=false);
	TextShaper(QString text, ScFace &scface, int fontSize);

	QList<GlyphCluster> shape();

private:
	struct TextRun {
		TextRun(int s, int l, int d)
			: start(s), len(l), dir(d), script(USCRIPT_INVALID_CODE)
		{ }

		TextRun(int s, int l, int d, UScriptCode sc)
			: start(s), len(l), dir(d), script(sc)
		{ }

		int start;
		int len;
		int dir;
		UScriptCode script;
	};

	void buildText(QString &text, QMap<int, int> &textMap);
	QList<TextRun> itemizeBiDi(QString &text);
	QList<TextRun> itemizeScripts(QString &text, QList<TextRun> &runs);
	QList<TextRun> itemizeStyles(QMap<int, int> &textMap, QList<TextRun> &runs);

	PageItem *m_item;
	StoryText m_story;
	int m_firstChar;
	bool m_singlePar;
	QString m_text;
	ScFace m_scface;
	int m_fontSize;
	QMap<int, int> m_textMap;
};

#endif // TEXTSHAPER_H
