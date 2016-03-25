#ifndef TEXTSHAPER_H
#define TEXTSHAPER_H

#include <QList>
#include <QMap>
#include <unicode/uscript.h>

class GlyphRun;

class TextShaper
{
public:
	TextShaper();

	QList<GlyphRun> shapeText();

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

	QList<TextRun> itemizeBiDi(QString &text);
	QList<TextRun> itemizeScript(QList<TextRun> &runs, QString &text);
	QList<TextRun> itemizeStyles(QList<TextRun> &runs, QMap<int, int> &textMap);
};

#endif // TEXTSHAPER_H
