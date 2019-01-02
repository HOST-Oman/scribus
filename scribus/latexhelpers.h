//Based on http://doc.trolltech.com/qq/qq21-syntaxhighlighter.html#example
/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
						latexhelpers.h  -  description
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

#ifndef LATEXHELPERS_H
#define LATEXHELPERS_H

#include <QSyntaxHighlighter>
#include <QXmlStreamReader>
#include <QString>
#include <QObject>
#include <QPointer>

class LatexHighlighterRule
{
	public:
		LatexHighlighterRule(){multiline=false;}
		QRegExp regex;
		QTextCharFormat format;
		bool multiline;
};

class LatexHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT

	public:
		LatexHighlighter(QTextDocument *document);
		void setConfig(QList<LatexHighlighterRule *> *config) { m_rules = config; rehighlight();}
	protected:
		void highlightBlock(const QString &text);
	private:
		QList<LatexHighlighterRule *> *m_rules;
};

class I18nXmlStreamReader : public QXmlStreamReader
{
	public:
		I18nXmlStreamReader() : QXmlStreamReader() {}
		I18nXmlStreamReader(QIODevice *device) : QXmlStreamReader(device) {}
		QString readI18nText(bool unindent=false);
};

class LatexConfigParser
{
	public:
		LatexConfigParser() {};
		static QString absoluteFilename(QString fn);
		static QString configBase();
		bool parseConfigFile(QString fn);
		QString executable() const;
		QString imageExtension() const { return m_imageExtension; }
		QString emptyFrameText() const { return m_emptyFrameText; }
		QString preamble() const { return m_preamble; }
		QString postamble() const { return m_postamble; }
		QString description() const { return m_description; }
		QString error() const { return m_error; }
		QString icon() const { return m_icon; }
		QString filename() const { return m_filename; }
		QMap<QString,QString> properties;
		QList<LatexHighlighterRule *> highlighterRules;
	protected:
		QString m_error;
		QString m_description, m_executable, m_imageExtension, m_emptyFrameText;
		QString m_preamble, m_postamble, m_icon;
		QString m_filename;
		I18nXmlStreamReader xml;
		void formatError(const QString& message);
		void parseElements();
		void parseTab();
		void parseHighlighter();
		void ignoreList();
		bool StrRefToBool(const QStringRef &str) const;
};

class LatexConfigCache;
class LatexConfigCache {
	public:
		static LatexConfigCache* instance();
		static QStringList defaultConfigs();
		static QMap<QString, QString> defaultCommands();
		LatexConfigCache() {}
		LatexConfigParser* parser(const QString& filename, bool warnOnError = false);
		bool hasError(const QString& filename);
	protected:
		void createParser(const QString& filename, bool warnOnError);
	private:
		QMap<QString, LatexConfigParser*> m_parsers;
		QMap<QString, bool> m_error;
		static LatexConfigCache *m_instance;
};


#endif
